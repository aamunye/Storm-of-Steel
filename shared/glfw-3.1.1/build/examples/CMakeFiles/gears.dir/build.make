# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build

# Include any dependencies generated for this target.
include examples/CMakeFiles/gears.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/gears.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/gears.dir/flags.make

examples/CMakeFiles/gears.dir/gears.c.o: examples/CMakeFiles/gears.dir/flags.make
examples/CMakeFiles/gears.dir/gears.c.o: ../examples/gears.c
	$(CMAKE_COMMAND) -E cmake_progress_report /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object examples/CMakeFiles/gears.dir/gears.c.o"
	cd /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gears.dir/gears.c.o   -c /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/examples/gears.c

examples/CMakeFiles/gears.dir/gears.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gears.dir/gears.c.i"
	cd /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/examples/gears.c > CMakeFiles/gears.dir/gears.c.i

examples/CMakeFiles/gears.dir/gears.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gears.dir/gears.c.s"
	cd /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/examples/gears.c -o CMakeFiles/gears.dir/gears.c.s

examples/CMakeFiles/gears.dir/gears.c.o.requires:
.PHONY : examples/CMakeFiles/gears.dir/gears.c.o.requires

examples/CMakeFiles/gears.dir/gears.c.o.provides: examples/CMakeFiles/gears.dir/gears.c.o.requires
	$(MAKE) -f examples/CMakeFiles/gears.dir/build.make examples/CMakeFiles/gears.dir/gears.c.o.provides.build
.PHONY : examples/CMakeFiles/gears.dir/gears.c.o.provides

examples/CMakeFiles/gears.dir/gears.c.o.provides.build: examples/CMakeFiles/gears.dir/gears.c.o

# Object files for target gears
gears_OBJECTS = \
"CMakeFiles/gears.dir/gears.c.o"

# External object files for target gears
gears_EXTERNAL_OBJECTS =

examples/gears: examples/CMakeFiles/gears.dir/gears.c.o
examples/gears: examples/CMakeFiles/gears.dir/build.make
examples/gears: src/libglfw3.a
examples/gears: /usr/lib/x86_64-linux-gnu/libGLU.so
examples/gears: /usr/lib/x86_64-linux-gnu/libX11.so
examples/gears: /usr/lib/x86_64-linux-gnu/libXrandr.so
examples/gears: /usr/lib/x86_64-linux-gnu/libXinerama.so
examples/gears: /usr/lib/x86_64-linux-gnu/libXi.so
examples/gears: /usr/lib/x86_64-linux-gnu/libXxf86vm.so
examples/gears: /usr/lib/x86_64-linux-gnu/librt.so
examples/gears: /usr/lib/x86_64-linux-gnu/libm.so
examples/gears: /usr/lib/x86_64-linux-gnu/libXcursor.so
examples/gears: /usr/lib/x86_64-linux-gnu/libGL.so
examples/gears: examples/CMakeFiles/gears.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable gears"
	cd /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gears.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/gears.dir/build: examples/gears
.PHONY : examples/CMakeFiles/gears.dir/build

examples/CMakeFiles/gears.dir/requires: examples/CMakeFiles/gears.dir/gears.c.o.requires
.PHONY : examples/CMakeFiles/gears.dir/requires

examples/CMakeFiles/gears.dir/clean:
	cd /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/gears.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/gears.dir/clean

examples/CMakeFiles/gears.dir/depend:
	cd /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1 /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/examples /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples /u1/aamunye/cs488/cs488-a0a1/shared/glfw-3.1.1/build/examples/CMakeFiles/gears.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/gears.dir/depend

