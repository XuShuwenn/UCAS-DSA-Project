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

// BFS路径搜索，找到从startId到endId的最短路径
std::vector<int> MondrianMaze::findPath(int startId, int endId, int minRooms) const {
    // 队列中存储当前节点
    std::queue<int> q;
    // a_map来存储路径，key是当前节点，value是它的前一个节点
    std::map<int, int> parent_map;
    // 访问过的节点集合
    std::unordered_set<int> visited;

    q.push(startId);
    visited.insert(startId);
    parent_map[startId] = -1; // -1表示没有父节点

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == endId) {
            // 找到终点，开始回溯路径
            std::vector<int> path;
            int node = endId;
            while (node != -1) {
                path.push_back(node);
                node = parent_map[node];
            }
            std::reverse(path.begin(), path.end());
            if(path.size() >= (size_t)minRooms) {
                return path;
            } else {
                return {}; // 路径太短，视为无解
            }
        }

        for (int neighbor : rooms[current].neighbors) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                parent_map[neighbor] = current;
                q.push(neighbor);
            }
        }
    }

    return {}; // 无解
}

// 枚举所有最短路径，最多返回前maxPaths条
void findAllPathsLimited(const MondrianMaze& maze, int start, int end, int maxPaths, int /*maxLength*/, std::vector<std::vector<int>>& allPaths) {
    // 1. 先用BFS求最短路径长度
    std::queue<std::pair<int, int>> q;
    std::unordered_map<int, int> dist;
    q.push({start, 0});
    dist[start] = 0;
    int minLen = -1;
    while (!q.empty()) {
        auto [u, d] = q.front(); q.pop();
        if (u == end) { minLen = d; break; }
        for (int nb : maze.getRoom(u).neighbors) {
            if (!dist.count(nb)) {
                dist[nb] = d + 1;
                q.push({nb, d + 1});
            }
        }
    }
    if (minLen == -1) return; // 无解

    // 2. 用DFS枚举所有长度等于minLen的路径
    std::vector<int> path;
    std::unordered_set<int> visited;
    std::function<void(int, int)> dfs = [&](int u, int depth) {
        if ((int)allPaths.size() >= maxPaths) return;
        path.push_back(u);
        visited.insert(u);
        if (u == end && depth == minLen) {
            allPaths.push_back(path);
        } else if (depth < minLen) {
            for (int nb : maze.getRoom(u).neighbors) {
                if (!visited.count(nb)) {
                    dfs(nb, depth + 1);
                }
            }
        }
        path.pop_back();
        visited.erase(u);
    };
    dfs(start, 0);
} 