# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.4

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master

# Include any dependencies generated for this target.
include examples/CMakeFiles/flandmark_2.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/flandmark_2.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/flandmark_2.dir/flags.make

examples/CMakeFiles/flandmark_2.dir/example2.cpp.o: examples/CMakeFiles/flandmark_2.dir/flags.make
examples/CMakeFiles/flandmark_2.dir/example2.cpp.o: examples/example2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/flandmark_2.dir/example2.cpp.o"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/flandmark_2.dir/example2.cpp.o -c /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples/example2.cpp

examples/CMakeFiles/flandmark_2.dir/example2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flandmark_2.dir/example2.cpp.i"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples/example2.cpp > CMakeFiles/flandmark_2.dir/example2.cpp.i

examples/CMakeFiles/flandmark_2.dir/example2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flandmark_2.dir/example2.cpp.s"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples/example2.cpp -o CMakeFiles/flandmark_2.dir/example2.cpp.s

examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.requires:

.PHONY : examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.requires

examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.provides: examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.requires
	$(MAKE) -f examples/CMakeFiles/flandmark_2.dir/build.make examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.provides.build
.PHONY : examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.provides

examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.provides.build: examples/CMakeFiles/flandmark_2.dir/example2.cpp.o


# Object files for target flandmark_2
flandmark_2_OBJECTS = \
"CMakeFiles/flandmark_2.dir/example2.cpp.o"

# External object files for target flandmark_2
flandmark_2_EXTERNAL_OBJECTS =

examples/flandmark_2: examples/CMakeFiles/flandmark_2.dir/example2.cpp.o
examples/flandmark_2: examples/CMakeFiles/flandmark_2.dir/build.make
examples/flandmark_2: libflandmark/libflandmark_static.a
examples/flandmark_2: /usr/local/lib/libopencv_videostab.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_ts.a
examples/flandmark_2: /usr/local/lib/libopencv_superres.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_stitching.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_contrib.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_nonfree.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_ocl.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_gpu.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_objdetect.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_photo.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_legacy.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_video.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_ml.so.2.4.13
examples/flandmark_2: /usr/local/cuda-8.0/lib64/libcublas.so
examples/flandmark_2: /usr/local/cuda-8.0/lib64/libcufft.so
examples/flandmark_2: /usr/local/lib/libopencv_calib3d.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_features2d.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_highgui.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_imgproc.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_flann.so.2.4.13
examples/flandmark_2: /usr/local/lib/libopencv_core.so.2.4.13
examples/flandmark_2: /usr/local/cuda-8.0/lib64/libcudart.so
examples/flandmark_2: /usr/local/cuda-8.0/lib64/libnppc.so
examples/flandmark_2: /usr/local/cuda-8.0/lib64/libnppi.so
examples/flandmark_2: /usr/local/cuda-8.0/lib64/libnpps.so
examples/flandmark_2: examples/CMakeFiles/flandmark_2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable flandmark_2"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/flandmark_2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/flandmark_2.dir/build: examples/flandmark_2

.PHONY : examples/CMakeFiles/flandmark_2.dir/build

examples/CMakeFiles/flandmark_2.dir/requires: examples/CMakeFiles/flandmark_2.dir/example2.cpp.o.requires

.PHONY : examples/CMakeFiles/flandmark_2.dir/requires

examples/CMakeFiles/flandmark_2.dir/clean:
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples && $(CMAKE_COMMAND) -P CMakeFiles/flandmark_2.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/flandmark_2.dir/clean

examples/CMakeFiles/flandmark_2.dir/depend:
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/examples/CMakeFiles/flandmark_2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/flandmark_2.dir/depend

