COMMON=../../common
include $(COMMON)/Makefile.config

CXX = g++
CXX_FLAGS  = -O3 -std=c++11 \
						 $(INCLUDE_DIRS) \
						 -fpermissive \
						 -I$(COMMON)/include \
						 -I$(CAFFE)/include \
						 -I$(CUDA)/include
ifeq ($(MPI), 1)
CXX = mpic++
CXX_FLAGS += -DMPIVER
endif

ifeq ($(CPU_ONLY), 1)
	CXX_FLAGS  += -DCPU_ONLY
endif

LINK_FLAGS = $(LIBRARY_DIRS) \
						 -lopencv_core \
						 -lopencv_highgui \
						 -lopencv_imgproc \
						 -lopencv_objdetect \
						 $(COMMON)/libtonic.a \
						 libflandmark_static.a \
						 -lboost_program_options \
						 -lboost_filesystem \
						 -lboost_system \
						 -lglog \
						 -lrt \
						 -lpthread \
						 $(CAFFE)/lib/libcaffe.so

SRC=src

# File names
EXEC = tonic-img
ifeq ($(MPI), 1)
EXEC = tonic-img-mpi
endif
SOURCES = $(wildcard $(SRC)/*.cpp) $(COMMON)/src/tonic.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) -o $(EXEC) $(LINK_FLAGS)


# To obtain object files
%.o: %.cpp Makefile
	$(CXX) -c $(CXX_FLAGS) $(EXTRA_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f tomic-img tonic-img-mpi $(SRC)/*.o $(COMMON)/src/*.o
