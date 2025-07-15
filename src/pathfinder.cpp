#include "pathfinder.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <climits>

/**
 * 路径寻找器的实现
 * 实现了DFS、BFS和A*三种路径寻找算法
 */

PathFinder::SearchResult PathFinder::findPathDFS(Maze& maze) {
    auto start = std::chrono::high_resolution_clock::now();
    
    SearchResult result;
    result.algorithm = "深度优先搜索(DFS)";
    
    // 创建访问标记矩阵
    std::vector<std::vector<bool>> visited(maze.getRows(), 
                                         std::vector<bool>(maze.getCols(), false));
    
    std::vector<Point> path;
    int visitedCount = 0;
    
    // 从入口开始搜索
    Point entrance = maze.getEntrance();
    Point exit = maze.getExit();
    
    bool found = dfsHelper(maze, entrance, exit, path, visited, visitedCount);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    result.found = found;
    result.path = path;
    result.steps = found ? path.size() - 1 : 0;
    result.visitedNodes = visitedCount;
    result.searchTime = duration.count() / 1000.0;
    
    return result;
}

bool PathFinder::dfsHelper(Maze& maze, const Point& current, const Point& target,
                          std::vector<Point>& path, std::vector<std::vector<bool>>& visited,
                          int& visitedCount) {
    // 标记当前点为已访问
    visited[current.x][current.y] = true;
    visitedCount++;
    path.push_back(current);
    
    // 如果到达目标点
    if (current == target) {
        return true;
    }
    
    // 获取所有可访问的邻居
    std::vector<Point> neighbors = maze.getAccessibleNeighbors(current);
    
    for (const auto& neighbor : neighbors) {
        if (!visited[neighbor.x][neighbor.y]) {
            if (dfsHelper(maze, neighbor, target, path, visited, visitedCount)) {
                return true;
            }
        }
    }
    
    // 回溯
    path.pop_back();
    return false;
}

