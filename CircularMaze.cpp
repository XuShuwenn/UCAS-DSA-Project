#include "CircularMaze.h"
#include <cmath>
#include <random>
#include <stack>
#include <algorithm>
#include <vector>

CircularMaze::CircularMaze(int rings) : Maze(rings, 0), rings(rings) {
    cells_in_ring.reserve(rings);
    for (int i = 0; i < rings; ++i) {
        // Let's use a scheme where each ring has a multiple of the previous one for easy mapping.
        // E.g., 6, 12, 24, ...
        cells_in_ring.push_back(6 * std::pow(2, i));
    }

    // Walls: 0 for clockwise, 1 for outward
    horizontal_walls.resize(rings);
    vertical_walls.resize(rings);
    for (int i = 0; i < rings; ++i) {
        horizontal_walls[i].assign(cells_in_ring[i], true); // Concentric walls
        vertical_walls[i].assign(cells_in_ring[i], true);   // Radial walls
    }

    setEntrance({0, 0});
    setExit({rings - 1, 0});
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
    int i = p.y;
    int num_cells = cells_in_ring[r];

    // Clockwise
    if (!vertical_walls[r][i]) {
        neighbors.push_back({r, (i + 1) % num_cells});
    }
    // Counter-clockwise
    if (!vertical_walls[r][(i - 1 + num_cells) % num_cells]) {
        neighbors.push_back({r, (i - 1 + num_cells) % num_cells});
    }
    // Outward
    if (r + 1 < rings && !horizontal_walls[r][i]) {
        int outer_cells = cells_in_ring[r+1];
        float ratio = (float)outer_cells / num_cells;
        neighbors.push_back({r + 1, (int)(i * ratio)});
        if (ratio > 1.5) { // Connect to two cells in the outer ring if ratio is large enough
             neighbors.push_back({r + 1, (int)(i * ratio) + 1});
        }
    }
    // Inward
    if (r > 0) {
        int inner_cells = cells_in_ring[r-1];
        float ratio = (float)num_cells / inner_cells;
        Point inner_neighbor = {r - 1, (int)(i / ratio)};
        if (!horizontal_walls[inner_neighbor.x][inner_neighbor.y]) {
            neighbors.push_back(inner_neighbor);
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
    int i = p.y;
    int num_cells = cells_in_ring[r];

    // Clockwise and Counter-clockwise
    neighbors.push_back({r, (i + 1) % num_cells});
    neighbors.push_back({r, (i - 1 + num_cells) % num_cells});
    
    // Outward neighbors
    if (r + 1 < rings) {
        int outer_cells = cells_in_ring[r+1];
        float ratio = (float)outer_cells / num_cells;
        neighbors.push_back({r + 1, (int)(i * ratio)});
         if (ratio > 1.5) {
             neighbors.push_back({r + 1, (int)(i * ratio) + 1});
        }
    }

    // Inward neighbor
    if (r > 0) {
        int inner_cells = cells_in_ring[r-1];
        float ratio = (float)num_cells / inner_cells;
        neighbors.push_back({r - 1, (int)(i / ratio)});
    }
    return neighbors;
}

void CircularMaze::removeWallBetween(const Point& a, const Point& b) {
    if (a.x == b.x) { // Same ring, must be a vertical (radial) wall
        int i = std::min(a.y, b.y);
        this->vertical_walls[a.x][i] = false;
    } else { // Different rings, must be a horizontal (concentric) wall
        const Point& inner = (a.x < b.x) ? a : b;
        this->horizontal_walls[inner.x][inner.y] = false;
    }
} 