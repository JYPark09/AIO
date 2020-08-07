#include <AIO/Search/SearchManager.hpp>

namespace AIO::Search
{
SearchManager::SearchManager(std::size_t threadNum) : threadNum_(threadNum)
{
    // Do nothing
}

void SearchManager::Pause()
{
    resumeGroup_.Wait();

    {
        std::lock_guard<std::mutex> lock(mutex_);

        state_ = SearchState::PAUSE;
    }
    pauseGroup_.Wait();
}

void SearchManager::AckPause()
{
    pauseGroup_.Done();
}

void SearchManager::Resume()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (state_ == SearchState::SEARCHING)
            return;

        resumeGroup_.Add(threadNum_);
        pauseGroup_.Add(threadNum_);

        state_ = SearchState::SEARCHING;
    }

    cv_.notify_all();
    resumeGroup_.Wait();
}

void SearchManager::Terminate()
{
    Pause();

    {
        std::lock_guard<std::mutex> lock(mutex_);

        state_ = SearchState::TERMINATE;
    }

    cv_.notify_all();
}

bool SearchManager::WaitResume()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(lock, [this] { return state_ != SearchState::PAUSE; });

        if (state_ == SearchState::TERMINATE)
            return false;
    }
    
    resumeGroup_.Done();
    return true;
}

SearchState SearchManager::GetState() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return state_;
}

}  // namespace AIO::Search
