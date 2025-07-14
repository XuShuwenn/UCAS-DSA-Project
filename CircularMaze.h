#ifndef CIRCULAR_MAZE_H
#define CIRCULAR_MAZE_H

#include "maze.h"
#include <vector>

// 前向声明
class PathFinder;
class Visualizer;

class CircularMaze : public Maze {
    friend class Visualizer; // Visualizer可以访问私有成员
public:
    CircularMaze(int rings);

    // 重写或新增适合圆形迷宫的方法
    void generate() override;
    std::vector<Point> getAccessibleNeighbors(const Point& p) const override;
    bool isValidPosition(const Point& p) const override;

    int getRings() const { return rings; }
    int getCellsInRing(int ring) const;

private:
    int rings;
    std::vector<int> cells_in_ring;
    // 使用一种方式来存储墙壁，例如：
    // 0: clockwise, 1: outward
    std::vector<std::vector<bool>> horizontal_walls; // "水平"墙 (同心圆)
    std::vector<std::vector<bool>> vertical_walls;   // "垂直"墙 (放射状)

    // Helper functions
    std::vector<Point> getAllNeighbors(const Point& p) const;
    void removeWallBetween(const Point& a, const Point& b);
};

#endif // CIRCULAR_MAZE_H 