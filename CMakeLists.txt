cmake_minimum_required(VERSION 3.20)
project(Gluttonous_snake)

set(CMAKE_CXX_STANDARD 14)

# 设置SDL2、SDL2_image、SDL2_mixer和SDL2_ttf的头文件路径
set(SDL_INCLUDE_DIR "D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/include")
include_directories(${SDL_INCLUDE_DIR})

# 设置SDL2、SDL2_image、SDL2_mixer和SDL2_ttf的库文件路径
set(SDL_LIB_DIR "D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib")
link_directories(${SDL_LIB_DIR})

# 添加可执行文件
add_executable(Snake main.cpp)

# 链接SDL2、SDL2_image、SDL2_mixer、SDL2_ttf库和SDL_main
target_link_libraries(Snake PRIVATE
        "${SDL_LIB_DIR}/libSDL2main.a"
        "${SDL_LIB_DIR}/libSDL2.dll.a"
        "${SDL_LIB_DIR}/libSDL2_image.dll.a"
        "${SDL_LIB_DIR}/libSDL2_mixer.dll.a"  # 添加SDL2_mixer库的链接
        "${SDL_LIB_DIR}/libSDL2_ttf.dll.a" #添加SDL2_ttf库的链接
)

# 设置编译时的链接标志（如果需要控制台输出）
set_target_properties(Snake PROPERTIES LINK_FLAGS "-mconsole")
