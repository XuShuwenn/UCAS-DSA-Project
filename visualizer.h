#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "maze.h"
#include "pathfinder.h"
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include "mondrian_maze.h"

class CircularMaze; // 前向声明

/**
 * 可视化器类 - 用于显示线段墙壁迷宫和路径
 * 功能：
 * 1. ASCII字符界面显示迷宫（使用线段表示墙壁）
 * 2. 动画显示路径寻找过程
 * 3. 彩色终端输出（如果支持）
 * 4. 导出HTML和文本文件
 * 5. 统计信息显示
 */
class Visualizer {
public:
    // 显示模式
    enum DisplayMode {
        SIMPLE,      // 简单字符显示
        COLORED,     // 彩色显示（需要终端支持）
        ANIMATED     // 动画显示
    };
    
    // 颜色代码（ANSI）
    struct Colors {
        static const std::string RESET;
        static const std::string BLACK;
        static const std::string RED;
        static const std::string GREEN;
        static const std::string YELLOW;
        static const std::string BLUE;
        static const std::string MAGENTA;
        static const std::string CYAN;
        static const std::string WHITE;
        static const std::string BG_BLACK;
        static const std::string BG_RED;
        static const std::string BG_GREEN;
        static const std::string BG_YELLOW;
        static const std::string BG_BLUE;
        static const std::string BG_MAGENTA;
        static const std::string BG_CYAN;
        static const std::string BG_WHITE;
    };

private:
    DisplayMode mode;
    bool useColors;
    int animationDelay;  // 动画延迟（毫秒）

public:
    // 构造函数
    Visualizer(DisplayMode mode = SIMPLE, bool useColors = false, int animationDelay = 100);
    
    // 设置显示参数
    void setMode(DisplayMode newMode);
    void setUseColors(bool useColorOutput);
    void setAnimationDelay(int delay);
    
    // 显示迷宫
    void displayMaze(const Maze& maze) const;
    void displayMazeWithPath(const Maze& maze, const std::vector<Point>& path) const;
    
    // 动画显示路径寻找过程
    void animatePathFinding(Maze& maze, const std::vector<Point>& searchOrder, 
                           const std::vector<Point>& finalPath) const;
    
    // 导出功能
    void exportToHTML(const Maze& maze, const std::vector<Point>& path, 
                     const std::string& filename) const;
    void exportCircularToHTML(const CircularMaze& maze, const std::vector<Point>& path,
                              const std::string& filename) const; // 新增函数
    void exportToText(const Maze& maze, const std::vector<Point>& path, 
                     const std::string& filename) const;
    void exportMondrianToHTML(const MondrianMaze& maze, const std::vector<int>& path, const std::string& filename) const;
    
    // 统计信息显示
    void printStatistics(const Maze& maze) const;
    
    // 显示搜索结果
    void displaySearchResults(const PathFinder::SearchResult& result) const;
    void displayComparison(const std::vector<PathFinder::SearchResult>& results) const;
    
    // 实用函数
    static void printWelcome();
    static void printHelp();

private:
    // 迷宫显示相关
    void displayMazeWithLineWalls(const Maze& maze) const;
    std::string getCellSymbol(const MazeCell& cell) const;
    std::string getConnector(bool hasLeft, bool hasRight, bool hasTop, bool hasBottom) const; // Changed return type to std::string
    
    // HTML生成
    std::string generateHTMLHeader() const;
    std::string generateMazeHTML(const Maze& maze, const std::vector<Point>& path) const;
    std::string generateHTMLFooter() const;
    
    // 颜色和样式
    std::string getColorCode(const std::string& color) const;
    bool isColorSupported() const;
    
    // 屏幕控制
    void clearScreen() const;
    void drawBorder(int width, int style = 0) const;
};

#endif // VISUALIZER_H
