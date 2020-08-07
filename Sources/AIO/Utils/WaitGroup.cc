#include <AIO/Utils/WaitGroup.hpp>

namespace AIO::Utils
{
void WaitGroup::Add(int incr)
{
    counter_ += incr;
}

void WaitGroup::Done()
{
    --counter_;
    if (counter_ <= 0)
        cv_.notify_all();
}

void WaitGroup::Wait()
{
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this] { return counter_ <= 0; });
}
}
