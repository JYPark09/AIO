#ifndef AIO_BARRIER_HPP
#define AIO_BARRIER_HPP

#include <condition_variable>
#include <cstddef>
#include <mutex>

namespace AIO::Utils
{
class Barrier final
{
 public:
    explicit Barrier(std::size_t count);

    void Done();
    void Wait();

 private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::size_t count_;
};
}  // namespace AIO::Utils

#endif  // AIO_BARRIER_HPP
