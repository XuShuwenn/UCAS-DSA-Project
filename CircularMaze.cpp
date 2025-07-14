#include "CircularMaze.h"
#include <cmath>
#include <random>
#include <stack>
#include <algorithm>
#include <vector>

CircularMaze::CircularMaze(int rings) : Maze(std::max(3, rings), 0), rings(std::max(3, rings)) {
    cells_in_ring.reserve(this->rings);
    for (int i = 0; i < this->rings; ++i) {
        int n = std::max(6, 6 * (1 << i)); // 每一环至少6个格子
        cells_in_ring.push_back(n);
    }
    horizontal_walls.resize(this->rings);
    vertical_walls.resize(this->rings);
    for (int i = 0; i < this->rings; ++i) {
        horizontal_walls[i].assign(cells_in_ring[i], true);
        vertical_walls[i].assign(cells_in_ring[i], true);
    }
    setEntrance({0, 0});
    setExit({this->rings - 1, 0});
}

void CircularMaze::generate() {
    std::stack<Point> s;
    std::vector<std::vector<bool>> visited(rings);
    for(int i = 0; i < rings; ++i) {
        visited[i].assign(cells_in_ring[i], false);
    }
    std::mt19937 rng(std::random_device{}());
    Point start = {0, 0};
    s.push(start);
    visited[start.x][start.y] = true;
    while(!s.empty()){
        Point current = s.top();
        std::vector<Point> neighbors = getAllNeighbors(current);
        std::shuffle(neighbors.begin(), neighbors.end(), rng);
        Point next = {-1, -1};
        for(const auto& n : neighbors) {
            if(isValidPosition(n) && !visited[n.x][n.y]) {
                next = n;
                break;
            }
        }
        if(next.x != -1){
            removeWallBetween(current, next);
            visited[next.x][next.y] = true;
            s.push(next);
        } else {
            s.pop();
        }
    }
}

std::vector<Point> CircularMaze::getAccessibleNeighbors(const Point& p) const {
    std::vector<Point> neighbors;
    int r = p.x;
    if (r < 0 || r >= rings) return neighbors;
    int i = p.y;
    int num_cells = cells_in_ring[r];
    if (i < 0 || i >= num_cells) return neighbors;
    // Clockwise
    if (num_cells > 0 && !vertical_walls[r][i]) {
        neighbors.push_back({r, (i + 1) % num_cells});
    }
    // Counter-clockwise
    if (num_cells > 0 && !vertical_walls[r][(i - 1 + num_cells) % num_cells]) {
        neighbors.push_back({r, (i - 1 + num_cells) % num_cells});
    }
    // Outward
    if (r + 1 < rings && num_cells > 0 && !horizontal_walls[r][i]) {
        int outer_cells = cells_in_ring[r+1];
        float ratio = (float)outer_cells / num_cells;
        neighbors.push_back({r + 1, (int)(i * ratio) % outer_cells});
        if (ratio > 1.5) {
             neighbors.push_back({r + 1, ((int)(i * ratio) + 1) % outer_cells});
        }
    }
    // Inward
    if (r > 0) {
        int inner_cells = cells_in_ring[r-1];
        float ratio = (float)num_cells / inner_cells;
        int inner_idx = (int)(i / ratio);
        if (inner_idx >= 0 && inner_idx < inner_cells && !horizontal_walls[r-1][inner_idx]) {
            neighbors.push_back({r - 1, inner_idx});
        }
    }
    return neighbors;
}

bool CircularMaze::isValidPosition(const Point& p) const {
    if (p.x < 0 || p.x >= rings) return false;
    if (p.y < 0 || p.y >= cells_in_ring[p.x]) return false;
    return true;
}

int CircularMaze::getCellsInRing(int ring) const {
    if (ring >= 0 && ring < rings) {
        return cells_in_ring[ring];
    }
    return 0;
} 

// --- Helper Implementations ---

std::vector<Point> CircularMaze::getAllNeighbors(const Point& p) const {
    std::vector<Point> neighbors;
    int r = p.x;
    if (r < 0 || r >= rings) return neighbors;
    int i = p.y;
    int num_cells = cells_in_ring[r];
    if (i < 0 || i >= num_cells) return neighbors;
    // Clockwise and Counter-clockwise
    neighbors.push_back({r, (i + 1) % num_cells});
    neighbors.push_back({r, (i - 1 + num_cells) % num_cells});
    // Outward neighbors
    if (r + 1 < rings) {
        int outer_cells = cells_in_ring[r+1];
        float ratio = (float)outer_cells / num_cells;
        neighbors.push_back({r + 1, (int)(i * ratio) % outer_cells});
        if (ratio > 1.5) {
            neighbors.push_back({r + 1, ((int)(i * ratio) + 1) % outer_cells});
        }
    }
    // Inward neighbor
    if (r > 0) {
        int inner_cells = cells_in_ring[r-1];
        float ratio = (float)num_cells / inner_cells;
        int inner_idx = (int)(i / ratio);
        if (inner_idx >= 0 && inner_idx < inner_cells)
            neighbors.push_back({r - 1, inner_idx});
    }
    return neighbors;
}

void CircularMaze::removeWallBetween(const Point& a, const Point& b) {
    if (a.x == b.x) { // Same ring, must be a vertical (radial) wall
        int i = std::min(a.y, b.y);
        if (a.x >= 0 && a.x < rings && i >= 0 && i < (int)vertical_walls[a.x].size())
            this->vertical_walls[a.x][i] = false;
    } else { // Different rings, must be a horizontal (concentric) wall
        const Point& inner = (a.x < b.x) ? a : b;
        if (inner.x >= 0 && inner.x < rings && inner.y >= 0 && inner.y < (int)horizontal_walls[inner.x].size())
            this->horizontal_walls[inner.x][inner.y] = false;
    }
} 