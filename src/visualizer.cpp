#include "visualizer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <set>
#include <thread>
#include <chrono>
#include <cstring>
#include "CircularMaze.h"
#include <fstream>
#include <cmath>
#include "mondrian_maze.h"
#include <unordered_set> // Added for unordered_set
#include <map>

/**
 * 可视化器的实现 - 支持线段墙壁显示
 * 提供多种迷宫显示方式和动画效果
 */

// 颜色代码定义
const std::string Visualizer::Colors::RESET = "\033[0m";
const std::string Visualizer::Colors::BLACK = "\033[30m";
const std::string Visualizer::Colors::RED = "\033[31m";
const std::string Visualizer::Colors::GREEN = "\033[32m";
const std::string Visualizer::Colors::YELLOW = "\033[33m";
const std::string Visualizer::Colors::BLUE = "\033[34m";
const std::string Visualizer::Colors::MAGENTA = "\033[35m";
const std::string Visualizer::Colors::CYAN = "\033[36m";
const std::string Visualizer::Colors::WHITE = "\033[37m";
const std::string Visualizer::Colors::BG_BLACK = "\033[40m";
const std::string Visualizer::Colors::BG_RED = "\033[41m";
const std::string Visualizer::Colors::BG_GREEN = "\033[42m";
const std::string Visualizer::Colors::BG_YELLOW = "\033[43m";
const std::string Visualizer::Colors::BG_BLUE = "\033[44m";
const std::string Visualizer::Colors::BG_MAGENTA = "\033[45m";
const std::string Visualizer::Colors::BG_CYAN = "\033[46m";
const std::string Visualizer::Colors::BG_WHITE = "\033[47m";

Visualizer::Visualizer(DisplayMode mode, bool useColors, int animationDelay)
    : mode(mode), useColors(useColors && isColorSupported()), animationDelay(animationDelay) {
}

void Visualizer::displayMaze(const Maze& maze) const {
    if (mode == ANIMATED) {
        clearScreen();
    }
    
    displayMazeWithLineWalls(maze);
}

