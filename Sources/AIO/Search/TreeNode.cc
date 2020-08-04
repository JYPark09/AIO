#include <AIO/Search/TreeNode.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>

namespace AIO::Search
{
TreeNode::TreeNode(TreeNode&& other) noexcept
    : action(other.action),
      color(other.color),
      numChildren(other.numChildren),
      policy(other.policy),
      parentNode(other.parentNode),
      mostLeftChildNode(other.mostLeftChildNode),
      rightSiblingNode(other.rightSiblingNode)
{
    state = other.state.load();

    visits = other.visits.load();
    values = other.values.load();
    virtualLoss = other.virtualLoss.load();
}

TreeNode* TreeNode::Select(const SearchOptions& opt) const
{
    assert(mostLeftChildNode != nullptr);

    float totalParentVisits = 0;
    for (TreeNode* tempNowNode = mostLeftChildNode; tempNowNode != nullptr;
         tempNowNode = tempNowNode->rightSiblingNode)
    {
        totalParentVisits += tempNowNode->visits;
    }

    float maxValue = -FLT_MAX;
    TreeNode* bestNode = nullptr;

    for (TreeNode* tempNowNode = mostLeftChildNode; tempNowNode != nullptr;
         tempNowNode = tempNowNode->rightSiblingNode)
    {
        float Q;
        // calculate Qvalue
        {
            const float v = tempNowNode->visits;
            const float w = tempNowNode->values;
            const float vl = tempNowNode->virtualLoss;

            Q = (w - vl - 1.f) / (v + vl + 1.f);
        }

        float u;
        // calculate uValue
        {
            const float p = tempNowNode->policy;
            const float v = tempNowNode->visits;

            u = opt.cPUCT * p * std::sqrtf(totalParentVisits) / (1.f + v);
        }

        const float value = Q + u;

        if (maxValue < value)
        {
            maxValue = value;
            bestNode = tempNowNode;
        }
    }

    assert(bestNode != nullptr);
    return bestNode;
}

void TreeNode::Expand(const Game::Board& state, const Network::Tensor& policy)
{
    {
        if (this->state != ExpandState::UNEXPANDED)
            return;

        ExpandState expected = ExpandState::UNEXPANDED;
        if (!this->state.compare_exchange_weak(expected,
                                               ExpandState::EXPANDING))
            return;
    }

    auto moveList = state.ValidMoves();

    float probSum = 1e-10f;
    for (const auto move : moveList)
    {
        probSum += policy[Game::PointUtil::UnextendedPt(move)];
    }

    std::sort(moveList.begin(), moveList.end(), [&policy](int a, int b) {
        return policy[Game::PointUtil::UnextendedPt(a)] >
               policy[Game::PointUtil::UnextendedPt(b)];
    });

    const Game::StoneColor color = state.Current();

    TreeNode* nowNode = nullptr;
    for (const auto move : moveList)
    {
        TreeNode* node = new TreeNode;
        node->color = color;
        node->action = move;
        node->policy = policy[Game::PointUtil::UnextendedPt(move)] / probSum;

        if (nowNode == nullptr)
            mostLeftChildNode = node;
        else
            nowNode->rightSiblingNode = node;

        ++node->numChildren;
        node->parentNode = this;
        nowNode = node;
    }

    this->state = ExpandState::EXPANDED;
}

TreeNode* TreeNode::GetMaxVisitedChild() const
{
    int maxVisits = -INT_MAX;
    TreeNode* maxNode = nullptr;

    for (TreeNode* tempNowNode = mostLeftChildNode; tempNowNode != nullptr;
         tempNowNode = tempNowNode->rightSiblingNode)
    {
        const int v = tempNowNode->visits;

        if (maxVisits < v)
        {
            maxVisits = v;
            maxNode = tempNowNode;
        }
    }

    assert(maxNode != nullptr);
    return maxNode;
}
}  // namespace AIO::Search
