#include <AIO/Search/TreeNode.hpp>

#include <algorithm>
#include <array>

namespace AIO::Search
{
void TreeNode::Expand(const Game::Board& state, const Network::Tensor& policy)
{
    auto moveList = state.ValidMoves();
    std::transform(moveList.begin(), moveList.end(), moveList.begin(),
                   Game::PointUtil::UnextendedPt);

    float probSum = 1e-10f;
    for (const auto move : moveList)
    {
        probSum += policy[move];
    }

    std::sort(moveList.begin(), moveList.end(),
              [&policy](int a, int b) { return policy[a] > policy[b]; });

    Game::StoneColor color = state.Current();

    TreeNode* nowNode = nullptr;
    for (const auto move : moveList)
    {
        TreeNode* node = new TreeNode;
        node->color = color;
        node->action = move;
        node->policy = policy[move] / probSum;

        if (nowNode == nullptr)
            mostLeftChildNode = node;
        else
            nowNode->rightSiblingNode = node;

        nowNode = node;
    }
}
}  // namespace AIO::Search
