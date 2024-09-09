cmake_minimum_required(VERSION 3.20)
project(Snake)

set(CMAKE_CXX_STANDARD 11)

# 设置SDL2头文件路径
set(SDL_INCLUDE_DIR "D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/include")
include_directories(${SDL_INCLUDE_DIR})

# 设置SDL2库文件路径
set(SDL_LIB_DIR "D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib")

# 添加可执行文件
add_executable(Snake main.cpp main.h)
set_target_properties(Snake PROPERTIES LINK_FLAGS "-mconsole")

# 链接SDL2库和SDL_main
target_link_libraries(Snake PRIVATE ${SDL_LIB_DIR}/libSDL2.dll.a mingw32 SDL2main)