void Visualizer::displayMazeWithLineWalls(const Maze& maze) const {
    std::cout << "\n" << getColorCode(Colors::CYAN) << "迷宫结构（线段表示墙壁）：" 
              << getColorCode(Colors::RESET) << "\n\n";
    
    int rows = maze.getRows();
    int cols = maze.getCols();
    
    // 显示列标号
    std::cout << "    ";
    for (int j = 0; j < cols; j++) {
        std::cout << std::setw(4) << j;
    }
    std::cout << "\n";
    
    // 显示顶部边界
    std::cout << "  ┌";
    for (int j = 0; j < cols; j++) {
        if (maze.hasWall(0, j, WallDirection::TOP)) {
            std::cout << "───";
        } else {
            std::cout << "   ";
        }
        if (j < cols - 1) {
            std::cout << "┬";
        }
    }
    std::cout << "┐\n";
    
    // 显示迷宫内容
    for (int i = 0; i < rows; i++) {
        // 显示行标号和左边界
        std::cout << std::setw(2) << i << (maze.hasWall(i, 0, WallDirection::LEFT) ? "│" : " ");
        
        for (int j = 0; j < cols; j++) {
            // 显示格子内容
            std::string cellContent = getCellSymbol(maze.getCell(i, j));
            std::cout << cellContent;
            
            // 显示右边界
            if (j < cols - 1) {
                if (maze.hasWall(i, j, WallDirection::RIGHT)) {
                    std::cout << "│";
                } else {
                    std::cout << " ";
                }
            }
        }
        
        // 显示最右边界
        std::cout << (maze.hasWall(i, cols-1, WallDirection::RIGHT) ? "│" : " ") << "\n";
        
        // 显示底部边界（除最后一行）
        if (i < rows - 1) {
            std::cout << "  ";
            
            // 左侧连接符
            bool hasLeftBottom = maze.hasWall(i, 0, WallDirection::BOTTOM);
            bool hasLeftTop = maze.hasWall(i+1, 0, WallDirection::TOP);
            bool hasLeftLeft = maze.hasWall(i, 0, WallDirection::LEFT);
            bool hasNextLeftLeft = maze.hasWall(i+1, 0, WallDirection::LEFT);
            
            if ((hasLeftBottom || hasLeftTop) && (hasLeftLeft || hasNextLeftLeft)) {
                std::cout << "├";
            } else if (hasLeftBottom || hasLeftTop) {
                std::cout << "─";
            } else if (hasLeftLeft || hasNextLeftLeft) {
                std::cout << "│";
            } else {
                std::cout << " ";
            }
            
            // 中间的水平线和连接符
            for (int j = 0; j < cols; j++) {
                // 水平线
                bool hasBottom = maze.hasWall(i, j, WallDirection::BOTTOM);
                bool hasTop = maze.hasWall(i+1, j, WallDirection::TOP);
                if (hasBottom || hasTop) {
                    std::cout << "───";
                } else {
                    std::cout << "   ";
                }
                
                // 连接符（除最后一列）
                if (j < cols - 1) {
                    bool hasCurrentBottom = hasBottom || hasTop;
                    bool hasRightBottom = maze.hasWall(i, j+1, WallDirection::BOTTOM) || 
                                         maze.hasWall(i+1, j+1, WallDirection::TOP);
                    bool hasCurrentRight = maze.hasWall(i, j, WallDirection::RIGHT);
                    bool hasNextRight = maze.hasWall(i+1, j, WallDirection::RIGHT);
                    
                    std::string connector = getConnector(hasCurrentBottom, hasRightBottom, 
                                                hasCurrentRight, hasNextRight);
                    std::cout << connector;
                }
            }
            
            // 右侧连接符
            bool hasRightBottom = maze.hasWall(i, cols-1, WallDirection::BOTTOM);
            bool hasRightTop = maze.hasWall(i+1, cols-1, WallDirection::TOP);
            bool hasRightRight = maze.hasWall(i, cols-1, WallDirection::RIGHT);
            bool hasNextRightRight = maze.hasWall(i+1, cols-1, WallDirection::RIGHT);
            
            if ((hasRightBottom || hasRightTop) && (hasRightRight || hasNextRightRight)) {
                std::cout << "┤";
            } else if (hasRightBottom || hasRightTop) {
                std::cout << "─";
            } else if (hasRightRight || hasNextRightRight) {
                std::cout << "│";
            } else {
                std::cout << " ";
            }
            
            std::cout << "\n";
        }
    }
    
    // 显示底部边界
    std::cout << "  └";
    for (int j = 0; j < cols; j++) {
        if (maze.hasWall(rows-1, j, WallDirection::BOTTOM)) {
            std::cout << "───";
        } else {
            std::cout << "   ";
        }
        if (j < cols - 1) {
            std::cout << "┴";
        }
    }
    std::cout << "┘\n\n";
}

std::string Visualizer::getConnector(bool hasLeft, bool hasRight, bool hasTop, bool hasBottom) const {
    int connections = hasLeft + hasRight + hasTop + hasBottom;
    
    if (connections == 4) return "┼";
    if (connections == 3) {
        if (!hasLeft) return "├";
        if (!hasRight) return "┤";
        if (!hasTop) return "┬";
        if (!hasBottom) return "┴";
    }
    if (connections == 2) {
        if (hasLeft && hasRight) return "─";
        if (hasTop && hasBottom) return "│";
        if (hasLeft && hasTop) return "┘";
        if (hasLeft && hasBottom) return "┐";
        if (hasRight && hasTop) return "└";
        if (hasRight && hasBottom) return "┌";
    }
    if (connections == 1) {
        if (hasLeft || hasRight) return "─";
        if (hasTop || hasBottom) return "│";
    }
    return " "; // Default case
}

std::string Visualizer::getCellSymbol(const MazeCell& cell) const {
    std::string symbol;
    std::string colorCode;
    
    switch (cell.type) {
        case CellType::ENTRANCE:
            symbol = " S ";
            colorCode = useColors ? Colors::BG_GREEN + Colors::BLACK : "";
            break;
        case CellType::EXIT:
            symbol = " E ";
            colorCode = useColors ? Colors::BG_RED + Colors::WHITE : "";
            break;
        case CellType::VISITED:
            symbol = " · ";
            colorCode = useColors ? Colors::BG_YELLOW + Colors::BLACK : "";
            break;
        default:  // PATH
            symbol = "   ";
            colorCode = "";
            break;
    }
    
    if (useColors && !colorCode.empty()) {
        return colorCode + symbol + Colors::RESET;
    }
    return symbol;
}

