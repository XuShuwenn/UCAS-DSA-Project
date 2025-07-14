#include "mondrian_maze.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

// 仿照用户提供的蒙德里安画作设计的迷宫布局
MondrianMaze::MondrianMaze() {
    // 房间定义: id, x, y, width, height, color, neighbors
    rooms = {
        {0, 0, 0, 40, 160, "#000000", {1,2,8}},
        {1, 40, 0, 320, 40, "#1d3557", {0,2,3,9}},
        {2, 40, 40, 80, 80, "#f1faee", {0,1,3,5}},
        {3, 120, 40, 120, 120, "#f4d35e", {1,2,4,6}},
        {4, 240, 40, 120, 120, "#e63946", {3,7,9}},
        {5, 40, 120, 80, 80, "#f1faee", {2,6,8,10}},
        {6, 160, 160, 80, 80, "#e63946", {3,5,7,11}},
        {7, 240, 160, 120, 40, "#1d3557", {4,6,12}},
        {8, 0, 160, 40, 120, "#f1faee", {0,5,10}},
        {9, 360, 0, 40, 160, "#f4d35e", {1,4,13}},
        {10, 0, 280, 120, 80, "#f1faee", {5,8,11}},
        {11, 120, 280, 120, 80, "#000000", {6,10,12}},
        {12, 240, 200, 120, 80, "#1d3557", {7,11,13}},
        {13, 360, 160, 40, 200, "#f1faee", {9,12}}
    };
    entranceId = 0; // 左上黑色块
    exitId = 13;    // 右下白色块
}

int MondrianMaze::getRoomCount() const { return rooms.size(); }
const Room& MondrianMaze::getRoom(int id) const { return rooms.at(id); }
const std::vector<Room>& MondrianMaze::getRooms() const { return rooms; }
int MondrianMaze::getEntranceId() const { return entranceId; }
int MondrianMaze::getExitId() const { return exitId; }

// BFS路径搜索，要求经过至少minRooms个房间
std::vector<int> MondrianMaze::findPath(int startId, int endId, int minRooms) const {
    std::queue<std::vector<int>> q;
    std::unordered_set<int> visited;
    q.push({startId});
    while (!q.empty()) {
        auto path = q.front(); q.pop();
        int curr = path.back();
        if (curr == endId && path.size() >= (size_t)minRooms) return path;
        for (int nb : rooms[curr].neighbors) {
            if (std::find(path.begin(), path.end(), nb) == path.end()) {
                auto next = path;
                next.push_back(nb);
                q.push(next);
            }
        }
    }
    return {}; // 无解
} 