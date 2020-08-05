#include <AIO/Search/TreeNode.hpp>

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>

namespace AIO::Search
{
TreeNode::TreeNode(TreeNode&& other) noexcept
    : state(other.state.load()),
      action(other.action),
      color(other.color),
      numChildren(other.numChildren),
      policy(other.policy),
      visits(other.visits.load()),
      values(other.values.load()),
      virtualLoss(other.virtualLoss.load()),
      parentNode(other.parentNode),
      mostLeftChildNode(other.mostLeftChildNode),
      rightSiblingNode(other.rightSiblingNode)
{
    other.state = ExpandState::UNEXPANDED;
    other.mostLeftChildNode = nullptr;
    other.numChildren = 0;
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

            u = opt.cPUCT * p * std::sqrt(totalParentVisits) / (1.f + v);
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

    const Game::StoneColor color = state.Current();
    auto moveList = state.ValidMoves();

    if (!moveList.empty())
    {
        float probSum = 1e-10f;
        for (const auto move : moveList)
        {
            probSum += policy[Game::PointUtil::UnextendedPt(move)];
        }

        std::sort(moveList.begin(), moveList.end(), [&policy](int a, int b) {
            return policy[Game::PointUtil::UnextendedPt(a)] >
                   policy[Game::PointUtil::UnextendedPt(b)];
        });

        TreeNode* nowNode = nullptr;
        for (const auto move : moveList)
        {
            TreeNode* node = new TreeNode;
            node->color = color;
            node->action = move;
            node->policy =
                policy[Game::PointUtil::UnextendedPt(move)] / probSum;

            if (nowNode == nullptr)
                mostLeftChildNode = node;
            else
                nowNode->rightSiblingNode = node;

            ++numChildren;
            node->parentNode = this;
            nowNode = node;
        }
    }
    else
    {
        TreeNode* node = new TreeNode;
        node->color = color;
        node->action = Game::PASS;
        node->policy = 1.f;
        node->parentNode = this;

        ++numChildren;
        mostLeftChildNode = node;
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
