// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <random>

namespace
{
    const filesystem::path root_path{ "C:/VrLog" };
}

filesystem::path make_log_directory()
{   
    const auto dirname = core::time_string("%b%d_%H%M%S"sv);
    const auto dirpath = root_path / dirname;
    if (!is_directory(dirpath))
    {
        core::verify(is_directory(dirpath.root_path()));
        remove_all(dirpath);
        core::verify(create_directories(dirpath));
    }
    return dirpath;
}

struct sinker
{
    std::vector<ipc::message> messages;
    std::mutex mutex;
    std::condition_variable condition;
    std::future<void> task;
    std::string filename;
    bool next_iteration = false;
};

struct duration_notation
{
    const std::chrono::high_resolution_clock::duration& dura;
    friend std::ostream& operator<<(std::ostream& os, const duration_notation& dn)
    {
        using namespace std::chrono;
        return 
            dn.dura < 1ms ? os << duration_cast<duration<double, std::micro>>(dn.dura).count() << "us" :
            dn.dura < 1s ? os << duration_cast<duration<double, std::milli>>(dn.dura).count() << "ms" :
            dn.dura < 1min ? os << duration_cast<duration<double>>(dn.dura).count() << "s" :
            dn.dura < 1h ? os << duration_cast<duration<double, std::ratio<60>>>(dn.dura).count() << "min" :
            os << duration_cast<duration<double, std::ratio<3600>>>(dn.dura).count() << "h";
    }
};

void probe_critical_message(const ipc::message& message)
{
    if (!message.is<ipc::info_launch>() && !message.is<ipc::info_started>() && !message.is<ipc::info_exit>() && !message.is<ipc::media_format>()
        && !message.is<ipc::first_frame_available>() && !message.is<ipc::first_frame_updated>() && !message.is<vr::Compositor_CumulativeStats>())
        return;
    std::cout 
        << termcolor::green << "[critical] " << message.description() << " "
        << duration_notation{ message.timing() } << termcolor::reset << "\n";
}

int main()
{
    const auto pcondition = std::make_shared<std::condition_variable>();
    const auto pmutex = std::make_shared<std::mutex>();
    const auto event_sinker = std::make_shared<sinker>(); event_sinker->filename = "event.json";
    const auto vr_sinker = std::make_shared<sinker>(); vr_sinker->filename = "openvr.json";
    const auto update_sinker = std::make_shared<sinker>(); update_sinker->filename = "update.json";
    const auto dirpath = std::make_shared<const filesystem::path>();
    std::cout << termcolor::cyan << "[Debug] console launched, default logging root directory at " << root_path.generic_string() << " folder" << termcolor::reset << "\n";
    const auto start_barrier = std::make_shared<sync::barrier>(4, [dirpath]()
    {
        const_cast<filesystem::path&>(*dirpath) = make_log_directory();
        std::cout << termcolor::cyan << "[Debug] new logs created at " << dirpath->generic_string() << " folder" << termcolor::reset << "\n";
    });
    auto sinker_associate = [=](const size_t index) mutable->sinker&
    {
        static std::array<std::shared_ptr<sinker>, ipc::message::index_size()> associate;
        //if (std::any_of(associate.cbegin(), associate.cend(), [](const auto& p) { return p == nullptr; }))
        if (!associate.front())
        {
            associate.fill(nullptr);
            associate.at(ipc::message::index<vr::Compositor_FrameTiming>()) = vr_sinker;
            associate.at(ipc::message::index<vr::Compositor_CumulativeStats>()) = event_sinker;
            associate.at(ipc::message::index<ipc::info_launch>()) = event_sinker;
            associate.at(ipc::message::index<ipc::info_started>()) = event_sinker;
            associate.at(ipc::message::index<ipc::info_exit>()) = event_sinker;
            associate.at(ipc::message::index<ipc::media_format>()) = event_sinker;
            associate.at(ipc::message::index<ipc::update_index>()) = update_sinker;
            associate.at(ipc::message::index<ipc::tagged_pack>()) = event_sinker;
            associate.at(ipc::message::index<ipc::first_frame_available>()) = event_sinker;
            associate.at(ipc::message::index<ipc::first_frame_updated>()) = event_sinker;
            core::verify(std::none_of(associate.cbegin(), associate.cend(), [](const auto& p) { return p == nullptr; }));
        }
        return *associate.at(index);
    };
    core::repeat_each([dirpath, start_barrier](std::shared_ptr<sinker> psinker)
    {
        psinker->task = std::async([&dirpath, start_barrier, psinker]()
        {
            auto& sinker = *psinker;
            while (true)
            {
                start_barrier->arrive_and_wait();
                std::cout << "file barrier\n";
                std::ofstream ofstream;
                cereal::JSONOutputArchive archive{ ofstream };
                ofstream.open(*dirpath / sinker.filename, std::ios::trunc);
                core::verify(ofstream.good());
                std::unique_lock<std::mutex> exlock{ sinker.mutex,std::defer_lock };
                auto next_iteration = false;
                while (!next_iteration)
                {
                    exlock.lock();
                    sinker.condition.wait(exlock);
                    next_iteration = std::exchange(sinker.next_iteration, false);
                    auto messages = std::exchange(sinker.messages, {});
                    exlock.unlock();
                    for (auto& msg : messages)
                        archive << std::move(msg);
                }
                ofstream << std::flush; //_
                std::cout << "finish sinker " << sinker.filename << "\n";
            }
        });
    }, event_sinker, vr_sinker, update_sinker);
    ipc::channel recv_channel{ false };
    std::optional<std::chrono::duration<double>> start_time;
    uint64_t count_frame_timing = 0, count_gpu_update = 0;
    while (true)
    {
        auto message = recv_channel.receive();
        if (message.is<ipc::media_format>())            //if (message.is<ipc::message::info_launch>())
        {
            start_barrier->arrive_and_wait();
            start_time = message.timing();
        }
        else if (message.is<ipc::update_index>())
            ++count_frame_timing;
        else if (message.is<vr::Compositor_FrameTiming>())
            ++count_gpu_update;
        if (std::chrono::duration<double> current_time = message.timing(); !start_time.has_value())
            start_time = current_time;
        else if (current_time - *start_time > 1s && (count_frame_timing || count_gpu_update))
        {
            std::cout 
                << "[info] message " << start_time->count() << "sec -> " << current_time.count() << "sec, "
                << "gpu update " << count_gpu_update << " / openvr frame timing " << count_frame_timing << "\n";
            start_time = std::nullopt;
        }        
        probe_critical_message(message);
        auto& sinker = sinker_associate(message.index());
        {
            std::lock_guard<std::mutex> exlock{ sinker.mutex };
            sinker.messages.push_back(std::move(message));
            if (message.is<ipc::info_exit>())
                sinker.next_iteration = true;
        }
        if (message.is<ipc::info_exit>())
        {
            count_frame_timing = 0; count_gpu_update = 0;
            std::scoped_lock<std::mutex, std::mutex> exlocks{ vr_sinker->mutex,update_sinker->mutex };
            vr_sinker->next_iteration = true;
            vr_sinker->condition.notify_one();
            update_sinker->next_iteration = true;
            update_sinker->condition.notify_one();
        }
        sinker.condition.notify_one();
    }
    return 0;
}