# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\CLion 2024.1.1\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "D:\CLion 2024.1.1\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\clion-codes\Gluttonous-snake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\clion-codes\Gluttonous-snake\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Snake.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Snake.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Snake.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Snake.dir/flags.make

CMakeFiles/Snake.dir/main.cpp.obj: CMakeFiles/Snake.dir/flags.make
CMakeFiles/Snake.dir/main.cpp.obj: CMakeFiles/Snake.dir/includes_CXX.rsp
CMakeFiles/Snake.dir/main.cpp.obj: D:/clion-codes/Gluttonous-snake/main.cpp
CMakeFiles/Snake.dir/main.cpp.obj: CMakeFiles/Snake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\clion-codes\Gluttonous-snake\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Snake.dir/main.cpp.obj"
	"D:\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Snake.dir/main.cpp.obj -MF CMakeFiles\Snake.dir\main.cpp.obj.d -o CMakeFiles\Snake.dir\main.cpp.obj -c D:\clion-codes\Gluttonous-snake\main.cpp

CMakeFiles/Snake.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Snake.dir/main.cpp.i"
	"D:\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\clion-codes\Gluttonous-snake\main.cpp > CMakeFiles\Snake.dir\main.cpp.i

CMakeFiles/Snake.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Snake.dir/main.cpp.s"
	"D:\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\clion-codes\Gluttonous-snake\main.cpp -o CMakeFiles\Snake.dir\main.cpp.s

# Object files for target Snake
Snake_OBJECTS = \
"CMakeFiles/Snake.dir/main.cpp.obj"

# External object files for target Snake
Snake_EXTERNAL_OBJECTS =

Snake.exe: CMakeFiles/Snake.dir/main.cpp.obj
Snake.exe: CMakeFiles/Snake.dir/build.make
Snake.exe: D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib/libSDL2main.a
Snake.exe: D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib/libSDL2.dll.a
Snake.exe: D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib/libSDL2_image.dll.a
Snake.exe: D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib/libSDL2_mixer.dll.a
Snake.exe: D:/SDLpaint/SDL2/mingw(CLion+VSC)/SDL2-2.26.0-allinone/x86_64-w64-mingw32/lib/libSDL2_ttf.dll.a
Snake.exe: CMakeFiles/Snake.dir/linkLibs.rsp
Snake.exe: CMakeFiles/Snake.dir/objects1.rsp
Snake.exe: CMakeFiles/Snake.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=D:\clion-codes\Gluttonous-snake\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Snake.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Snake.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Snake.dir/build: Snake.exe
.PHONY : CMakeFiles/Snake.dir/build

CMakeFiles/Snake.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Snake.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Snake.dir/clean

CMakeFiles/Snake.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\clion-codes\Gluttonous-snake D:\clion-codes\Gluttonous-snake D:\clion-codes\Gluttonous-snake\cmake-build-debug D:\clion-codes\Gluttonous-snake\cmake-build-debug D:\clion-codes\Gluttonous-snake\cmake-build-debug\CMakeFiles\Snake.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Snake.dir/depend