PathFinder::SearchResult PathFinder::findPathBFS(Maze& maze) {
    auto start = std::chrono::high_resolution_clock::now();
    
    SearchResult result;
    result.algorithm = "广度优先搜索(BFS)";
    
    Point entrance = maze.getEntrance();
    Point exit = maze.getExit();
    
    // BFS队列
    std::queue<Point> queue;
    std::vector<std::vector<bool>> visited(maze.getRows(), 
                                         std::vector<bool>(maze.getCols(), false));
    std::unordered_map<Point, Point, PointHash> parent;
    
    queue.push(entrance);
    visited[entrance.x][entrance.y] = true;
    int visitedCount = 1;
    
    bool found = false;
    
    while (!queue.empty() && !found) {
        Point current = queue.front();
        queue.pop();
        
        if (current == exit) {
            found = true;
            break;
        }
        
        // 访问所有邻居
        std::vector<Point> neighbors = maze.getAccessibleNeighbors(current);
        for (const auto& neighbor : neighbors) {
            if (!visited[neighbor.x][neighbor.y]) {
                visited[neighbor.x][neighbor.y] = true;
                visitedCount++;
                parent[neighbor] = current;
                queue.push(neighbor);
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    result.found = found;
    result.visitedNodes = visitedCount;
    result.searchTime = duration.count() / 1000.0;
    
    if (found) {
        // 重构路径
        std::vector<Point> path;
        Point current = exit;
        
        while (!(current == entrance)) {
            path.push_back(current);
            current = parent[current];
        }
        path.push_back(entrance);
        
        std::reverse(path.begin(), path.end());
        result.path = path;
        result.steps = path.size() - 1;
    }
    
    return result;
}

PathFinder::SearchResult PathFinder::findPathAStar(Maze& maze) {
    auto start = std::chrono::high_resolution_clock::now();
    
    SearchResult result;
    result.algorithm = "A*算法";
    
    Point entrance = maze.getEntrance();
    Point exit = maze.getExit();
    
    // A*算法的开放列表和关闭列表
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openList;
    std::unordered_map<Point, int, PointHash> closedList;
    std::unordered_map<Point, AStarNode*, PointHash> nodeMap;
    
    // 创建起始节点
    AStarNode* startNode = new AStarNode(entrance, 0, manhattanDistance(entrance, exit));
    openList.push(*startNode);
    nodeMap[entrance] = startNode;
    
    int visitedCount = 0;
    bool found = false;
    AStarNode* endNode = nullptr;
    
    while (!openList.empty() && !found) {
        AStarNode current = openList.top();
        openList.pop();
        
        // 如果该节点已经在关闭列表中，跳过
        if (closedList.find(current.point) != closedList.end()) {
            continue;
        }
        
        // 将当前节点加入关闭列表
        closedList[current.point] = current.fCost;
        visitedCount++;
        
        // 如果到达目标
        if (current.point == exit) {
            found = true;
            endNode = nodeMap[current.point];
            break;
        }
        
        // 检查所有邻居
        std::vector<Point> neighbors = maze.getAccessibleNeighbors(current.point);
        for (const auto& neighbor : neighbors) {
            // 如果邻居在关闭列表中，跳过
            if (closedList.find(neighbor) != closedList.end()) {
                continue;
            }
            
            int newGCost = current.gCost + 1;
            int hCost = manhattanDistance(neighbor, exit);
            
            // 如果邻居不在开放列表中，或者找到了更短的路径
            auto it = nodeMap.find(neighbor);
            if (it == nodeMap.end() || newGCost < it->second->gCost) {
                AStarNode* neighborNode = new AStarNode(neighbor, newGCost, hCost, nodeMap[current.point]);
                nodeMap[neighbor] = neighborNode;
                openList.push(*neighborNode);
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    result.found = found;
    result.visitedNodes = visitedCount;
    result.searchTime = duration.count() / 1000.0;
    
    if (found && endNode) {
        result.path = reconstructPath(endNode);
        result.steps = result.path.size() - 1;
    }
    
    // 清理内存
    for (auto& pair : nodeMap) {
        delete pair.second;
    }
    
    return result;
}

int PathFinder::manhattanDistance(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Point> PathFinder::reconstructPath(AStarNode* endNode) {
    std::vector<Point> path;
    AStarNode* current = endNode;
    
    while (current != nullptr) {
        path.push_back(current->point);
        current = current->parent;
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::vector<Point>> PathFinder::findAllPaths(Maze& maze, int maxPaths) {
    std::vector<std::vector<Point>> allPaths;
    std::vector<Point> currentPath;
    std::vector<std::vector<bool>> visited(maze.getRows(), 
                                         std::vector<bool>(maze.getCols(), false));
    
    Point entrance = maze.getEntrance();
    Point exit = maze.getExit();
    
    findAllPathsDFS(maze, entrance, exit, currentPath, allPaths, visited, maxPaths);
    
    return allPaths;
}

void PathFinder::findAllPathsDFS(Maze& maze, const Point& current, const Point& target,
                                std::vector<Point>& currentPath, 
                                std::vector<std::vector<Point>>& allPaths,
                                std::vector<std::vector<bool>>& visited,
                                int maxPaths) {
    if (allPaths.size() >= maxPaths) {
        return;
    }
    
    visited[current.x][current.y] = true;
    currentPath.push_back(current);
    
    if (current == target) {
        allPaths.push_back(currentPath);
    } else {
        std::vector<Point> neighbors = maze.getAccessibleNeighbors(current);
        for (const auto& neighbor : neighbors) {
            if (!visited[neighbor.x][neighbor.y]) {
                findAllPathsDFS(maze, neighbor, target, currentPath, allPaths, visited, maxPaths);
            }
        }
    }
    
    // 回溯
    currentPath.pop_back();
    visited[current.x][current.y] = false;
}

void PathFinder::compareAlgorithms(Maze& maze) {
    std::cout << "\n=== 算法性能比较 ===" << std::endl;
    std::cout << std::setw(15) << "算法" 
              << std::setw(10) << "找到路径" 
              << std::setw(10) << "路径长度" 
              << std::setw(12) << "访问节点数" 
              << std::setw(12) << "搜索时间(ms)" << std::endl;
    std::cout << std::string(59, '-') << std::endl;
    
    // 测试DFS
    Maze mazeCopy1 = maze.clone();
    SearchResult dfsResult = findPathDFS(mazeCopy1);
    std::cout << std::setw(15) << dfsResult.algorithm
              << std::setw(10) << (dfsResult.found ? "是" : "否")
              << std::setw(10) << dfsResult.steps
              << std::setw(12) << dfsResult.visitedNodes
              << std::setw(12) << std::fixed << std::setprecision(3) << dfsResult.searchTime
              << std::endl;
    
    // 测试BFS
    Maze mazeCopy2 = maze.clone();
    SearchResult bfsResult = findPathBFS(mazeCopy2);
    std::cout << std::setw(15) << bfsResult.algorithm
              << std::setw(10) << (bfsResult.found ? "是" : "否")
              << std::setw(10) << bfsResult.steps
              << std::setw(12) << bfsResult.visitedNodes
              << std::setw(12) << std::fixed << std::setprecision(3) << bfsResult.searchTime
              << std::endl;
    
    // 测试A*
    Maze mazeCopy3 = maze.clone();
    SearchResult astarResult = findPathAStar(mazeCopy3);
    std::cout << std::setw(15) << astarResult.algorithm
              << std::setw(10) << (astarResult.found ? "是" : "否")
              << std::setw(10) << astarResult.steps
              << std::setw(12) << astarResult.visitedNodes
              << std::setw(12) << std::fixed << std::setprecision(3) << astarResult.searchTime
              << std::endl;
    
    std::cout << std::endl;
}

void PathFinder::printSearchResult(const SearchResult& result) {
    std::cout << "\n=== " << result.algorithm << " 搜索结果 ===" << std::endl;
    std::cout << "找到路径: " << (result.found ? "是" : "否") << std::endl;
    
    if (result.found) {
        std::cout << "路径长度: " << result.steps << " 步" << std::endl;
        std::cout << "访问节点数: " << result.visitedNodes << std::endl;
        std::cout << "搜索时间: " << std::fixed << std::setprecision(3) 
                  << result.searchTime << " 毫秒" << std::endl;
        
        std::cout << "路径坐标: ";
        for (size_t i = 0; i < result.path.size(); i++) {
            if (i > 0) std::cout << " -> ";
            std::cout << "(" << result.path[i].x << "," << result.path[i].y << ")";
        }
        std::cout << std::endl;
    } else {
        std::cout << "访问节点数: " << result.visitedNodes << std::endl;
        std::cout << "搜索时间: " << std::fixed << std::setprecision(3) 
                  << result.searchTime << " 毫秒" << std::endl;
    }
    std::cout << std::endl;
}
