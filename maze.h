#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <iostream>
#include <random>
#include <queue>

/**
 * 迷宫类 - 使用线段表示墙壁的迷宫系统
 * 功能：
 * 1. 表示m×n的迷宫矩阵，每个格子有四个边界线段
 * 2. 随机生成迷宫或手动设置迷宫
 * 3. 设置入口和出口
 * 4. 提供迷宫访问接口
 */

// 墙壁方向枚举
enum class WallDirection {
    TOP = 0,     // 上边墙
    RIGHT = 1,   // 右边墙
    BOTTOM = 2,  // 下边墙
    LEFT = 3     // 左边墙
};

// 格子类型
enum class CellType {
    PATH = 0,      // 可通行路径
    ENTRANCE = 1,  // 入口
    EXIT = 2,      // 出口
    VISITED = 3    // 已访问（用于路径标记）
};

// 坐标结构
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

// 迷宫格子结构体 - 使用线段表示墙壁
struct MazeCell {
    bool walls[4];  // 四个方向的墙：上、右、下、左
    CellType type;  // 格子类型
    
    MazeCell() : type(CellType::PATH) {
        // 默认所有墙都存在
        for (int i = 0; i < 4; i++) {
            walls[i] = true;
        }
    }
    
    // 检查某个方向是否有墙
    bool hasWall(WallDirection dir) const {
        return walls[static_cast<int>(dir)];
    }
    
    // 设置某个方向的墙
    void setWall(WallDirection dir, bool hasWall) {
        walls[static_cast<int>(dir)] = hasWall;
    }
    
    // 移除某个方向的墙
    void removeWall(WallDirection dir) {
        walls[static_cast<int>(dir)] = false;
    }
};

class Maze {
private:
    int rows, cols;                              // 迷宫的行数和列数
    std::vector<std::vector<MazeCell>> grid;     // 迷宫矩阵（使用线段墙壁）
    Point entrance, exit;                        // 入口和出口坐标
    std::mt19937 rng;                           // 随机数生成器

    // 获取相邻格子的坐标和对应的墙方向
    Point getAdjacentCell(const Point& p, WallDirection dir) const;
    WallDirection getOppositeDirection(WallDirection dir) const;

public:
    // 构造函数
    Maze(int rows, int cols);
    
    // 析构函数
    ~Maze() = default;

    // 基本访问函数
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    
    // 格子操作
    const MazeCell& getCell(int x, int y) const;
    const MazeCell& getCell(const Point& p) const { return getCell(p.x, p.y); }
    void setCellType(int x, int y, CellType type);
    void setCellType(const Point& p, CellType type) { setCellType(p.x, p.y, type); }
    
    // 墙壁操作
    bool hasWall(int x, int y, WallDirection dir) const;
    bool hasWall(const Point& p, WallDirection dir) const { return hasWall(p.x, p.y, dir); }
    void setWall(int x, int y, WallDirection dir, bool hasWall);
    void setWall(const Point& p, WallDirection dir, bool hasWall) { setWall(p.x, p.y, dir, hasWall); }
    void removeWall(int x, int y, WallDirection dir);
    void removeWall(const Point& p, WallDirection dir) { removeWall(p.x, p.y, dir); }
    
    // 移除两个相邻格子之间的墙
    void removeWallBetween(const Point& p1, const Point& p2);
    
    // 入口和出口
    Point getEntrance() const { return entrance; }
    Point getExit() const { return exit; }
    void setEntrance(const Point& p);
    void setExit(const Point& p);
    
    // 迷宫生成
    void generateRandomMaze(double wallRemovalProbability = 0.6);
    void generateMazeWithDFS();  // 使用深度优先搜索生成迷宫
    void generatePerfectMaze();  // 生成完美迷宫（每两点间只有唯一路径）
    void loadFromWallArray(const std::vector<std::vector<std::vector<bool>>>& wallData);
    
    // 实用函数
    bool isValidPosition(int x, int y) const;
    bool isValidPosition(const Point& p) const { return isValidPosition(p.x, p.y); }
    bool canMoveTo(const Point& from, const Point& to) const;  // 检查两个相邻格子间是否可通行
    std::vector<Point> getAccessibleNeighbors(const Point& p) const;  // 获取可到达的邻居
    std::vector<Point> getAllNeighbors(const Point& p) const;         // 获取所有邻居（不考虑墙壁）
    
    // 重置迷宫状态（清除访问标记）
    void resetVisited();
    
    // 打印迷宫（控制台输出 - 使用线段显示）
    void printMaze() const;
    void printMazeWithPath(const std::vector<Point>& path) const;
    void printMazeDetailed() const;  // 详细显示所有墙壁线段
    
    // 克隆迷宫（用于保存原始状态）
    Maze clone() const;
    
    // 迷宫统计信息
    int countWalls() const;
    int countOpenPaths() const;
    double getConnectivity() const;  // 计算迷宫连通性
};

#endif // MAZE_H
