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
include libflandmark/CMakeFiles/flandmark_shared.dir/depend.make

# Include the progress variables for this target.
include libflandmark/CMakeFiles/flandmark_shared.dir/progress.make

# Include the compile flags for this target's objects.
include libflandmark/CMakeFiles/flandmark_shared.dir/flags.make

libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o: libflandmark/CMakeFiles/flandmark_shared.dir/flags.make
libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o: libflandmark/flandmark_detector.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o -c /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/flandmark_detector.cpp

libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.i"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/flandmark_detector.cpp > CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.i

libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.s"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/flandmark_detector.cpp -o CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.s

libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.requires:

.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.requires

libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.provides: libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.requires
	$(MAKE) -f libflandmark/CMakeFiles/flandmark_shared.dir/build.make libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.provides.build
.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.provides

libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.provides.build: libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o


libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o: libflandmark/CMakeFiles/flandmark_shared.dir/flags.make
libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o: libflandmark/liblbp.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/flandmark_shared.dir/liblbp.cpp.o -c /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/liblbp.cpp

libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flandmark_shared.dir/liblbp.cpp.i"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/liblbp.cpp > CMakeFiles/flandmark_shared.dir/liblbp.cpp.i

libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flandmark_shared.dir/liblbp.cpp.s"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/liblbp.cpp -o CMakeFiles/flandmark_shared.dir/liblbp.cpp.s

libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.requires:

.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.requires

libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.provides: libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.requires
	$(MAKE) -f libflandmark/CMakeFiles/flandmark_shared.dir/build.make libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.provides.build
.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.provides

libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.provides.build: libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o


# Object files for target flandmark_shared
flandmark_shared_OBJECTS = \
"CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o" \
"CMakeFiles/flandmark_shared.dir/liblbp.cpp.o"

# External object files for target flandmark_shared
flandmark_shared_EXTERNAL_OBJECTS =

libflandmark/libflandmark_shared.so: libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o
libflandmark/libflandmark_shared.so: libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o
libflandmark/libflandmark_shared.so: libflandmark/CMakeFiles/flandmark_shared.dir/build.make
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_videostab.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_ts.a
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_superres.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_stitching.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_contrib.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_nonfree.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_ocl.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_gpu.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_photo.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_objdetect.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_legacy.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_video.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_ml.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_calib3d.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_features2d.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_highgui.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_imgproc.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_flann.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/lib/libopencv_core.so.2.4.13
libflandmark/libflandmark_shared.so: /usr/local/cuda-8.0/lib64/libcudart.so
libflandmark/libflandmark_shared.so: /usr/local/cuda-8.0/lib64/libnppc.so
libflandmark/libflandmark_shared.so: /usr/local/cuda-8.0/lib64/libnppi.so
libflandmark/libflandmark_shared.so: /usr/local/cuda-8.0/lib64/libnpps.so
libflandmark/libflandmark_shared.so: /usr/local/cuda-8.0/lib64/libcublas.so
libflandmark/libflandmark_shared.so: /usr/local/cuda-8.0/lib64/libcufft.so
libflandmark/libflandmark_shared.so: libflandmark/CMakeFiles/flandmark_shared.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libflandmark_shared.so"
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/flandmark_shared.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libflandmark/CMakeFiles/flandmark_shared.dir/build: libflandmark/libflandmark_shared.so

.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/build

libflandmark/CMakeFiles/flandmark_shared.dir/requires: libflandmark/CMakeFiles/flandmark_shared.dir/flandmark_detector.cpp.o.requires
libflandmark/CMakeFiles/flandmark_shared.dir/requires: libflandmark/CMakeFiles/flandmark_shared.dir/liblbp.cpp.o.requires

.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/requires

libflandmark/CMakeFiles/flandmark_shared.dir/clean:
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark && $(CMAKE_COMMAND) -P CMakeFiles/flandmark_shared.dir/cmake_clean.cmake
.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/clean

libflandmark/CMakeFiles/flandmark_shared.dir/depend:
	cd /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark /home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark/CMakeFiles/flandmark_shared.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libflandmark/CMakeFiles/flandmark_shared.dir/depend

