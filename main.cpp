#include "maze.h"
#include "pathfinder.h"
#include "visualizer.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "CircularMaze.h"
#include "mondrian_maze.h"

/**
 * 主程序文件
 * 功能：
 * 1. 提供用户交互界面
 * 2. 整合迷宫生成、路径寻找和可视化功能
 * 3. 演示各种算法的使用
 * 4. 提供菜单驱动的操作界面
 */

class MazeApplication {
private:
    std::unique_ptr<Maze> maze;
    PathFinder pathFinder;
    Visualizer visualizer;
    
public:
    MazeApplication() : maze(nullptr), visualizer(Visualizer::COLORED, true, 50) {
    }
    
    void run() {
        Visualizer::printWelcome();
        
        int choice;
        do {
            printMainMenu();
            std::cin >> choice;
            
            switch (choice) {
                case 1:
                    createMaze();
                    break;
                case 2:
                    if (maze) {
                        solveMazeMenu();
                    } else {
                        std::cout << "请先创建迷宫！" << std::endl;
                    }
                    break;
                case 3:
                    if (maze) {
                        compareAllAlgorithms();
                    } else {
                        std::cout << "请先创建迷宫！" << std::endl;
                    }
                    break;
                case 4:
                    if (maze) {
                        visualizer.displayMaze(*maze);
                    } else {
                        std::cout << "请先创建迷宫！" << std::endl;
                    }
                    break;
                case 5:
                    if (maze) {
                        exportResults();
                    } else {
                        std::cout << "请先创建迷宫！" << std::endl;
                    }
                    break;
                case 6:
                    demonstrationMode();
                    break;
                case 7:
                    Visualizer::printHelp();
                    break;
                case 8:
                    mondrianMazeAdventure(visualizer);
                    break;
                case 0:
                    std::cout << "感谢使用迷宫寻路系统！" << std::endl;
                    break;
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
            
            if (choice != 0) {
                std::cout << "\n按回车键继续...";
                std::cin.ignore();
                std::cin.get();
            }
            
        } while (choice != 0);
    }

private:
    void printMainMenu() {
        std::cout << "\n" << std::string(40, '=') << std::endl;
        std::cout << "          迷宫寻路系统主菜单" << std::endl;
        std::cout << std::string(40, '=') << std::endl;
        std::cout << "1. 创建迷宫" << std::endl;
        std::cout << "2. 求解迷宫" << std::endl;
        std::cout << "3. 算法性能比较" << std::endl;
        std::cout << "4. 显示当前迷宫" << std::endl;
        std::cout << "5. 导出结果" << std::endl;
        std::cout << "6. 演示模式" << std::endl;
        std::cout << "7. 帮助信息" << std::endl;
        std::cout << "8. 闯入蒙德里安名画" << std::endl;
        std::cout << "0. 退出程序" << std::endl;
        std::cout << std::string(40, '=') << std::endl;
        std::cout << "请选择操作 (0-8): ";
    }
    
    void createMaze() {
        std::cout << "\n--- 创建迷宫 ---\n";
        std::cout << "1. 创建矩形迷宫\n";
        std::cout << "2. 创建圆形迷宫\n";
        std::cout << "请选择: ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            // 矩形迷宫
            int rows, cols;
            std::cout << "请输入迷宫行数: ";
            std::cin >> rows;
            std::cout << "请输入迷宫列数: ";
            std::cin >> cols;
            maze = std::make_unique<Maze>(rows, cols);

            std::cout << "1. 随机墙壁生成\n2. DFS生成 (保证连通)\n请选择生成方式: ";
            int gen_choice;
            std::cin >> gen_choice;
            if (gen_choice == 1) {
                double wallProb;
                std::cout << "请输入墙壁移除概率 (0.0-1.0): ";
                std::cin >> wallProb;
                maze->generateRandomMaze(wallProb);
            } else {
                maze->generateWithDFS();
            }
            std::cout << "矩形迷宫已生成!\n";

        } else if (choice == 2) {
            std::cout << "输入环数: ";
            int rings;
            std::cin >> rings;
            maze = std::make_unique<CircularMaze>(rings);
            maze->generate();
            std::cout << "圆形迷宫已生成!\n";
        }
        
        // 设置入口和出口
        setupEntranceAndExit();
        
        std::cout << "迷宫创建成功！" << std::endl;
        visualizer.displayMaze(*maze);
    }
    
