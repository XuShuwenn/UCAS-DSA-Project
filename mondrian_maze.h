#ifndef MONDRIAN_MAZE_H
#define MONDRIAN_MAZE_H
#include <vector>
#include <string>
#include <unordered_map>

struct Room {
    int id;
    int x, y, width, height; // 画布坐标和尺寸
    std::string color;
    std::vector<int> neighbors; // 相邻房间id
};

class MondrianMaze {
public:
    MondrianMaze();
    int getRoomCount() const;
    const Room& getRoom(int id) const;
    const std::vector<Room>& getRooms() const;
    std::vector<int> findPath(int startId, int endId, int minRooms = 3) const;
    int getEntranceId() const;
    int getExitId() const;
private:
    std::vector<Room> rooms;
    int entranceId;
    int exitId;
};

void findAllPathsLimited(const MondrianMaze& maze, int start, int end, int maxPaths, int maxLength, std::vector<std::vector<int>>& allPaths);

#endif // MONDRIAN_MAZE_H 