#ifndef AIO_WAIT_GROUP_HPP
#define AIO_WAIT_GROUP_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace AIO::Utils
{
class WaitGroup final
{
 public:
    void Add(int incr = 1);
    void Done();
    void Wait();

 private:
    std::mutex mutex_;
    std::atomic<int> counter_{ 0 };
    std::condition_variable cv_;
};
}  // namespace AIO::Utils

#endif  // AIO_WAIT_GROUP_HPP
