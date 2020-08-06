#include <AIO/Utils/Barrier.hpp>

namespace AIO::Utils
{
void Barrier::Init(std::size_t count)
{
    std::lock_guard<std::mutex> lock(mutex_);

    count_ = count;
}

void Barrier::Done()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        --count_;
        if (count_ < 0)
            throw std::runtime_error("counter cannot be nagative");
    }

    if (count_ == 0)
        cv_.notify_all();
}

void Barrier::Wait()
{
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [&] { return count_ == 0; });
}
}  // namespace AIO::Utils