    void setupEntranceAndExit() {
        std::cout << "\n=== 设置入口和出口 ===" << std::endl;
        std::cout << "1. 使用默认位置 (左上角和右下角)" << std::endl;
        std::cout << "2. 自定义位置" << std::endl;
        std::cout << "请选择 (1-2): ";
        
        int choice;
        std::cin >> choice;
        
        if (choice == 2) {
            int ex, ey, exitx, exity;
            std::cout << "请输入入口坐标 (行 列): ";
            std::cin >> ex >> ey;
            std::cout << "请输入出口坐标 (行 列): ";
            std::cin >> exitx >> exity;
            
            if (maze->isValidPosition(Point(ex, ey)) && maze->isValidPosition(Point(exitx, exity))) {
                maze->setEntrance({ex, ey});
                maze->setExit({exitx, exity});
            } else {
                std::cout << "坐标无效，使用默认位置。" << std::endl;
            }
        }
        // 默认情况下使用构造函数中设置的位置
    }
    
    void createSampleMaze(int rows, int cols) {
        // 创建一个简单的示例迷宫
        std::vector<std::vector<int>> sampleData;
        
        if (rows >= 5 && cols >= 5) {
            // 创建一个有趣的迷宫模式
            sampleData.resize(rows, std::vector<int>(cols, 0));
            
            // 添加一些墙壁
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (i == 0 || i == rows-1 || j == 0 || j == cols-1) {
                        // 边界墙壁，但保留入口和出口
                        if (!((i == 0 && j == 0) || (i == rows-1 && j == cols-1))) {
                            if ((i + j) % 3 != 0) {
                                sampleData[i][j] = 1; // 墙壁
                            }
                        }
                    } else if ((i + j) % 4 == 0) {
                        sampleData[i][j] = 1; // 内部墙壁
                    }
                }
            }
            
