#include "mondrian_maze.h"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <map>
#include <cstdlib>
#include <ctime>

// 颜色池（不含白色）
static const std::vector<std::string> mondrian_colors = {
    "#e63946", // 红
    "#f4d35e", // 黄
    "#1d3557", // 蓝
    "#000000", // 黑
    "#43aa8b", // 绿
    "#f3722c", // 橙
    "#9d4edd", // 紫
    "#f8961e", // 橙黄
    "#577590", // 蓝灰
    "#f94144"  // 深红
};

struct Block {
    int x, y, w, h;
    int colorIdx;
};

// 优先分割最大块，交替分割方向
void splitBlocksMondrian(std::vector<Block>& blocks, int x, int y, int w, int h, int targetBlocks) {
    struct QBlock { int x, y, w, h, depth; };
    std::vector<QBlock> q = { {x, y, w, h, 0} };
    while ((int)blocks.size() + (int)q.size() < targetBlocks) {
        // 找到面积最大的块分割
        auto it = std::max_element(q.begin(), q.end(), [](const QBlock& a, const QBlock& b) {
            return a.w * a.h < b.w * b.h;
        });
        if (it == q.end()) break;
        QBlock blk = *it;
        q.erase(it);
        bool splitVert = (blk.depth % 2 == 0);
        if (blk.w > blk.h) splitVert = true;
        if (blk.h > blk.w) splitVert = false;
        if (splitVert && blk.w >= 120) {
            int sw = 60 + rand() % (blk.w - 60);
            q.push_back({blk.x, blk.y, sw, blk.h, blk.depth+1});
            q.push_back({blk.x+sw, blk.y, blk.w-sw, blk.h, blk.depth+1});
        } else if (!splitVert && blk.h >= 120) {
            int sh = 60 + rand() % (blk.h - 60);
            q.push_back({blk.x, blk.y, blk.w, sh, blk.depth+1});
            q.push_back({blk.x, blk.y+sh, blk.w, blk.h-sh, blk.depth+1});
        } else {
            // 无法再分割，直接作为块
            blocks.push_back({blk.x, blk.y, blk.w, blk.h, (int)blocks.size() % mondrian_colors.size()});
        }
    }
    // 剩余的都作为块
    for (const auto& blk : q) {
        blocks.push_back({blk.x, blk.y, blk.w, blk.h, (int)blocks.size() % mondrian_colors.size()});
    }
}

MondrianMaze::MondrianMaze() {
    srand(time(0));
    while (true) {
        // 1. 生成色块
        std::vector<Block> blocks;
        splitBlocksMondrian(blocks, 0, 0, 800, 800, 100); // 目标100块
        // 2. 转为Room
        rooms.clear();
        for (size_t i = 0; i < blocks.size(); ++i) {
            const auto& b = blocks[i];
            rooms.push_back({(int)i, b.x, b.y, b.w, b.h, mondrian_colors[b.colorIdx], {}});
        }
        // 3. 建立邻接关系（修正版）
        for (size_t i = 0; i < rooms.size(); ++i) {
            for (size_t j = i + 1; j < rooms.size(); ++j) {
                auto& r1 = rooms[i];
                auto& r2 = rooms[j];
                // 检查水平相邻 (r1在r2左边或右边)
                if ((r1.x + r1.width == r2.x || r2.x + r2.width == r1.x) &&
                    (std::max(r1.y, r2.y) < std::min(r1.y + r1.height, r2.y + r2.height))) {
                    r1.neighbors.push_back(r2.id);
                    r2.neighbors.push_back(r1.id);
                }
                // 检查垂直相邻 (r1在r2上方或下方)
                if ((r1.y + r1.height == r2.y || r2.y + r2.height == r1.y) &&
                    (std::max(r1.x, r2.x) < std::min(r1.x + r1.width, r2.x + r2.width))) {
                    r1.neighbors.push_back(r2.id);
                    r2.neighbors.push_back(r1.id);
                }
            }
        }
        // 4. 入口/出口选择为最远的两个房间
        int maxDist = -1, ent = 0, ext = 0;
        for (size_t i = 0; i < rooms.size(); ++i) {
            for (size_t j = 0; j < rooms.size(); ++j) {
                int dx = (rooms[i].x + rooms[i].width/2) - (rooms[j].x + rooms[j].width/2);
                int dy = (rooms[i].y + rooms[i].height/2) - (rooms[j].y + rooms[j].height/2);
                int d = dx*dx + dy*dy;
                if (d > maxDist) { maxDist = d; ent = i; ext = j; }
            }
        }
        entranceId = ent;
        exitId = ext;
        // 5. 检查可解性和路径长度
        std::vector<int> path = findPath(entranceId, exitId, 1);
        if (!path.empty() && path.size() >= 3) break;
        // 否则重新生成
    }
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

// 枚举所有路径，最多返回前maxPaths条，且路径长度不超过maxLength
void findAllPathsLimited(const MondrianMaze& maze, int start, int end, int maxPaths, int maxLength, std::vector<std::vector<int>>& allPaths) {
    std::vector<int> path;
    std::unordered_set<int> visited;
    std::function<void(int)> dfs = [&](int u) {
        if ((int)allPaths.size() >= maxPaths || (int)path.size() >= maxLength) {
            return;
        }

        path.push_back(u);
        visited.insert(u);

        if (u == end) {
            allPaths.push_back(path);
        } else {
            for (int nb : maze.getRoom(u).neighbors) {
                if (visited.find(nb) == visited.end()) {
                    dfs(nb);
                }
            }
        }
        
        path.pop_back();
        visited.erase(u);
    };
    dfs(start);
} 