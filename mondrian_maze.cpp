#include "mondrian_maze.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

// 经典蒙德里安布局（手动定义，4房间示例，可扩展）
MondrianMaze::MondrianMaze() {
    // 房间定义: id, x, y, width, height, color, neighbors
    rooms = {
        {0, 0, 0, 120, 200, "#e63946", {1,2}},         // 红色
        {1, 120, 0, 180, 120, "#457b9d", {0,2,3}},     // 蓝色
        {2, 0, 200, 120, 120, "#f1faee", {0,1,3}},     // 白色
        {3, 120, 120, 180, 200, "#f4d35e", {1,2}}      // 黄色
    };
    entranceId = 0;
    exitId = 3;
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