#include "maze.h"
#include <algorithm>
#include <stack>
#include <iostream>
#include <iomanip>

/**
 * 迷宫类实现 - 使用线段表示墙壁
 */

Maze::Maze(int rows, int cols) : rows(rows), cols(cols), rng(std::random_device{}()) {
    // 初始化迷宫网格
    grid.resize(rows, std::vector<MazeCell>(cols));
    
    // 初始化所有格子都有四面墙
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = MazeCell();  // 默认构造函数会设置所有墙壁
        }
    }
    
    // 设置默认入口和出口
    entrance = Point(0, 0);
    exit = Point(rows - 1, cols - 1);
    setCellType(entrance, CellType::ENTRANCE);
    setCellType(exit, CellType::EXIT);
}

const MazeCell& Maze::getCell(int x, int y) const {
    if (!isValidPosition(x, y)) {
        static MazeCell invalidCell;
        return invalidCell;
    }
    return grid[x][y];
}

void Maze::setCellType(int x, int y, CellType type) {
    if (isValidPosition(x, y)) {
        grid[x][y].type = type;
    }
}

bool Maze::hasWall(int x, int y, WallDirection dir) const {
    if (!isValidPosition(x, y)) {
        return true;  // 边界外视为有墙
    }
    return grid[x][y].hasWall(dir);
}

void Maze::setWall(int x, int y, WallDirection dir, bool hasWall) {
    if (!isValidPosition(x, y)) return;
    
    grid[x][y].setWall(dir, hasWall);
    
    // 同时更新相邻格子的对应墙壁
    Point adjacent = getAdjacentCell(Point(x, y), dir);
    if (isValidPosition(adjacent)) {
        WallDirection oppositeDir = getOppositeDirection(dir);
        grid[adjacent.x][adjacent.y].setWall(oppositeDir, hasWall);
    }
}

void Maze::removeWall(int x, int y, WallDirection dir) {
    setWall(x, y, dir, false);
}

Point Maze::getAdjacentCell(const Point& p, WallDirection dir) const {
    switch (dir) {
        case WallDirection::TOP:    return Point(p.x - 1, p.y);
        case WallDirection::RIGHT:  return Point(p.x, p.y + 1);
        case WallDirection::BOTTOM: return Point(p.x + 1, p.y);
        case WallDirection::LEFT:   return Point(p.x, p.y - 1);
        default: return Point(-1, -1);
    }
}

WallDirection Maze::getOppositeDirection(WallDirection dir) const {
    switch (dir) {
        case WallDirection::TOP:    return WallDirection::BOTTOM;
        case WallDirection::RIGHT:  return WallDirection::LEFT;
        case WallDirection::BOTTOM: return WallDirection::TOP;
        case WallDirection::LEFT:   return WallDirection::RIGHT;
        default: return WallDirection::TOP;
    }
}

void Maze::removeWallBetween(const Point& p1, const Point& p2) {
    if (!isValidPosition(p1) || !isValidPosition(p2)) return;
    
    // 确定两个格子的相对位置
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    
    // 只处理相邻格子
    if (abs(dx) + abs(dy) != 1) return;
    
    WallDirection dir1, dir2;
    if (dx == -1) {  // p2在p1上方
        dir1 = WallDirection::TOP;
        dir2 = WallDirection::BOTTOM;
    } else if (dx == 1) {  // p2在p1下方
        dir1 = WallDirection::BOTTOM;
        dir2 = WallDirection::TOP;
    } else if (dy == -1) {  // p2在p1左边
        dir1 = WallDirection::LEFT;
        dir2 = WallDirection::RIGHT;
    } else {  // p2在p1右边
        dir1 = WallDirection::RIGHT;
        dir2 = WallDirection::LEFT;
    }
    
    grid[p1.x][p1.y].removeWall(dir1);
    grid[p2.x][p2.y].removeWall(dir2);
}

void Maze::setEntrance(const Point& p) {
    if (isValidPosition(p)) {
        if (entrance != Point(-1, -1)) {
            setCellType(entrance, CellType::PATH);
        }
        entrance = p;
        setCellType(p, CellType::ENTRANCE);
    }
}

void Maze::setExit(const Point& p) {
    if (isValidPosition(p)) {
        if (exit != Point(-1, -1)) {
            setCellType(exit, CellType::PATH);
        }
        exit = p;
        setCellType(p, CellType::EXIT);
    }
}

