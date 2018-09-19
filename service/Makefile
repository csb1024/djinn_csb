ARCH   := $(shell getconf LONG_BIT)

ifeq (${ARCH},$(filter ${ARCH},32 64))
   NVML_LIB := /usr/src/gdk/nvml/lib
else
   $(error Unknown architecture!)
endif


COMMON=../common
include $(COMMON)/Makefile.config

CXX = g++

CXX_FLAGS  = -g -std=c++11 -ggdb \
						 $(INCLUDE_DIRS) \
						 -fpermissive \
						 -I$(COMMON)/include \
						 -I$(CAFFE)/include \
						 -I$(CUDA)/include \

CXX_FLAGS += -I /usr/include/nvidia/gdk/ -rdynamic

# comment the following to diable debugging messages 
CXX_FLAGS += -DDEBUG=1
ifeq ($(B2B), 1)
	CXX_FLAGS += -DB2B
endif
ifeq ($(CON),1)
	CXX_FLAGS += -DCONCURRENT
endif

						

ifeq ($(CPU_ONLY), 1)
	CXX_FLAGS  += -DCPU_ONLY
endif

LINK_FLAGS = $(LIBRARY_DIRS) \
						 -lpthread \
						 -ldl \
						 -lrt \
						 -lboost_program_options \
						 -lboost_filesystem \
						 -lboost_system \
						 -lglog \
						 $(CAFFE)/lib/libcaffe.so \
						 $(COMMON)/libtonic.a

LINK_FLAGS += -lnvidia-ml -L $(NVML_LIB)



# File names
EXEC = djinn
#MED = mediator

SRC=src
SOURCES = $(wildcard $(SRC)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) -o $(EXEC) $(LINK_FLAGS)

# To obtain object files
%.o: %.cpp Makefile
	$(CXX) -c $(CXX_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(SRC)/*.o $(COMMON)/src/*.o
