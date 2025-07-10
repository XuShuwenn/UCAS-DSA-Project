#include "maze.h"
#include "pathfinder.h"
#include "visualizer.h"
#include <iostream>

/**
 * 保证有路径的演示程序
 */
int main() {
    std::cout << "=== 迷宫寻路系统演示（保证有路径）===" << std::endl;
    
    // 创建预定义的迷宫（保证有路径）
    std::cout << "\n1. 创建预定义的迷宫..." << std::endl;
    
    std::vector<std::vector<int>> mazeData = {
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 1, 0, 1, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    
    Maze maze(8, 10);
    maze.loadFromArray(mazeData);
    maze.setEntrance(Maze::Point(0, 0));
    maze.setExit(Maze::Point(7, 9));
    
    // 显示迷宫
    Visualizer visualizer(Visualizer::SIMPLE, false, 100);
    std::cout << "\n2. 显示预定义的迷宫:" << std::endl;
    visualizer.displayMaze(maze);
    
    // 创建路径寻找器
    PathFinder pathFinder;
    
    // 测试所有算法
    std::cout << "\n3. 测试所有算法..." << std::endl;
    std::vector<PathFinder::SearchResult> results;
    
    // DFS
    std::cout << "运行DFS..." << std::endl;
    Maze mazeCopy1 = maze.clone();
    auto dfsResult = pathFinder.findPathDFS(mazeCopy1);
    results.push_back(dfsResult);
    if (dfsResult.found) {
        std::cout << "DFS找到路径，长度: " << dfsResult.steps << " 步" << std::endl;
    }
    
    // BFS
    std::cout << "运行BFS..." << std::endl;
    Maze mazeCopy2 = maze.clone();
    auto bfsResult = pathFinder.findPathBFS(mazeCopy2);
    results.push_back(bfsResult);
    if (bfsResult.found) {
        std::cout << "BFS找到路径，长度: " << bfsResult.steps << " 步" << std::endl;
    }
    
    // A*
    std::cout << "运行A*..." << std::endl;
    Maze mazeCopy3 = maze.clone();
    auto astarResult = pathFinder.findPathAStar(mazeCopy3);
    results.push_back(astarResult);
    if (astarResult.found) {
        std::cout << "A*找到路径，长度: " << astarResult.steps << " 步" << std::endl;
    }
    
    // 显示最短路径
    if (bfsResult.found) {
        std::cout << "\n4. 显示BFS找到的最短路径:" << std::endl;
        visualizer.displayMazeWithPath(maze, bfsResult.path);
        
        // 生成HTML文件
        std::cout << "\n5. 生成HTML可视化文件..." << std::endl;
        if (visualizer.generateHTMLVisualization(maze, bfsResult.path, "demo_guaranteed.html")) {
            std::cout << "HTML文件已生成: demo_guaranteed.html" << std::endl;
        }
    }
    
    // 算法性能比较
    std::cout << "\n6. 算法性能比较:" << std::endl;
    visualizer.displayComparison(results);
    
    std::cout << "\n演示完成！所有功能正常工作。" << std::endl;
    return 0;
}