void Maze::generateRandomMaze(double wallRemovalProbability) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    // 重新初始化所有格子为有四面墙
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = MazeCell();
        }
    }
    
    // 随机移除一些内部墙壁
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // 尝试移除右墙和下墙（避免重复处理相邻墙壁）
            if (j < cols - 1 && dist(rng) < wallRemovalProbability) {
                removeWall(i, j, WallDirection::RIGHT);
            }
            if (i < rows - 1 && dist(rng) < wallRemovalProbability) {
                removeWall(i, j, WallDirection::BOTTOM);
            }
        }
    }
    
    // 确保入口和出口有通路
    if (entrance.x == 0) {
        removeWall(entrance.x, entrance.y, WallDirection::TOP);
    } else if (entrance.x == rows - 1) {
        removeWall(entrance.x, entrance.y, WallDirection::BOTTOM);
    }
    if (entrance.y == 0) {
        removeWall(entrance.x, entrance.y, WallDirection::LEFT);
    } else if (entrance.y == cols - 1) {
        removeWall(entrance.x, entrance.y, WallDirection::RIGHT);
    }
    
    if (exit.x == 0) {
        removeWall(exit.x, exit.y, WallDirection::TOP);
    } else if (exit.x == rows - 1) {
        removeWall(exit.x, exit.y, WallDirection::BOTTOM);
    }
    if (exit.y == 0) {
        removeWall(exit.x, exit.y, WallDirection::LEFT);
    } else if (exit.y == cols - 1) {
        removeWall(exit.x, exit.y, WallDirection::RIGHT);
    }
}

void Maze::generateMazeWithDFS() {
    // 重新初始化所有格子为有四面墙
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = MazeCell();
            grid[i][j].type = CellType::PATH;
        }
    }
    
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::stack<Point> stack;
    std::uniform_int_distribution<int> dist(0, 3);
    
    // 从入口开始
    Point current = entrance;
    visited[current.x][current.y] = true;
    stack.push(current);
    
    while (!stack.empty()) {
        current = stack.top();
        
        // 获取所有未访问的邻居
        std::vector<Point> unvisitedNeighbors;
        std::vector<WallDirection> directions = {
            WallDirection::TOP, WallDirection::RIGHT, 
            WallDirection::BOTTOM, WallDirection::LEFT
        };
        
        for (WallDirection dir : directions) {
            Point neighbor = getAdjacentCell(current, dir);
            if (isValidPosition(neighbor) && !visited[neighbor.x][neighbor.y]) {
                unvisitedNeighbors.push_back(neighbor);
            }
        }
        
        if (!unvisitedNeighbors.empty()) {
            // 随机选择一个未访问的邻居
            int randIndex = dist(rng) % unvisitedNeighbors.size();
            Point next = unvisitedNeighbors[randIndex];
            
            // 移除当前格子和选中邻居之间的墙
            removeWallBetween(current, next);
            
            // 标记邻居为已访问并加入栈
            visited[next.x][next.y] = true;
            stack.push(next);
        } else {
            stack.pop();
        }
    }
    
    // 设置入口出口类型
    setCellType(entrance, CellType::ENTRANCE);
    setCellType(exit, CellType::EXIT);
}

void Maze::generatePerfectMaze() {
    generateMazeWithDFS();  // 完美迷宫就是DFS生成的迷宫
}

