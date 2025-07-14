# C++ 迷宫寻路系统 Makefile

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

# 源文件和目标文件
SOURCES = main.cpp maze.cpp pathfinder.cpp visualizer.cpp CircularMaze.cpp mondrian_maze.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = maze_solver

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "编译完成！可执行文件: $(TARGET)"

# 编译源文件为目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f *.txt *.html
	@echo "清理完成！"

# 重新编译
rebuild: clean all

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 调试模式编译
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

# 发布模式编译
release: CXXFLAGS = -std=c++17 -O3 -DNDEBUG
release: $(TARGET)

# 安装（复制到系统目录）
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	@echo "安装完成！"

# 卸载
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "卸载完成！"

# 创建测试用例
test: $(TARGET)
	@echo "运行测试..."
	./$(TARGET) < test_input.txt || echo "请手动运行程序进行测试"

# 生成文档
docs:
	@echo "生成项目文档..."
	doxygen Doxyfile 2>/dev/null || echo "需要安装doxygen来生成文档"

# 检查内存泄漏（需要valgrind）
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# 代码格式化（需要clang-format）
format:
	clang-format -i *.cpp *.h

# 静态代码分析（需要cppcheck）
analyze:
	cppcheck --enable=all --std=c++17 *.cpp *.h

# 打包源代码
package:
	tar -czf maze_solver_src.tar.gz *.cpp *.h Makefile README.md LICENSE

# 帮助信息
help:
	@echo "可用的make目标："
	@echo "  all      - 编译程序（默认）"
	@echo "  clean    - 清理生成文件"
	@echo "  rebuild  - 重新编译"
	@echo "  run      - 编译并运行程序"
	@echo "  debug    - 调试模式编译"
	@echo "  release  - 发布模式编译"
	@echo "  install  - 安装到系统目录"
	@echo "  uninstall- 从系统目录卸载"
	@echo "  test     - 运行测试"
	@echo "  docs     - 生成文档"
	@echo "  memcheck - 内存泄漏检查"
	@echo "  format   - 代码格式化"
	@echo "  analyze  - 静态代码分析"
	@echo "  package  - 打包源代码"
	@echo "  help     - 显示帮助信息"

# 伪目标声明
.PHONY: all clean rebuild run debug release install uninstall test docs memcheck format analyze package help

# 依赖关系
main.o: main.cpp maze.h pathfinder.h visualizer.h CircularMaze.h mondrian_maze.h
maze.o: maze.cpp maze.h
pathfinder.o: pathfinder.cpp pathfinder.h maze.h
visualizer.o: visualizer.cpp visualizer.h maze.h pathfinder.h CircularMaze.h mondrian_maze.h
CircularMaze.o: CircularMaze.cpp CircularMaze.h maze.h
