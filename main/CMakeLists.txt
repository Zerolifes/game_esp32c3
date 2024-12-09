# Đặt tệp nguồn cho ứng dụng chính
set(COMPONENT_SRCS "main.cpp" "Cell.cpp" "graphic.cpp" "Maze.cpp" "User.cpp")

# Yêu cầu sử dụng component ssd1306
set(EXTRA_COMPONENT_DIRS ../components/ssd1306)

# Đăng ký component chính
register_component()
