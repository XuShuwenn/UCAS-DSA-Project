#include "maze.h"
#include "pathfinder.h"
#include "visualizer.h"
#include <iostream>

/**
 * 简单的演示程序
 * 展示迷宫寻路系统的基本功能
 */
int main() {
    std::cout << "=== 迷宫寻路系统演示 ===" << std::endl;
    
    // 创建一个小型迷宫
    std::cout << "\n1. 创建8x12的迷宫..." << std::endl;
    Maze maze(8, 12);
    maze.generateRandomMaze(0.3);
    
    // 显示迷宫
    Visualizer visualizer(Visualizer::SIMPLE, false, 100);
    std::cout << "\n2. 显示生成的迷宫:" << std::endl;
    visualizer.displayMaze(maze);
    
    // 创建路径寻找器
    PathFinder pathFinder;
    
    // 测试BFS算法
    std::cout << "\n3. 使用BFS寻找最短路径..." << std::endl;
    Maze mazeCopy = maze.clone();
    auto bfsResult = pathFinder.findPathBFS(mazeCopy);
    
    if (bfsResult.found) {
        std::cout << "找到路径！长度: " << bfsResult.steps << " 步" << std::endl;
        visualizer.displayMazeWithPath(maze, bfsResult.path);
        
        // 保存HTML文件
        std::cout << "\n4. 生成HTML可视化文件..." << std::endl;
        if (visualizer.generateHTMLVisualization(maze, bfsResult.path, "demo.html")) {
            std::cout << "HTML文件已生成: demo.html" << std::endl;
        }
    } else {
        std::cout << "未找到路径！" << std::endl;
    }
    
    // 算法性能比较
    std::cout << "\n5. 算法性能比较:" << std::endl;
    std::vector<PathFinder::SearchResult> results;
    
    // DFS
    Maze mazeCopy1 = maze.clone();
    results.push_back(pathFinder.findPathDFS(mazeCopy1));
    
    // BFS
    Maze mazeCopy2 = maze.clone();
    results.push_back(pathFinder.findPathBFS(mazeCopy2));
    
    // A*
    Maze mazeCopy3 = maze.clone();
    results.push_back(pathFinder.findPathAStar(mazeCopy3));
    
    visualizer.displayComparison(results);
    
    std::cout << "\n演示完成！" << std::endl;
    return 0;
}