bool Maze::isValidPosition(int x, int y) const {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

bool Maze::canMoveTo(const Point& from, const Point& to) const {
    if (!isValidPosition(from) || !isValidPosition(to)) {
        return false;
    }
    
    // 检查两个格子是否相邻
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    if (abs(dx) + abs(dy) != 1) {
        return false;
    }
    
    // 确定墙的方向
    WallDirection dir;
    if (dx == -1) dir = WallDirection::TOP;
    else if (dx == 1) dir = WallDirection::BOTTOM;
    else if (dy == -1) dir = WallDirection::LEFT;
    else dir = WallDirection::RIGHT;
    
    // 检查墙是否存在
    return !hasWall(from, dir);
}

std::vector<Point> Maze::getAccessibleNeighbors(const Point& p) const {
    std::vector<Point> neighbors;
    std::vector<WallDirection> directions = {
        WallDirection::TOP, WallDirection::RIGHT, 
        WallDirection::BOTTOM, WallDirection::LEFT
    };
    
    for (WallDirection dir : directions) {
        Point neighbor = getAdjacentCell(p, dir);
        if (isValidPosition(neighbor) && !hasWall(p, dir)) {
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}

std::vector<Point> Maze::getAllNeighbors(const Point& p) const {
    std::vector<Point> neighbors;
    std::vector<WallDirection> directions = {
        WallDirection::TOP, WallDirection::RIGHT, 
        WallDirection::BOTTOM, WallDirection::LEFT
    };
    
    for (WallDirection dir : directions) {
        Point neighbor = getAdjacentCell(p, dir);
        if (isValidPosition(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}

void Maze::resetVisited() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j].type == CellType::VISITED) {
                grid[i][j].type = CellType::PATH;
            }
        }
    }
    setCellType(entrance, CellType::ENTRANCE);
    setCellType(exit, CellType::EXIT);
}

void Maze::printMaze() const {
    std::cout << "\n迷宫结构（使用线段表示墙壁）：\n";
    
    // 打印顶部边界
    std::cout << "┌";
    for (int j = 0; j < cols; j++) {
        std::cout << (hasWall(0, j, WallDirection::TOP) ? "───" : "   ");
        if (j < cols - 1) std::cout << "┬";
    }
    std::cout << "┐\n";
    
    for (int i = 0; i < rows; i++) {
        // 打印行内容
        std::cout << (hasWall(i, 0, WallDirection::LEFT) ? "│" : " ");
        for (int j = 0; j < cols; j++) {
            // 打印格子内容
            switch (grid[i][j].type) {
                case CellType::ENTRANCE: std::cout << " S "; break;
                case CellType::EXIT:     std::cout << " E "; break;
                case CellType::VISITED:  std::cout << " · "; break;
                default:                 std::cout << "   "; break;
            }
            
            // 打印右墙
            if (j < cols - 1) {
                std::cout << (hasWall(i, j, WallDirection::RIGHT) ? "│" : " ");
            }
        }
        std::cout << (hasWall(i, cols-1, WallDirection::RIGHT) ? "│" : " ") << "\n";
        
        // 打印底部墙壁（除最后一行）
        if (i < rows - 1) {
            std::cout << (hasWall(i, 0, WallDirection::BOTTOM) || hasWall(i+1, 0, WallDirection::TOP) ? "├" : " ");
            for (int j = 0; j < cols; j++) {
                bool hasBottomWall = hasWall(i, j, WallDirection::BOTTOM) || hasWall(i+1, j, WallDirection::TOP);
                std::cout << (hasBottomWall ? "───" : "   ");
                
                if (j < cols - 1) {
                    bool hasRightWall = hasWall(i, j, WallDirection::RIGHT) || hasWall(i, j+1, WallDirection::LEFT);
                    bool hasBottomRightWall = hasWall(i, j+1, WallDirection::BOTTOM) || hasWall(i+1, j+1, WallDirection::TOP);
                    bool hasRightBottomWall = hasWall(i+1, j, WallDirection::RIGHT) || hasWall(i+1, j+1, WallDirection::LEFT);
                    
                    if (hasBottomWall && hasRightWall && hasBottomRightWall && hasRightBottomWall) {
                        std::cout << "┼";
                    } else if (hasBottomWall && hasBottomRightWall) {
                        std::cout << "┬";
                    } else if (hasRightWall && hasRightBottomWall) {
                        std::cout << "┤";
                    } else if (hasBottomWall || hasBottomRightWall) {
                        std::cout << "─";
                    } else if (hasRightWall || hasRightBottomWall) {
                        std::cout << "│";
                    } else {
                        std::cout << " ";
                    }
                }
            }
            bool hasBottomWall = hasWall(i, cols-1, WallDirection::BOTTOM) || hasWall(i+1, cols-1, WallDirection::TOP);
            bool hasRightWall = hasWall(i, cols-1, WallDirection::RIGHT) || hasWall(i+1, cols-1, WallDirection::RIGHT);
            std::cout << (hasBottomWall && hasRightWall ? "┤" : (hasBottomWall ? "─" : (hasRightWall ? "│" : " "))) << "\n";
        }
    }
    
    // 打印底部边界
    std::cout << "└";
    for (int j = 0; j < cols; j++) {
        std::cout << (hasWall(rows-1, j, WallDirection::BOTTOM) ? "───" : "   ");
        if (j < cols - 1) std::cout << "┴";
    }
    std::cout << "┘\n";
}

void Maze::printMazeWithPath(const std::vector<Point>& path) const {
    // 创建临时副本
    Maze temp = clone();
    
    // 标记路径
    for (const Point& p : path) {
        if (p != entrance && p != exit) {
            temp.setCellType(p, CellType::VISITED);
        }
    }
    
    temp.printMaze();
}

void Maze::printMazeDetailed() const {
    std::cout << "\n详细迷宫信息：\n";
    std::cout << "尺寸: " << rows << "×" << cols << "\n";
    std::cout << "入口: (" << entrance.x << ", " << entrance.y << ")\n";
    std::cout << "出口: (" << exit.x << ", " << exit.y << ")\n";
    std::cout << "墙壁数量: " << countWalls() << "\n";
    std::cout << "开放通道: " << countOpenPaths() << "\n";
    std::cout << "连通性: " << std::fixed << std::setprecision(2) << getConnectivity() * 100 << "%\n";
    
    printMaze();
}

Maze Maze::clone() const {
    Maze copy(rows, cols);
    copy.grid = grid;
    copy.entrance = entrance;
    copy.exit = exit;
    return copy;
}

int Maze::countWalls() const {
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int d = 0; d < 4; d++) {
                if (grid[i][j].hasWall(static_cast<WallDirection>(d))) {
                    count++;
                }
            }
        }
    }
    return count / 2;  // 每面墙被计算了两次
}

int Maze::countOpenPaths() const {
    int totalPossibleWalls = 2 * rows * cols - rows - cols;  // 内部墙壁总数
    return totalPossibleWalls - countWalls();
}

double Maze::getConnectivity() const {
    int totalPossibleWalls = 2 * rows * cols - rows - cols;
    int openPaths = countOpenPaths();
    return static_cast<double>(openPaths) / totalPossibleWalls;
}