void Visualizer::displayMazeWithPath(const Maze& maze, const std::vector<Point>& path) const {
    Maze tempMaze = maze.clone();
    
    // 标记路径
    for (const Point& p : path) {
        if (p != maze.getEntrance() && p != maze.getExit()) {
            tempMaze.setCellType(p, CellType::VISITED);
        }
    }
    
    displayMazeWithLineWalls(tempMaze);
    
    // 显示路径信息
    std::cout << getColorCode(Colors::CYAN) << "路径信息：" << getColorCode(Colors::RESET) << "\n";
    std::cout << "路径长度: " << (path.empty() ? 0 : path.size() - 1) << " 步\n";
    std::cout << "路径坐标: ";
    for (size_t i = 0; i < path.size(); i++) {
        if (i > 0) std::cout << " → ";
        std::cout << "(" << path[i].x << "," << path[i].y << ")";
        if (i > 0 && i % 8 == 0) std::cout << "\n          ";
    }
    std::cout << "\n\n";
}

void Visualizer::animatePathFinding(Maze& maze, const std::vector<Point>& searchOrder,
                                  const std::vector<Point>& finalPath) const {
    if (mode != ANIMATED) {
        displayMazeWithPath(maze, finalPath);
        return;
    }
    
    std::cout << getColorCode(Colors::YELLOW) << "开始路径搜索动画..." << getColorCode(Colors::RESET) << "\n\n";
    
    // 重置迷宫
    maze.resetVisited();
    
    // 逐步显示搜索过程
    for (size_t i = 0; i < searchOrder.size(); i++) {
        const Point& current = searchOrder[i];
        
        if (current != maze.getEntrance() && current != maze.getExit()) {
            maze.setCellType(current, CellType::VISITED);
        }
        
        clearScreen();
        std::cout << getColorCode(Colors::CYAN) << "搜索进度: " << (i + 1) << "/" << searchOrder.size() 
                  << getColorCode(Colors::RESET) << "\n";
        displayMazeWithLineWalls(maze);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));
    }
    
    // 显示最终路径
    std::cout << getColorCode(Colors::GREEN) << "找到路径！显示最终结果..." << getColorCode(Colors::RESET) << "\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    clearScreen();
    displayMazeWithPath(maze, finalPath);
}

void Visualizer::exportToHTML(const Maze& maze, const std::vector<Point>& path, 
                            const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建HTML文件: " << filename << std::endl;
        return;
    }
    
    file << generateHTMLHeader();
    file << generateMazeHTML(maze, path);
    file << generateHTMLFooter();
    
    file.close();
    std::cout << getColorCode(Colors::GREEN) << "迷宫已导出到HTML文件: " << filename 
              << getColorCode(Colors::RESET) << std::endl;
}