            // 对于线段墙壁的迷宫，需要用不同的方法
            // 创建简单的随机迷宫
            maze->generateRandomMaze(0.4);
        } else {
            // 对于小迷宫，使用随机生成
            maze->generateRandomMaze(0.25);
        }
    }
    
    void solveMazeMenu() {
        std::cout << "\n=== 选择求解算法 ===" << std::endl;
        std::cout << "1. 深度优先搜索 (DFS)" << std::endl;
        std::cout << "2. 广度优先搜索 (BFS) - 最短路径" << std::endl;
        std::cout << "3. A*算法 - 启发式搜索" << std::endl;
        std::cout << "4. 找到所有路径" << std::endl;
        std::cout << "请选择算法 (1-4): ";
        
        int choice;
        std::cin >> choice;
        
        // 创建迷宫副本以保持原始状态
        Maze mazeCopy = maze->clone();
        PathFinder::SearchResult result;
        
        switch (choice) {
            case 1:
                result = pathFinder.findPathDFS(mazeCopy);
                break;
            case 2:
                result = pathFinder.findPathBFS(mazeCopy);
                break;
            case 3:
                result = pathFinder.findPathAStar(mazeCopy);
                break;
            case 4:
                findAllPathsDemo();
                return;
            default:
                std::cout << "无效选择！" << std::endl;
                return;
        }
        
        // 显示结果
        visualizer.displaySearchResults(result);
        
        if (result.found) {
            std::cout << "是否显示路径可视化？(y/n): ";
            char showPath;
            std::cin >> showPath;
            
            if (showPath == 'y' || showPath == 'Y') {
                visualizer.displayMazeWithPath(*maze, result.path);
            }
            
            std::cout << "是否启用动画演示？(y/n): ";
            char animate;
            std::cin >> animate;
            
            if (animate == 'y' || animate == 'Y') {
                // 注意：这里需要重新搜索以获取搜索顺序
                // 在实际实现中，可以修改PathFinder来返回搜索顺序
                visualizer.setMode(Visualizer::ANIMATED);
                std::cout << "动画演示功能需要算法返回搜索顺序，当前版本暂不支持。" << std::endl;
                visualizer.setMode(Visualizer::COLORED);
            }
        }
    }
    
    void findAllPathsDemo() {
        std::cout << "\n正在寻找所有可能路径..." << std::endl;
        
        auto allPaths = pathFinder.findAllPaths(*maze, 10); // 限制最多10条路径
        
        std::cout << "找到 " << allPaths.size() << " 条路径：" << std::endl;
        
        for (size_t i = 0; i < allPaths.size(); i++) {
            std::cout << "\n路径 " << (i + 1) << " (长度: " << (allPaths[i].size() - 1) << " 步):" << std::endl;
            
            // 显示前5个坐标
            for (size_t j = 0; j < std::min(size_t(5), allPaths[i].size()); j++) {
                if (j > 0) std::cout << " -> ";
                std::cout << "(" << allPaths[i][j].x << "," << allPaths[i][j].y << ")";
            }
            if (allPaths[i].size() > 5) {
                std::cout << " -> ... -> (" << allPaths[i].back().x << "," << allPaths[i].back().y << ")";
            }
            std::cout << std::endl;
        }
        
        if (!allPaths.empty()) {
            std::cout << "\n是否显示最短路径？(y/n): ";
            char showShortest;
            std::cin >> showShortest;
            
            if (showShortest == 'y' || showShortest == 'Y') {
                auto shortest = std::min_element(allPaths.begin(), allPaths.end(),
                    [](const std::vector<Point>& a, const std::vector<Point>& b) {
                        return a.size() < b.size();
                    });
                
                visualizer.displayMazeWithPath(*maze, *shortest);
            }
        }
    }
    
    void compareAllAlgorithms() {
        std::cout << "\n=== 算法性能比较 ===" << std::endl;
        std::cout << "正在运行所有算法..." << std::endl;
        
        std::vector<PathFinder::SearchResult> results;
        
        // 测试DFS
        Maze mazeCopy1 = maze->clone();
        results.push_back(pathFinder.findPathDFS(mazeCopy1));
        
        // 测试BFS
        Maze mazeCopy2 = maze->clone();
        results.push_back(pathFinder.findPathBFS(mazeCopy2));
        
        // 测试A*
        Maze mazeCopy3 = maze->clone();
        results.push_back(pathFinder.findPathAStar(mazeCopy3));
        
        // 显示比较结果
        visualizer.displayComparison(results);
        
        // 显示最优路径
        auto bestResult = std::min_element(results.begin(), results.end(),
            [](const PathFinder::SearchResult& a, const PathFinder::SearchResult& b) {
                if (!a.found && !b.found) return false;
                if (!a.found) return false;
                if (!b.found) return true;
                return a.steps < b.steps;
            });
        
        if (bestResult != results.end() && bestResult->found) {
            std::cout << "是否显示最优路径？(y/n): ";
            char showBest;
            std::cin >> showBest;
            
            if (showBest == 'y' || showBest == 'Y') {
                visualizer.displayMazeWithPath(*maze, bestResult->path);
            }
        }
    }
    
    void exportResults() {
        std::cout << "\n=== 导出结果 ===" << std::endl;
        std::cout << "1. 导出迷宫到文本文件" << std::endl;
        std::cout << "2. 生成HTML可视化文件" << std::endl;
        std::cout << "3. 导出最短路径" << std::endl;
        std::cout << "请选择 (1-3): ";
        
        int choice;
        std::cin >> choice;
        
        std::string filename;
        std::cout << "请输入文件名（不含扩展名）: ";
        std::cin >> filename;
        
        switch (choice) {
            case 1:
                visualizer.exportToText(*maze, {}, filename + ".txt");
                std::cout << "迷宫已导出到 " << filename << ".txt" << std::endl;
                break;
            case 2: {
                // 使用BFS找到最短路径
                Maze mazeCopy = maze->clone();
                auto result = pathFinder.findPathBFS(mazeCopy);
                
                if (dynamic_cast<const CircularMaze*>(maze.get())) {
                    auto* circular_maze = dynamic_cast<const CircularMaze*>(maze.get());
                    visualizer.exportCircularToHTML(*circular_maze, result.path, filename + ".html");
                } else {
                    visualizer.exportToHTML(*maze, result.path, filename + ".html");
                }
                std::cout << "HTML文件已生成：" << filename << ".html" << std::endl;
                break;
            }
            case 3: {
                // 导出最短路径
                Maze mazeCopy = maze->clone();
                auto result = pathFinder.findPathBFS(mazeCopy);
                
                if (result.found) {
                    visualizer.exportToText(*maze, result.path, filename + "_path.txt");
                    std::cout << "路径已导出到 " << filename << "_path.txt" << std::endl;
                }
                break;
            }
            default:
                std::cout << "无效选择！" << std::endl;
                break;
        }
    }
    
    void demonstrationMode() {
        std::cout << "\n=== 演示模式 ===" << std::endl;
        std::cout << "将创建一个演示迷宫并展示所有算法..." << std::endl;
        
        // 创建演示迷宫
        maze = std::make_unique<Maze>(10, 15);
        maze->generateRandomMaze(0.35);
        
        std::cout << "\n演示迷宫：" << std::endl;
        visualizer.displayMaze(*maze);
        
        std::cout << "\n开始算法演示..." << std::endl;
        compareAllAlgorithms();
        
        // 创建HTML可视化
        Maze mazeCopy = maze->clone();
        auto result = pathFinder.findPathBFS(mazeCopy);
        if (result.found) {
            if (dynamic_cast<const CircularMaze*>(maze.get())) {
                auto* circular_maze = dynamic_cast<const CircularMaze*>(maze.get());
                visualizer.exportCircularToHTML(*circular_maze, result.path, "demo_maze.html");
            } else {
                visualizer.exportToHTML(*maze, result.path, "demo_maze.html");
            }
            std::cout << "\n演示结果已保存到 demo_maze.html" << std::endl;
        }
    }

    void mondrianMazeAdventure(const Visualizer& visualizer) {
        MondrianMaze mondrian;
        std::vector<std::vector<int>> allPaths;
        findAllPathsLimited(mondrian, mondrian.getEntranceId(), mondrian.getExitId(), 6, allPaths);
        if (allPaths.empty()) {
            std::cout << "未找到任何路径！\n";
            return;
        }
        // 输出所有路径
        int minLen = 1e9, minIdx = 0;
        std::cout << "共找到 " << allPaths.size() << " 条路径：\n";
        for (size_t i = 0; i < allPaths.size(); ++i) {
            std::cout << "路径 " << (i+1) << " (长度: " << allPaths[i].size() << "): ";
            for (int rid : allPaths[i]) std::cout << rid << " ";
            std::cout << std::endl;
            if ((int)allPaths[i].size() < minLen) { minLen = allPaths[i].size(); minIdx = i; }
        }
        std::cout << "最短路径为第 " << (minIdx+1) << " 条，长度: " << minLen << std::endl;
        std::string filename = "mondrian_multi_paths.html";
        visualizer.exportMondrianMultiPathsToHTML(mondrian, allPaths, minIdx, filename);
        std::cout << "已生成多路径HTML文件: " << filename << "，请用浏览器打开体验！\n";
    }
};

int main() {
    try {
        MazeApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
