// Plugin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gallery/interface.h"
namespace filesystem = std::experimental::filesystem;
namespace metric
{
    void evaluate_playback(const filesystem::path& logfile)
    {
        core::verify(is_regular_file(logfile));
        std::ifstream ifs{ logfile };
        std::string line;
        std::vector<double> frame_timings;
        frame_timings.reserve(7200);
        while (std::getline(ifs, line))
        {
            size_t pos = 0;
            const auto keyword = std::string_view{ "\"SystemTimeInSeconds\": " };
            if (pos = line.find(keyword.data()); pos == std::string::npos) continue;
            pos += keyword.size();
            const auto pos_end = line.find(',', pos);
            static std::stringstream convert;
            convert.str(std::string_view{ std::next(line.data(),pos), pos_end - pos }.data());
            convert.clear();
            double value = 0;
            if (convert >> value; value == 0) continue;
            frame_timings.push_back(value);
        }
        //const auto iter_end = std::unique(frame_timings.begin(), frame_timings.end());
        //frame_timings.erase(iter_end, frame_timings.end());
        std::sort(frame_timings.begin(), frame_timings.end());
        //const auto min_max = std::minmax_element(frame_timings.begin(), frame_timings.end());
        std::vector<double> frame_latency;
        frame_latency.reserve(frame_timings.size());
        const auto fps = std::divides<void>{}(frame_timings.size() - 1, frame_timings.back() - frame_timings.front());
        std::adjacent_difference(frame_timings.begin(), frame_timings.end(), std::back_inserter(frame_latency));
        const auto stuck = std::count_if(frame_latency.begin() + 1, frame_latency.end(),
            [fps](const auto latency) { return fps * latency > 2; });
#pragma warning(push)
#pragma warning(disable:4244)
        const auto stuck_percent = std::divides<double>{}(stuck, frame_timings.size());
#pragma warning(pop)
    }
}
/*namespace sync
{
    // thread-safe lock-free asynchronous task chain
	class chain
	{
		std::shared_ptr<std::future<void>> pending_;
        std::atomic<uint64_t> id_ = 0;
	public:
        chain() = default;
        chain(const chain&) = delete;
        chain& operator=(const chain&) = delete;
		template<typename Callable>
		void append(Callable&& callable)
		{
            std::promise<decltype(pending_)::element_type*> promise;
            auto sfuture = promise.get_future().share();
            decltype(pending_) pending_new = nullptr;
            static thread_local std::vector<decltype(pending_)> temporary;
            auto id = id_.fetch_add(1, std::memory_order_acquire);
            auto pending_old = std::atomic_load_explicit(&pending_, std::memory_order_relaxed);
			do
			{
				pending_new = std::make_shared<decltype(pending_)::element_type>(
					std::async([pending_old, sfuture, callable = std::forward<Callable>(callable)]() mutable
				{
					sfuture.wait();
                    if (pending_old.get() != sfuture.get()) return;
                    if (pending_old) pending_old->get(); 
					std::invoke(callable);
				}));
				temporary.push_back(pending_new);
            } while (!std::atomic_compare_exchange_strong_explicit(&pending_, &pending_old, pending_new, 
                std::memory_order_acq_rel, std::memory_order_relaxed));
			promise.set_promise_through(pending_old.get());
            temporary.clear();
		}
        void wait() const
		{
            const auto pending_old = std::atomic_load_explicit(&pending_, std::memory_order_acquire);
            if (pending_old) pending_old->wait();
		}
	};
}*/
int main(int argc, char* argv[])
{
    //metric::evaluate_playback("D:/vr_log/log@21_51_34 8k h264/openvr.json");
    //metric::evaluate_playback("D:/vr_log/log@21_56_4 4k h264/openvr.json");
    //metric::evaluate_playback("D:/vr_log/log@22_5_1 4k h264 2/openvr.json");
    //metric::evaluate_playback("D:/vr_log/log@22_15_21 8k hevc/openvr.json");
    //metric::evaluate_playback("D:/vr_log/log@22_10_11 4k hevc/openvr.json");
/*    sync::chain chain;
    auto thread_num = std::thread::hardware_concurrency() * 2;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock{ mutex };
    std::atomic<decltype(thread_num)> count = 0;
    std::atomic<uint64_t> fvck_count = 0;
    std::condition_variable cond;
    for (auto i = 0; i != thread_num; ++i)
    {
        std::thread{ [&] {
            std::vector<std::future<int>> temp; temp.reserve(1000);
            for (auto j = 0; j != 1000; ++j)
            {
                temp.push_back(
                chain.append([&] {
                    std::cout << "laji";
                    fvck_count.fetch_add(1, std::memory_order_relaxed);
                    return 1;
                } ,sync::use_future));
            }
            std::cout << "\nfinish";
            if (count.fetch_add(1) == thread_num - 1)
                cond.notify_one();
        } }.detach();
    }
    cond.wait(lock);
    chain.abort_and_wait();*/
    
    dll::interprocess_create();
    dll::media_create();
    StoreMediaUrl("D:/Media/NewYork.mp4");
    int w = 0, h = 0;
    LoadVideoParams(w, h);
    while(IsVideoAvailable())
    {
        auto frame = dll::media_extract_frame();
        static auto count = 0;
        fmt::print("count {}\n", ++count);
    }
    dll::media_release();
    dll::interprocess_release();
    int dummy = 1;
#if 0
    auto aa = std::thread{ []() mutable{
        auto time_mark = std::chrono::high_resolution_clock::now();
        ipc::channel send_ch{ true };
        auto count = -1;
        while (++count != 2000) {
            fmt::print("!sending {} message!\n", count);
            auto duration = std::chrono::high_resolution_clock::now() - time_mark;
            auto timing = vr::Compositor_CumulativeStats{};
            auto msg = ipc::message{ std::move(timing), std::move(duration) };
            send_ch.async_send(timing, duration);
        }
        fmt::print("!!sending finish!!\n");
        std::this_thread::sleep_for(1h);
        fmt::print("!!sending finish future!!\n");
    } };
    aa.join();
#endif
}

