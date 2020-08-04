// ReSharper disable All
#include <AIO/Search/SearchEngine.hpp>

#include <AIO/Network/FakeNetwork.hpp>
#include <AIO/Utils/Utils.hpp>

#include <effolkronium/random.hpp>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>

#include <spdlog/spdlog.h>

namespace AIO::Search
{
SearchEngine::SearchEngine(SearchOptions option)
    : option_(option), manager_(option.NumSearchThreads)
{
    networkBarrier_.Init(option_.NumEvalThreads);
    for (int threadId = 0; threadId < option_.NumEvalThreads; ++threadId)
    {
        std::unique_ptr<Network::Network> network;

        network = std::make_unique<Network::FakeNetwork>();

        evalThreads_.emplace_back(&SearchEngine::evalThread, this, threadId,
                                  std::move(network));
    }

    for (int threadId = 0; threadId < option_.NumSearchThreads; ++threadId)
    {
        searchThreads_.emplace_back(&SearchEngine::searchThread, this,
                                    threadId);
    }

    deleteThread_ = std::thread(&SearchEngine::deleteThread, this);

    networkBarrier_.Wait();

    updateRoot(nullptr);

    spdlog::info("search engine initialize done");
}

SearchEngine::~SearchEngine()
{
    manager_.Terminate();
    for (auto& t : searchThreads_)
        if (t.joinable())
            t.join();

    runningEvalThread_ = false;
    for (auto& t : evalThreads_)
        if (t.joinable())
            t.join();

    runningDeleteThread_ = false;
    if (deleteThread_.joinable())
        deleteThread_.join();
}

void SearchEngine::Search()
{
    resumeSearch();

    bool stopFlag = false;
    while (!stopFlag)
    {
        stopFlag |= (numOfSimulations_ >= option_.MaxSimulations);
    }

    pauseSearch();
}

void SearchEngine::Play(Game::Point pt)
{
    pauseSearch();

    mainBoard_.Play(pt);

    TreeNode* newRoot = nullptr;
    for (TreeNode* tempNowNode = root_->mostLeftChildNode;
         tempNowNode != nullptr; tempNowNode = tempNowNode->rightSiblingNode)
    {
        if (tempNowNode->action == pt)
        {
            newRoot = tempNowNode;
            break;
        }
    }
    updateRoot(newRoot);
}

void SearchEngine::DumpStats() const
{
    std::vector<TreeNode*> children;
    for (TreeNode* tempNowNode = root_->mostLeftChildNode;
         tempNowNode != nullptr; tempNowNode = tempNowNode->rightSiblingNode)
    {
        children.emplace_back(tempNowNode);
    }

    std::sort(children.begin(), children.end(),
              [](TreeNode* a, TreeNode* b) { return a->visits > b->visits; });

    std::cerr << "root value: " << (root_->values / root_->visits) << '\n'
              << "total simulation: " << numOfSimulations_ << '\n'
              << "root visits: " << root_->visits << '\n';

    for (const TreeNode* child : children)
    {
        if (child->visits == 0)
            break;

        std::cerr << std::right << std::setw(5)
                  << Game::PointUtil::PointStr(child->action)
                  << " : (N: " << child->visits
                  << ") (Q: " << (child->values / child->visits)
                  << ") (P: " << child->policy << ") -> ";

        while (true)
        {
            std::cerr << Game::PointUtil::PointStr(child->action) << ' ';

            if (child->state == ExpandState::EXPANDED)
                child = child->GetMaxVisitedChild();
            else
                break;
        }

        std::cerr << '\n';
    }
}

Game::Point SearchEngine::GetBestMove() const
{
    const TreeNode* bestNode = getBestNode();

    if (bestNode == nullptr)
        return Game::PASS;

    return getBestNode()->action;
}

const TreeNode* SearchEngine::GetRoot() const noexcept
{
    return root_;
}

TreeNode* SearchEngine::getBestNode()
{
    return const_cast<TreeNode*>(std::as_const(*this).getBestNode());
}

const TreeNode* SearchEngine::getBestNode() const
{
    if (root_ == nullptr)
        return nullptr;

    return root_->GetMaxVisitedChild();
}

void SearchEngine::updateRoot(TreeNode* newNode)
{
    TreeNode* node;
    if (newNode == nullptr)
    {
        node = new TreeNode;
        node->color = mainBoard_.Opponent();
    }
    else
    {
        node = new TreeNode(std::move(*newNode));

        for (TreeNode* tempNowNode = node->mostLeftChildNode;
             tempNowNode != nullptr;
             tempNowNode = tempNowNode->rightSiblingNode)
            node->parentNode = node;

        node->parentNode = nullptr;
    }

    if (root_ != nullptr)
        enqDelete(root_);

    root_ = node;
}

void SearchEngine::initRoot()
{
    if (root_->state == ExpandState::UNEXPANDED)
    {
        Network::Tensor policy;
        [[maybe_unused]] float value;
        evaluate(mainBoard_, policy, value);

        root_->Expand(mainBoard_, policy);
    }

    if (option_.EnableDirichletNoise)
    {
        std::gamma_distribution<float> dist(option_.DirichletNoiseAlpha);
        std::array<float, Game::BOARD_SIZE> noise;

        for (std::size_t i = 0; i < Game::BOARD_SIZE; ++i)
            noise[i] = effolkronium::random_static::get(dist);

        const float noiseSum = std::accumulate(
            noise.begin(), noise.begin() + root_->numChildren, 1e-10f);

        float total = 1e-10f;
        std::size_t idx = 0;
        for (TreeNode* child = root_->mostLeftChildNode; child != nullptr;
             child = child->rightSiblingNode, ++idx)
        {
            child->policy =
                (option_.DirichletNoiseEps) * child->policy +
                (1.f - option_.DirichletNoiseEps) * noise[idx] / noiseSum;

            total += child->policy;
        }

        for (TreeNode* child = root_->mostLeftChildNode; child != nullptr;
             child = child->rightSiblingNode)
            child->policy /= total;
    }
}

void SearchEngine::pauseSearch()
{
    if (manager_.GetState() == SearchState::SEARCHING)
    {
        manager_.Pause();
        spdlog::info("pause search");
    }
}

void SearchEngine::resumeSearch()
{
    if (manager_.GetState() == SearchState::PAUSE)
    {
        numOfSimulations_ = 0;
        initRoot();

        manager_.Resume();
        spdlog::info("resume search");
    }
}

void SearchEngine::evaluate(const Game::Board& state, Network::Tensor& policy,
                            float& value)
{
    EvalTask task;
    task.state = Network::StateToTensor(state);
    auto fut = task.task.get_future();

    {
        std::lock_guard<std::mutex> lock(evalMutex_);

        evalTasks_.emplace_back(std::move(task));
    }

    auto [predPolicy, predValue] = fut.get();

    policy = std::move(predPolicy);
    value = predValue;
}

void SearchEngine::enqDelete(TreeNode* node)
{
    std::lock_guard<std::mutex> lock(deleteMutex_);

    deleteTasks_.emplace_back(node);
}

void SearchEngine::evalThread(int threadId,
                              std::unique_ptr<Network::Network> network)
{
    spdlog::info("[eval thread {}] initialize start", threadId);
    network->Initialize(option_.WeightFileName);

    spdlog::info("[eval thread {}] initialize ended", threadId);
    networkBarrier_.Done();

    while (runningEvalThread_)
    {
        std::vector<Network::Tensor> inputs;
        std::vector<std::promise<NetEval>> results;

        std::size_t batchSize;

        {
            std::lock_guard<std::mutex> lock(evalMutex_);

            batchSize =
                std::min<std::size_t>(option_.BatchSize, evalTasks_.size());
            for (std::size_t batch = 0; batch < batchSize; ++batch)
            {
                EvalTask task = std::move(evalTasks_.front());
                evalTasks_.pop_front();

                inputs.emplace_back(std::move(task.state));
                results.emplace_back(std::move(task.task));
            }
        }

        if (batchSize == 0)
            continue;

        std::vector<Network::Tensor> policy(batchSize);
        Network::Tensor value(batchSize);

        network->Evaluate(inputs, policy, value);

        for (std::size_t batch = 0; batch < batchSize; ++batch)
        {
            results[batch].set_value(
                { std::move(policy[batch]), value[batch] });
        }
    }

    spdlog::info("[eval thread {}] shutdown", threadId);
}

// ReSharper disable once CppInconsistentNaming
void SearchEngine::searchThread(int threadId)
{
    if (!manager_.WaitResume())
    {
        spdlog::info("[search thread {}] shutdown", threadId);
        return;
    }

    spdlog::info("[search thread {}] start searching loop", threadId);

    while (true)
    {
        if (manager_.GetState() != SearchState::SEARCHING)
        {
            manager_.AckPause();
            if (!manager_.WaitResume())
            {
                break;
            }
        }

        // Searching routine
        Game::Board bd(mainBoard_);

        TreeNode* tempNowNode = root_;
        while (tempNowNode->state == ExpandState::EXPANDED)
        {
            tempNowNode = tempNowNode->Select(option_);

            bd.Play(tempNowNode->action);
            Utils::AtomicAdd(tempNowNode->virtualLoss, option_.VirtualLoss);
        }

        const Game::StoneColor current = bd.Opponent();

        float valueToUpdate = 0;
        if (bd.IsEnd())
        {
            const Game::StoneColor winner = bd.GetWinner();

            if (winner == current)
                valueToUpdate = 1;
            else if (winner != Game::P_NONE)
                valueToUpdate = -1;
        }
        else
        {
            Network::Tensor policy;
            evaluate(bd, policy, valueToUpdate);

            tempNowNode->Expand(bd, policy);
        }

        while (tempNowNode != nullptr)
        {
            Utils::AtomicAdd(tempNowNode->values, valueToUpdate);
            ++tempNowNode->visits;

            if (tempNowNode != root_)
                Utils::AtomicAdd(tempNowNode->virtualLoss,
                                 -option_.VirtualLoss);

            tempNowNode = tempNowNode->parentNode;

            valueToUpdate = -valueToUpdate;
        }

        ++numOfSimulations_;
    }

    spdlog::info("[search thread {}] shutdown", threadId);
}

void SearchEngine::deleteThread()
{
    const auto& deleteNode = [](TreeNode* node) {
        static void (*impl)(TreeNode*) = [](TreeNode* node) {
            for (TreeNode* tempNowNode = node->mostLeftChildNode;
                 tempNowNode != nullptr;
                 tempNowNode = tempNowNode->rightSiblingNode)
                impl(tempNowNode);

            TreeNode* tempNowNode = node->mostLeftChildNode;
            TreeNode* nodeToDelete = nullptr;
            while (tempNowNode != nullptr)
            {
                nodeToDelete = tempNowNode;
                tempNowNode = tempNowNode->rightSiblingNode;

                delete nodeToDelete;
            }

            node->mostLeftChildNode = nullptr;
        };

        impl(node);
    };

    spdlog::info("[delete thread] start deleting loop");

    while (runningDeleteThread_)
    {
        std::lock_guard<std::mutex> lock(deleteMutex_);

        if (deleteTasks_.empty())
            continue;

        TreeNode* nodeToDelete = deleteTasks_.front();
        deleteTasks_.pop_front();

        deleteNode(nodeToDelete);
        delete nodeToDelete;
    }

    spdlog::info("[delete thread] shutdown");
}
}  // namespace AIO::Search
