// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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
void probe_message(const ipc::message& message)
{
    if (message.is<ipc::message::info_launch>())
        std::cout << "$message info_launch\n";
    else if (message.is<ipc::message::info_started>())
        std::cout << "$message info_start\n";
    else if (message.is<ipc::message::update_index>()); //fmt::print("$message update_index\n");
    else if (message.is<vr::Compositor_FrameTiming>());//fmt::print("$message Compositor_FrameTiming\n");
    else if (message.is<vr::Compositor_CumulativeStats>())
        std::cout << "$message Compositor_CumulativeStats\n";
    else if (message.is<ipc::message::info_exit>())
        std::cout << "$message info_exit\n";
}

int main()
{
    auto pcondition = std::make_shared<std::condition_variable>();
    auto pmutex = std::make_shared<std::mutex>();
    const auto event_sinker = std::make_shared<sinker>(); event_sinker->filename = "event.json";
    const auto vr_sinker = std::make_shared<sinker>(); vr_sinker->filename = "openvr.json";
    const auto update_sinker = std::make_shared<sinker>(); update_sinker->filename = "update.json";
    const auto start_barrier = std::make_shared<sync::barrier>(4);    
    auto sinker_associate = [=](const size_t index) mutable->sinker&
    {
        static std::array<std::shared_ptr<sinker>, ipc::message::index_size()> associate;
        if (std::any_of(associate.cbegin(), associate.cend(), [](const auto& p) { return p == nullptr; }))
        {
            associate.fill(nullptr);
            associate.at(ipc::message::index<vr::Compositor_FrameTiming>()) = vr_sinker;
            associate.at(ipc::message::index<vr::Compositor_CumulativeStats>()) = event_sinker;
            associate.at(ipc::message::index<ipc::message::info_launch>()) = event_sinker;
            associate.at(ipc::message::index<ipc::message::info_started>()) = event_sinker;
            associate.at(ipc::message::index<ipc::message::info_exit>()) = event_sinker;
            associate.at(ipc::message::index<ipc::message::update_index>()) = update_sinker;
            associate.at(ipc::message::index<ipc::message::tagged_pack>()) = event_sinker;
            associate.at(ipc::message::index<ipc::message::first_frame_available>()) = event_sinker;
            associate.at(ipc::message::index<ipc::message::first_frame_updated>()) = event_sinker;
            core::verify(std::none_of(associate.cbegin(), associate.cend(), [](const auto& p) { return p == nullptr; }));
        }
        return *associate.at(index);
    };
    core::repeat_each([start_barrier](std::shared_ptr<sinker> psinker)
    {
        psinker->task = std::async([start_barrier, psinker]()
        {
            auto& sinker = *psinker;
            static const filesystem::path dirpath;
            static sync::barrier file_barrier{ 3, []() { const_cast<filesystem::path&>(dirpath) = make_log_directory(); } };
            while (true)
            {
                file_barrier.arrive_and_wait();
                std::cout << "file barrier\n";
                std::ofstream ofstream;
                cereal::JSONOutputArchive archive{ ofstream };
                start_barrier->arrive_and_wait();
                ofstream.open(dirpath / sinker.filename, std::ios::trunc);
                core::verify(ofstream.good());
                std::cout << "start barrier\n";
                std::unique_lock<std::mutex> exlock{ sinker.mutex,std::defer_lock };
                auto next_iteration = false;
                while (!next_iteration)
                {
                    exlock.lock();
                    sinker.condition.wait(exlock);
                    next_iteration = std::exchange(sinker.next_iteration, false);
                    auto messages = std::exchange(sinker.messages, {});
                    exlock.unlock();
                    for (auto& message1 : messages)
                        archive << std::move(message1);
                }
                ofstream.flush();
                std::cout << "finish sinker " << sinker.filename << std::endl;
            }
        });
    }, event_sinker, vr_sinker, update_sinker);
    //std::this_thread::sleep_for(20s);
    ipc::channel recv_ch{ false };
    while (true)
    {
        auto result = recv_ch.async_receive();
        auto message = result.first.get();
        std::chrono::duration<double, std::milli> timing = message.timing();
        std::cout << "*received message, index" << message.index() << " timing" << timing.count() << "ms*\n";
        if (message.is<ipc::message::info_launch>())
        {
            start_barrier->arrive_and_wait();
            std::cout << "start barrier 123123\n";
        }
        auto& sinker = sinker_associate(message.index());
        {
            std::lock_guard<std::mutex> exlock{ sinker.mutex };
            sinker.messages.push_back(std::move(message));
            if (message.is<ipc::message::info_exit>())
                sinker.next_iteration = true;
        }
        if (message.is<ipc::message::info_exit>())
        {
            std::scoped_lock<std::mutex, std::mutex> exlocks{ vr_sinker->mutex,update_sinker->mutex };
            vr_sinker->next_iteration = true;
            vr_sinker->condition.notify_one();
            update_sinker->next_iteration = true;
            update_sinker->condition.notify_one();
        }
        sinker.condition.notify_one();
        probe_message(message);
    }
    std::cout << "$$receiving finish$$\n";
    return 0;
}