# 蒙德里安迷宫路径搜索系统

## 项目特性

- **多种迷宫类型**：支持经典格点迷宫、蒙德里安风格迷宫、圆形迷宫等多种生成方式。
- **多算法寻路**：内置广度优先搜索（BFS）、深度优先搜索（DFS）等多种路径搜索算法，保证最短路径可查找。
- **多路径支持**：可查找并导出蒙德里安迷宫的前6条最短路径，适合算法教学与可视化演示。
- **终端与HTML可视化**：支持终端ASCII可视化和美观的HTML导出，HTML文件自动居中显示，适合浏览器查看。
- **结构清晰，易于扩展**：采用模块化设计，便于添加新迷宫类型或算法。

---

## 项目框架

- **main.cpp**：主程序入口，负责用户交互、迷宫生成、算法选择、结果导出等流程控制。
- **maze.h / maze.cpp**：格点迷宫的核心数据结构与生成、操作方法。
- **CircularMaze.h / CircularMaze.cpp**：圆形迷宫的数据结构与生成、操作方法。
- **mondrian_maze.h / mondrian_maze.cpp**：蒙德里安风格迷宫的生成、房间分割、邻接关系与多路径搜索。
- **pathfinder.h / pathfinder.cpp**：通用路径搜索算法（BFS、DFS等），支持多种迷宫类型。
- **visualizer.h / visualizer.cpp**：终端与HTML可视化输出，支持多路径高亮、SVG导出等。
- **Makefile**：一键编译脚本，自动管理依赖。
- **.gitignore**：忽略中间文件、可执行文件、HTML导出文件等。

classDiagram
    class MazeApplication {
        -unique_ptr<Maze> maze
        -unique_ptr<PathFinder> pathFinder
        -unique_ptr<Visualizer> visualizer
        +run()
        +createMaze()
        +solveMazeMenu()
        +exportMenu()
    }

    class Maze {
        <<Abstract>>
        #width
        #height
        +virtual generate()
        +virtual getAccessibleNeighbors()
        +virtual isValidPosition()
    }

    class RectangularMaze {
        // Inherits from Maze
        -grid
        +generate() override
        +getAccessibleNeighbors() override
    }

    class CircularMaze {
        // Inherits from Maze
        -rings
        -sectors
        -horizontal_walls
        -vertical_walls
        +generate() override
        +getAccessibleNeighbors() override
    }

    class MondrianMaze {
        // Inherits from Maze
        -rooms
        -adjacency
        +generate() override
        +getAccessibleNeighbors() override
        +findMultiplePaths()
    }

    class PathFinder {
        +findPathDFS(Maze) SearchResult
        +findPathBFS(Maze) SearchResult
        +findMultipleShortestPaths(Maze, k) SearchResult[]
    }

    class Visualizer {
        +displayMaze(Maze)
        +exportToHTML(Maze, paths)
        +exportCircularToHTML(CircularMaze, paths)
        +exportMondrianToHTML(MondrianMaze, paths)
    }

    MazeApplication --> Maze : Creates
    MazeApplication --> PathFinder : Uses
    MazeApplication --> Visualizer : Uses
    PathFinder "1" -- "1" Maze : Uses (for solving)
    Visualizer "1" -- "1" Maze : Uses (for drawing)
    Maze <|-- RectangularMaze
    Maze <|-- CircularMaze
    Maze <|-- MondrianMaze

---

## 项目文件结构

```
UCAS-DSA-Project/
├── src/                    # 源代码目录
│   ├── main.cpp
│   ├── maze.cpp
│   ├── CircularMaze.cpp
│   ├── mondrian_maze.cpp
│   ├── pathfinder.cpp
│   └── visualizer.cpp
├── include/                # 头文件目录
│   ├── maze.h
│   ├── CircularMaze.h
│   ├── mondrian_maze.h
│   ├── pathfinder.h
│   └── visualizer.h
├── Makefile                # 构建脚本
├── LICENSE                 
├── .gitignore              
└── README.md               
```

---

## 使用说明

### 依赖环境

- 支持 C++11 及以上标准的编译器（如 g++）
- 推荐 Linux/macOS/WSL 环境，Windows 亦可

### 编译方法

```bash
make
```
或手动编译：
```bash
g++ -std=c++11 -Iinclude src/*.cpp -o maze_solver
```

### 运行方法

```bash
./maze_solver
```
根据终端提示选择迷宫类型、生成方式、算法、导出等操作。

### 主要功能菜单

1. 生成迷宫（支持格点、圆形、蒙德里安风格）
2. 选择路径搜索算法（BFS、DFS等）
3. 查找并显示路径（支持多路径）
4. 导出HTML可视化文件（自动居中，多路径高亮）

---

## 测试用例

### 1. 生成并求解经典格点迷宫

- 选择“生成迷宫”，输入尺寸如 10×10
- 选择“DFS”或“随机”生成方式
- 选择“BFS”算法求解
- 可导出HTML文件查看路径

### 2. 生成并求解蒙德里安迷宫（多路径）

- 选择“生成迷宫”->“蒙德里安风格”
- 自动生成色块分割迷宫
- 选择“多路径导出”，可导出前6条最短路径的HTML文件
- 打开HTML文件，查看所有路径高亮效果

### 3. 生成并求解圆形迷宫

- 选择“生成迷宫”->“圆形迷宫”
- 输入环数等参数
- 选择算法求解并导出HTML

---

## 其他说明

- 所有HTML导出文件均自动居中，适合演示与教学。
- 代码结构清晰，便于二次开发和扩展。
- 如需自定义迷宫类型或算法，可在 `src/` 和 `include/` 目录下扩展相应模块。

---



