# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/borisz/BME/nagyhf/nagyhf

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/borisz/BME/nagyhf/nagyhf/build

# Include any dependencies generated for this target.
include CMakeFiles/Borisz.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Borisz.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Borisz.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Borisz.dir/flags.make

CMakeFiles/Borisz.dir/main.cpp.o: CMakeFiles/Borisz.dir/flags.make
CMakeFiles/Borisz.dir/main.cpp.o: /home/borisz/BME/nagyhf/nagyhf/main.cpp
CMakeFiles/Borisz.dir/main.cpp.o: CMakeFiles/Borisz.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/borisz/BME/nagyhf/nagyhf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Borisz.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Borisz.dir/main.cpp.o -MF CMakeFiles/Borisz.dir/main.cpp.o.d -o CMakeFiles/Borisz.dir/main.cpp.o -c /home/borisz/BME/nagyhf/nagyhf/main.cpp

CMakeFiles/Borisz.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Borisz.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/borisz/BME/nagyhf/nagyhf/main.cpp > CMakeFiles/Borisz.dir/main.cpp.i

CMakeFiles/Borisz.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Borisz.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/borisz/BME/nagyhf/nagyhf/main.cpp -o CMakeFiles/Borisz.dir/main.cpp.s

CMakeFiles/Borisz.dir/backend.cpp.o: CMakeFiles/Borisz.dir/flags.make
CMakeFiles/Borisz.dir/backend.cpp.o: /home/borisz/BME/nagyhf/nagyhf/backend.cpp
CMakeFiles/Borisz.dir/backend.cpp.o: CMakeFiles/Borisz.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/borisz/BME/nagyhf/nagyhf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Borisz.dir/backend.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Borisz.dir/backend.cpp.o -MF CMakeFiles/Borisz.dir/backend.cpp.o.d -o CMakeFiles/Borisz.dir/backend.cpp.o -c /home/borisz/BME/nagyhf/nagyhf/backend.cpp

CMakeFiles/Borisz.dir/backend.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Borisz.dir/backend.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/borisz/BME/nagyhf/nagyhf/backend.cpp > CMakeFiles/Borisz.dir/backend.cpp.i

CMakeFiles/Borisz.dir/backend.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Borisz.dir/backend.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/borisz/BME/nagyhf/nagyhf/backend.cpp -o CMakeFiles/Borisz.dir/backend.cpp.s

# Object files for target Borisz
Borisz_OBJECTS = \
"CMakeFiles/Borisz.dir/main.cpp.o" \
"CMakeFiles/Borisz.dir/backend.cpp.o"

# External object files for target Borisz
Borisz_EXTERNAL_OBJECTS =

Borisz: CMakeFiles/Borisz.dir/main.cpp.o
Borisz: CMakeFiles/Borisz.dir/backend.cpp.o
Borisz: CMakeFiles/Borisz.dir/build.make
Borisz: /usr/lib/x86_64-linux-gnu/libcurl.so
Borisz: CMakeFiles/Borisz.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/borisz/BME/nagyhf/nagyhf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Borisz"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Borisz.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Borisz.dir/build: Borisz
.PHONY : CMakeFiles/Borisz.dir/build

CMakeFiles/Borisz.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Borisz.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Borisz.dir/clean

CMakeFiles/Borisz.dir/depend:
	cd /home/borisz/BME/nagyhf/nagyhf/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/borisz/BME/nagyhf/nagyhf /home/borisz/BME/nagyhf/nagyhf /home/borisz/BME/nagyhf/nagyhf/build /home/borisz/BME/nagyhf/nagyhf/build /home/borisz/BME/nagyhf/nagyhf/build/CMakeFiles/Borisz.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Borisz.dir/depend

