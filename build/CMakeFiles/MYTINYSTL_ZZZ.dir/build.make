# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = E:\Software\cmake\bin\cmake.exe

# The command to remove a file.
RM = E:\Software\cmake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\Study\code\c++\mytinystl-zzz

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\Study\code\c++\mytinystl-zzz\build

# Include any dependencies generated for this target.
include CMakeFiles/MYTINYSTL_ZZZ.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/MYTINYSTL_ZZZ.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/MYTINYSTL_ZZZ.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MYTINYSTL_ZZZ.dir/flags.make

CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj: CMakeFiles/MYTINYSTL_ZZZ.dir/flags.make
CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj: CMakeFiles/MYTINYSTL_ZZZ.dir/includes_CXX.rsp
CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj: E:/Study/code/c++/mytinystl-zzz/src/test.cpp
CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj: CMakeFiles/MYTINYSTL_ZZZ.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\Study\code\c++\mytinystl-zzz\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj"
	E:\Software\8.1.0\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj -MF CMakeFiles\MYTINYSTL_ZZZ.dir\src\test.cpp.obj.d -o CMakeFiles\MYTINYSTL_ZZZ.dir\src\test.cpp.obj -c E:\Study\code\c++\mytinystl-zzz\src\test.cpp

CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.i"
	E:\Software\8.1.0\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Study\code\c++\mytinystl-zzz\src\test.cpp > CMakeFiles\MYTINYSTL_ZZZ.dir\src\test.cpp.i

CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.s"
	E:\Software\8.1.0\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Study\code\c++\mytinystl-zzz\src\test.cpp -o CMakeFiles\MYTINYSTL_ZZZ.dir\src\test.cpp.s

# Object files for target MYTINYSTL_ZZZ
MYTINYSTL_ZZZ_OBJECTS = \
"CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj"

# External object files for target MYTINYSTL_ZZZ
MYTINYSTL_ZZZ_EXTERNAL_OBJECTS =

MYTINYSTL_ZZZ.exe: CMakeFiles/MYTINYSTL_ZZZ.dir/src/test.cpp.obj
MYTINYSTL_ZZZ.exe: CMakeFiles/MYTINYSTL_ZZZ.dir/build.make
MYTINYSTL_ZZZ.exe: CMakeFiles/MYTINYSTL_ZZZ.dir/linkLibs.rsp
MYTINYSTL_ZZZ.exe: CMakeFiles/MYTINYSTL_ZZZ.dir/objects1
MYTINYSTL_ZZZ.exe: CMakeFiles/MYTINYSTL_ZZZ.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\Study\code\c++\mytinystl-zzz\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable MYTINYSTL_ZZZ.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\MYTINYSTL_ZZZ.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MYTINYSTL_ZZZ.dir/build: MYTINYSTL_ZZZ.exe
.PHONY : CMakeFiles/MYTINYSTL_ZZZ.dir/build

CMakeFiles/MYTINYSTL_ZZZ.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\MYTINYSTL_ZZZ.dir\cmake_clean.cmake
.PHONY : CMakeFiles/MYTINYSTL_ZZZ.dir/clean

CMakeFiles/MYTINYSTL_ZZZ.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\Study\code\c++\mytinystl-zzz E:\Study\code\c++\mytinystl-zzz E:\Study\code\c++\mytinystl-zzz\build E:\Study\code\c++\mytinystl-zzz\build E:\Study\code\c++\mytinystl-zzz\build\CMakeFiles\MYTINYSTL_ZZZ.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MYTINYSTL_ZZZ.dir/depend

