#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "maze.h"
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <algorithm>

/**
 * 路径寻找器类 - 实现多种迷宫路径寻找算法
 * 功能：
 * 1. 深度优先搜索（DFS）寻找路径
 * 2. 广度优先搜索（BFS）寻找最短路径
 * 3. A*算法寻找最优路径
 * 4. 返回路径结果和统计信息
 */
class PathFinder {
public:
    // 路径搜索结果结构
    struct SearchResult {
        bool found;                         // 是否找到路径
        std::vector<Point> path;      // 找到的路径
        int steps;                          // 路径步数
        int visitedNodes;                   // 访问的节点数量
        double searchTime;                  // 搜索耗时（毫秒）
        std::string algorithm;              // 使用的算法名称
        
        SearchResult() : found(false), steps(0), visitedNodes(0), searchTime(0.0) {}
    };

private:
    // A*算法中的节点结构
    struct AStarNode {
        Point point;
        int gCost;  // 从起点到当前点的实际代价
        int hCost;  // 从当前点到终点的启发式代价
        int fCost;  // gCost + hCost
        AStarNode* parent;
        
        AStarNode(const Point& p, int g = 0, int h = 0, AStarNode* par = nullptr)
            : point(p), gCost(g), hCost(h), fCost(g + h), parent(par) {}
        
        bool operator>(const AStarNode& other) const {
            return fCost > other.fCost;
        }
    };

    // 哈希函数用于unordered_map
    struct PointHash {
        std::size_t operator()(const Point& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };

public:
    // 构造函数
    PathFinder() = default;
    
    // 深度优先搜索
    SearchResult findPathDFS(Maze& maze);
    
    // 广度优先搜索（最短路径）
    SearchResult findPathBFS(Maze& maze);
    
    // A*算法
    SearchResult findPathAStar(Maze& maze);
    
    // 找到所有可能的路径（限制最大路径数量以避免指数爆炸）
    std::vector<std::vector<Point>> findAllPaths(Maze& maze, int maxPaths = 100);
    
    // 比较不同算法的性能
    void compareAlgorithms(Maze& maze);
    
    // 打印搜索结果
    static void printSearchResult(const SearchResult& result);

private:
    // DFS递归辅助函数
    bool dfsHelper(Maze& maze, const Point& current, const Point& target,
                   std::vector<Point>& path, std::vector<std::vector<bool>>& visited,
                   int& visitedCount);
    
    // 计算曼哈顿距离（A*算法的启发式函数）
    int manhattanDistance(const Point& a, const Point& b);
    
    // 重构路径（从A*算法的结果中提取路径）
    std::vector<Point> reconstructPath(AStarNode* endNode);
    
    // 找到所有路径的DFS辅助函数
    void findAllPathsDFS(Maze& maze, const Point& current, const Point& target,
                        std::vector<Point>& currentPath, 
                        std::vector<std::vector<Point>>& allPaths,
                        std::vector<std::vector<bool>>& visited,
                        int maxPaths);
};

#endif // PATHFINDER_H