std::string Visualizer::generateHTMLHeader() const {
    return R"(<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>迷宫可视化</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 20px;
            text-align: center; /* 新增：让所有内容居中 */
        }
        .maze-container {
            display: inline-block;
            background-color: white;
            border: 2px solid #333;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
            margin: 0 auto; /* 新增：居中容器 */
        }
        .maze-grid {
            display: grid;
            gap: 1px;
            background-color: #333;
            border: 2px solid #333;
        }
        .maze-cell {
            width: 30px;
            height: 30px;
            position: relative;
            background-color: white;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 12px;
            font-weight: bold;
        }
        .cell-wall-top { border-top: 3px solid #333; }
        .cell-wall-right { border-right: 3px solid #333; }
        .cell-wall-bottom { border-bottom: 3px solid #333; }
        .cell-wall-left { border-left: 3px solid #333; }
        .cell-entrance { background-color: #4CAF50; color: white; }
        .cell-exit { background-color: #F44336; color: white; }
        .cell-path { background-color: #FFEB3B; }
        .cell-visited { background-color: #FFC107; }
        .arrow {
            position: absolute;
            left: 50%;
            top: 50%;
            width: 24px;
            height: 24px;
            transform: translate(-50%, -50%);
            pointer-events: none;
        }
        .info-panel {
            margin-top: 20px;
            padding: 15px;
            background-color: #e8e8e8;
            border-radius: 5px;
        }
        h1 { color: #333; text-align: center; }
        h2 { color: #666; }
    </style>
</head>
<body>
    <h1>🏃 迷宫路径可视化 🏃</h1>
)";
}

std::string Visualizer::generateMazeHTML(const Maze& maze, const std::vector<Point>& path) const {
    std::ostringstream html;
    
    int rows = maze.getRows();
    int cols = maze.getCols();
    
    // 创建路径标记集合
    std::set<Point> pathSet;
    for (const Point& p : path) {
        pathSet.insert(p);
    }
    
    // 计算每个点的下一个点方向
    std::map<Point, Point> nextMap;
    for (size_t i = 1; i < path.size(); ++i) {
        nextMap[path[i-1]] = path[i];
    }
    
    html << "    <div class=\"maze-container\">\n";
    html << "        <div class=\"maze-grid\" style=\"grid-template-columns: repeat(" 
         << cols << ", 30px);\">\n";
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            const MazeCell& cell = maze.getCell(i, j);
            Point currentPoint(i, j);
            
            html << "            <div class=\"maze-cell";
            
            // 添加墙壁样式
            if (cell.hasWall(WallDirection::TOP)) html << " cell-wall-top";
            if (cell.hasWall(WallDirection::RIGHT)) html << " cell-wall-right";
            if (cell.hasWall(WallDirection::BOTTOM)) html << " cell-wall-bottom";
            if (cell.hasWall(WallDirection::LEFT)) html << " cell-wall-left";
            
            // 添加格子类型样式
            if (cell.type == CellType::ENTRANCE) {
                html << " cell-entrance\">S";
            } else if (cell.type == CellType::EXIT) {
                html << " cell-exit\">E";
            } else if (pathSet.count(currentPoint) > 0) {
                html << " cell-path\">";
                auto it = nextMap.find(currentPoint);
                if (it != nextMap.end()) {
                    int dx = it->second.x - currentPoint.x;
                    int dy = it->second.y - currentPoint.y;
                    std::string rotate;
                    if (dx == 1 && dy == 0) rotate = "90";
                    else if (dx == -1 && dy == 0) rotate = "-90";
                    else if (dx == 0 && dy == 1) rotate = "0";
                    else if (dx == 0 && dy == -1) rotate = "180";
                    else rotate = "0";
                    html << "<svg class=\"arrow\" viewBox=\"0 0 24 24\" style=\"transform: rotate(" << rotate << "deg);\"><polygon points=\"6,4 18,12 6,20 8,12\" fill=\"#1976D2\"/></svg>";
                } else {
                    html << "<span style=\"font-size:20px;color:#1976D2;\">•</span>";
                }
            } else {
                html << "\">&nbsp;";
            }
            
            html << "</div>\n";
        }
    }
    
    html << "        </div>\n";
    html << "        <div class=\"info-panel\">\n";
    html << "            <h2>迷宫信息</h2>\n";
    html << "            <p><strong>尺寸:</strong> " << rows << " × " << cols << "</p>\n";
    html << "            <p><strong>入口:</strong> (" << maze.getEntrance().x << ", " 
         << maze.getEntrance().y << ")</p>\n";
    html << "            <p><strong>出口:</strong> (" << maze.getExit().x << ", " 
         << maze.getExit().y << ")</p>\n";
    html << "            <p><strong>路径长度:</strong> " << (path.empty() ? 0 : path.size() - 1) << " 步</p>\n";
    html << "            <p><strong>总墙壁数:</strong> " << maze.countWalls() << "</p>\n";
    html << "        </div>\n";
    html << "    </div>\n";
    
    return html.str();
}

std::string Visualizer::generateHTMLFooter() const {
    return R"(</body>
</html>)";
}

void Visualizer::exportToText(const Maze& maze, const std::vector<Point>& path, 
                            const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建文本文件: " << filename << std::endl;
        return;
    }
    
    // 重定向cout到文件
    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());
    
    // 输出迷宫信息
    std::cout << "迷宫路径寻找结果\n";
    std::cout << "================\n\n";
    std::cout << "迷宫尺寸: " << maze.getRows() << " × " << maze.getCols() << "\n";
    std::cout << "入口位置: (" << maze.getEntrance().x << ", " << maze.getEntrance().y << ")\n";
    std::cout << "出口位置: (" << maze.getExit().x << ", " << maze.getExit().y << ")\n";
    std::cout << "路径长度: " << (path.empty() ? 0 : path.size() - 1) << " 步\n\n";
    
    // 输出迷宫结构
    displayMazeWithPath(maze, path);
    
    // 恢复cout
    std::cout.rdbuf(oldCoutBuffer);
    file.close();
    
    std::cout << getColorCode(Colors::GREEN) << "迷宫已导出到文本文件: " << filename 
              << getColorCode(Colors::RESET) << std::endl;
}

void Visualizer::printStatistics(const Maze& maze) const {
    std::cout << getColorCode(Colors::CYAN) << "\n=== 迷宫统计信息 ===" << getColorCode(Colors::RESET) << "\n";
    std::cout << "迷宫尺寸: " << maze.getRows() << " × " << maze.getCols() << "\n";
    std::cout << "总格子数: " << maze.getRows() * maze.getCols() << "\n";
    std::cout << "墙壁数量: " << maze.countWalls() << "\n";
    std::cout << "开放通道: " << maze.countOpenPaths() << "\n";
    std::cout << "连通性: " << std::fixed << std::setprecision(1) 
              << maze.getConnectivity() * 100 << "%\n";
    std::cout << "入口位置: (" << maze.getEntrance().x << ", " << maze.getEntrance().y << ")\n";
    std::cout << "出口位置: (" << maze.getExit().x << ", " << maze.getExit().y << ")\n\n";
}

void Visualizer::clearScreen() const {
    #ifdef _WIN32
        (void)system("cls");
    #else
        (void)system("clear");
    #endif
}

void Visualizer::drawBorder(int width, int style) const {
    std::string borderChar = (style == 1) ? "═" : "─";
    std::string corner = (style == 1) ? "╔" : "┌";
    std::string endCorner = (style == 1) ? "╗" : "┐";
    
    std::cout << corner;
    for (int i = 0; i < width - 2; i++) {
        std::cout << borderChar;
    }
    std::cout << endCorner << std::endl;
}

bool Visualizer::isColorSupported() const {
    // 简单检查是否支持颜色输出
    const char* term = getenv("TERM");
    return term != nullptr && strstr(term, "color") != nullptr;
}

std::string Visualizer::getColorCode(const std::string& color) const {
    return useColors ? color : "";
}

void Visualizer::setMode(DisplayMode newMode) {
    mode = newMode;
}

void Visualizer::setUseColors(bool useColorOutput) {
    useColors = useColorOutput && isColorSupported();
}

void Visualizer::setAnimationDelay(int delay) {
    animationDelay = std::max(0, delay);
}

void Visualizer::displaySearchResults(const PathFinder::SearchResult& result) const {
    std::cout << "\n" << getColorCode(Colors::CYAN) << std::string(50, '=') << getColorCode(Colors::RESET) << std::endl;
    std::cout << "   " << result.algorithm << " 搜索结果" << std::endl;
    std::cout << getColorCode(Colors::CYAN) << std::string(50, '=') << getColorCode(Colors::RESET) << std::endl;
    
    if (result.found) {
        std::cout << getColorCode(Colors::GREEN) << "状态: 找到路径" << getColorCode(Colors::RESET) << std::endl;
        std::cout << "路径长度: " << result.steps << " 步" << std::endl;
    } else {
        std::cout << getColorCode(Colors::RED) << "状态: 未找到路径" << getColorCode(Colors::RESET) << std::endl;
    }
    
    std::cout << "访问节点数: " << result.visitedNodes << std::endl;
    std::cout << "搜索时间: " << std::fixed << std::setprecision(3) << result.searchTime << " 毫秒" << std::endl;
    
    if (result.found && !result.path.empty()) {
        std::cout << "\n路径详情:" << std::endl;
        for (size_t i = 0; i < result.path.size(); i++) {
            if (i % 5 == 0) std::cout << "\n  ";
            std::cout << "(" << result.path[i].x << "," << result.path[i].y << ")";
            if (i < result.path.size() - 1) std::cout << " → ";
        }
        std::cout << std::endl;
    }
    
    std::cout << getColorCode(Colors::CYAN) << std::string(50, '=') << getColorCode(Colors::RESET) << std::endl;
}

void Visualizer::displayComparison(const std::vector<PathFinder::SearchResult>& results) const {
    if (results.empty()) return;
    
    std::cout << "\n" << getColorCode(Colors::CYAN) << std::string(70, '=') << getColorCode(Colors::RESET) << std::endl;
    std::cout << "                        算法性能比较" << std::endl;
    std::cout << getColorCode(Colors::CYAN) << std::string(70, '=') << getColorCode(Colors::RESET) << std::endl;
    
    // 表头
    std::cout << std::setw(18) << "算法" 
              << std::setw(8) << "状态" 
              << std::setw(10) << "路径长度" 
              << std::setw(12) << "访问节点数" 
              << std::setw(12) << "时间(ms)" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    // 数据行
    for (const auto& result : results) {
        std::cout << std::setw(18) << result.algorithm
                  << std::setw(8) << (result.found ? "成功" : "失败")
                  << std::setw(10) << (result.found ? std::to_string(result.steps) : "N/A")
                  << std::setw(12) << result.visitedNodes
                  << std::setw(12) << std::fixed << std::setprecision(3) << result.searchTime
                  << std::endl;
    }
    
    std::cout << getColorCode(Colors::CYAN) << std::string(70, '=') << getColorCode(Colors::RESET) << std::endl;
}

void Visualizer::printWelcome() {
    std::cout << "\n";
    std::cout << "████████████████████████████████████████████████\n";
    std::cout << "█                                              █\n";
    std::cout << "█           🏃 C++ 迷宫寻路系统 🏃             █\n";
    std::cout << "█                                              █\n";
    std::cout << "█  功能特性：                                  █\n";
    std::cout << "█  • 线段表示迷宫墙壁                          █\n";
    std::cout << "█  • 随机迷宫生成                              █\n";
    std::cout << "█  • 多种路径寻找算法 (DFS/BFS/A*)             █\n";
    std::cout << "█  • 算法性能比较                              █\n";
    std::cout << "█  • 精美可视化展示                            █\n";
    std::cout << "█  • HTML导出功能                              █\n";
    std::cout << "█                                              █\n";
    std::cout << "████████████████████████████████████████████████\n";
    std::cout << std::endl;
}

void Visualizer::printHelp() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "                    操作指南" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "1. 生成迷宫 - 创建使用线段表示墙壁的迷宫" << std::endl;
    std::cout << "2. 选择算法 - DFS(深度优先)/BFS(广度优先)/A*" << std::endl;
    std::cout << "3. 寻找路径 - 自动寻找从入口到出口的路径" << std::endl;
    std::cout << "4. 查看结果 - 显示路径和性能统计" << std::endl;
    std::cout << "5. 比较算法 - 同时运行多种算法进行比较" << std::endl;
    std::cout << "6. 导出结果 - 保存为文本或HTML文件" << std::endl;
    std::cout << "\n符号说明:" << std::endl;
    std::cout << "┌─┐│  - 墙壁线段    S - 起点    E - 终点" << std::endl;
    std::cout << "     - 通路        · - 搜索过的路径" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

// Helper to convert polar to cartesian coordinates
Point polarToCartesian(double radius, double angle_rad) {
    return {(int)(radius * cos(angle_rad)), (int)(radius * sin(angle_rad))};
}

void Visualizer::exportCircularToHTML(const CircularMaze& maze, const std::vector<Point>& path,
                                      const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建HTML文件: " << filename << std::endl;
        return;
    }

    file << "<!DOCTYPE html><html><head><title>Circular Maze</title><style>body{text-align:center;}</style></head><body>";
    file << "<h1>Circular Maze Visualization</h1>";
    file << "<svg width=\"800\" height=\"800\" viewBox=\"-450 -450 900 900\" xmlns=\"http://www.w3.org/2000/svg\" style=\"background-color:#f0f0f0;\">";
    
    int rings = maze.getRings();
    double ring_radius_step = 40.0;
    
    // 1. Draw Walls
    for (int r = 0; r < rings; ++r) {
        int cells = maze.getCellsInRing(r);
        double angle_rad_step = 2 * M_PI / cells;
        double r_inner = (r + 1) * ring_radius_step;
        double r_outer = (r + 2) * ring_radius_step;

        for (int i = 0; i < cells; ++i) {
            double angle_start = i * angle_rad_step;
            
            // Draw radial walls (vertical_walls)
            if (maze.vertical_walls[r][i]) {
                Point p1 = polarToCartesian(r_inner, angle_start);
                Point p2 = polarToCartesian(r_outer, angle_start);
                file << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"black\" stroke-width=\"2\" />\n";
            }
            // Draw concentric walls (horizontal_walls)
            if (r < rings -1 && maze.horizontal_walls[r][i]) {
                 Point p_start = polarToCartesian(r_outer, angle_start);
                 Point p_end = polarToCartesian(r_outer, angle_start + angle_rad_step);
                 file << "<path d=\"M " << p_start.x << " " << p_start.y << " A " << r_outer << " " << r_outer << " 0 0 1 " << p_end.x << " " << p_end.y << "\" fill=\"none\" stroke=\"black\" stroke-width=\"2\" />\n";
            }
        }
    }
    // Draw the outermost wall
    double r_outermost = (rings + 1) * ring_radius_step;
    file << "<circle cx=\"0\" cy=\"0\" r=\"" << r_outermost << "\" fill=\"none\" stroke=\"black\" stroke-width=\"2\" />\n";
    
    // 2. Draw Path
    if (!path.empty()) {
        std::stringstream path_data;
        for (size_t i = 0; i < path.size(); ++i) {
            int r = path[i].x;
            int c = path[i].y;
            int cells = maze.getCellsInRing(r);
            double angle_rad_step = 2 * M_PI / cells;
            double radius = (r + 1.5) * ring_radius_step;
            double angle = (c + 0.5) * angle_rad_step;
            Point p = polarToCartesian(radius, angle);
            
            if (i == 0) path_data << "M";
            else path_data << "L";
            path_data << " " << p.x << " " << p.y << " ";
        }
        file << "<path d=\"" << path_data.str() << "\" fill=\"none\" stroke=\"#FF6347\" stroke-width=\"4\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n";
    }

    file << "</svg></body></html>";
    file.close();
    std::cout << "圆形迷宫已导出到HTML文件: " << filename << std::endl;
}

void Visualizer::exportMondrianToHTML(const MondrianMaze& maze, const std::vector<int>& path, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建HTML文件: " << filename << std::endl;
        return;
    }
    file << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Mondrian Maze</title>"
            "<style>body{display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh;margin:0;}"
            ".maze-center{display:flex;flex-direction:column;align-items:center;justify-content:center;width:100vw;height:100vh;}"
            "svg{display:block;margin:auto;}h2{text-align:center;}</style>"
            "</head><body><div class=\"maze-center\">\n";
    file << "<h2>闯入蒙德里安名画 - Mondrian Maze</h2>\n";
    file << "<svg width=\"400\" height=\"400\" style=\"background:#fff;box-shadow:0 0 8px #aaa;\">\n";
    
    // 创建一个路径房间ID的集合，方便快速查找
    std::unordered_set<int> path_ids(path.begin(), path.end());

    // 画所有房间
    for (const auto& room : maze.getRooms()) {
        bool inPath = path_ids.count(room.id);
        std::string fillColor = inPath ? "#9370DB" : room.color; // 高亮色：中紫色
        std::string border = "stroke:#222;stroke-width:3;";
        file << "<rect x=\"" << room.x << "\" y=\"" << room.y << "\" width=\"" << room.width << "\" height=\"" << room.height << "\" fill=\"" << fillColor << "\" style=\"" << border << "\"/>\n";
    }

    // 路径高亮连线
    if (path.size() >= 2) {
        std::cout << "Mondrian path: ";
        for (int rid : path) std::cout << rid << " ";
        std::cout << std::endl;
        file << "<polyline points=\"";
        for (int rid : path) {
            const auto& r = maze.getRoom(rid);
            file << (r.x + r.width/2) << "," << (r.y + r.height/2) << " ";
        }
        file << "\" fill=\"none\" stroke=\"#43aa8b\" stroke-width=\"4\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n";
    }
    // 标记入口和出口
    const auto& ent = maze.getRoom(maze.getEntranceId());
    const auto& ext = maze.getRoom(maze.getExitId());
    file << "<circle cx=\"" << (ent.x + ent.width/2) << "\" cy=\"" << (ent.y + ent.height/2) << "\" r=\"12\" fill=\"#43aa8b\"/>\n";
    file << "<circle cx=\"" << (ext.x + ext.width/2) << "\" cy=\"" << (ext.y + ext.height/2) << "\" r=\"12\" fill=\"#f3722c\"/>\n";
    file << "</svg>\n";
    file << "<p>绿色圆点为入口，橙色圆点为出口，红色粗线为路径。</p>\n";
    file << "</div></body></html>\n";
    file.close();
    std::cout << "蒙德里安迷宫已导出到HTML文件: " << filename << std::endl;
}

void Visualizer::exportMondrianMultiPathsToHTML(const MondrianMaze& maze, const std::vector<std::vector<int>>& paths, int shortestIdx, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建HTML文件: " << filename << std::endl;
        return;
    }
    file << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Mondrian Maze 多路径</title>"
            "<style>body{display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh;margin:0;}"
            ".path-block{margin:32px auto;text-align:center;}svg{display:block;margin:auto;}h3{text-align:center;margin-bottom:0;}</style>"
            "</head><body>\n";
    file << "<h2>闯入蒙德里安名画 - 多路径解法</h2>\n";
    file << "<h3>最短路径为第 " << (shortestIdx+1) << " 条，长度 " << paths[shortestIdx].size() << "</h3>\n";
    for (size_t i = 0; i < paths.size(); ++i) {
        file << "<div class=\"path-block\">\n";
        file << "<svg width=\"400\" height=\"400\" viewBox=\"0 0 800 800\" style=\"background:#fff;box-shadow:0 0 8px #aaa;\">\n";
        
        // 创建当前路径的ID集合
        std::unordered_set<int> current_path_ids(paths[i].begin(), paths[i].end());

        // 画所有房间
        for (const auto& room : maze.getRooms()) {
            bool inPath = current_path_ids.count(room.id);
            std::string fillColor = inPath ? "#9370DB" : room.color; // 高亮色：中紫色
            std::string border = "stroke:#222;stroke-width:3;";
            file << "<rect x=\"" << room.x << "\" y=\"" << room.y << "\" width=\"" << room.width << "\" height=\"" << room.height << "\" fill=\"" << fillColor << "\" style=\"" << border << "\"/>\n";
        }

        // 路径高亮连线（白色以在紫色背景上更突出）
        if (paths[i].size() > 1) {
            for (size_t j = 0; j < paths[i].size() - 1; ++j) {
                const auto& r1 = maze.getRoom(paths[i][j]);
                const auto& r2 = maze.getRoom(paths[i][j+1]);
                file << "<line x1=\"" << (r1.x + r1.width/2) << "\" y1=\"" << (r1.y + r1.height/2) << "\" "
                     << "x2=\"" << (r2.x + r2.width/2) << "\" y2=\"" << (r2.y + r2.height/2) << "\" "
                     << "fill=\"none\" stroke=\"#FFFFFF\" stroke-width=\"4\" stroke-linecap=\"round\" marker-end=\"url(#arrowhead)\"/>\n";
            }
        }
        // 标记入口和出口
        const auto& ent = maze.getRoom(maze.getEntranceId());
        const auto& ext = maze.getRoom(maze.getExitId());
        file << "<circle cx=\"" << (ent.x + ent.width/2) << "\" cy=\"" << (ent.y + ent.height/2) << "\" r=\"12\" fill=\"#43aa8b\"/>\n";
        file << "<circle cx=\"" << (ext.x + ext.width/2) << "\" cy=\"" << (ext.y + ext.height/2) << "\" r=\"12\" fill=\"#f3722c\"/>\n";
        file << "</svg>\n";
        file << "<div>路径 " << (i+1) << (i == (size_t)shortestIdx ? "（最短路径）" : "") << "，长度 " << paths[i].size() << "</div>\n";
        file << "</div>\n";
    }
    file << "<p>每张图绿色路径为该解法，绿色/橙色圆点为入口/出口。</p>\n";
    file << "</body></html>\n";
    file.close();
    std::cout << "多路径 Mondrian 迷宫已导出到HTML文件: " << filename << std::endl;
}
