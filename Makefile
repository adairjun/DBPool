# # Use 'make V=1' to see the full commands
#使用安静模式可以不看gcc,g++的参数
CC := gcc 
CXX := g++
AR := ar


INCLUDE := -I/usr/local/include
LIBS := -L/usr/local/lib -L/usr/lib64/mysql -lpthread -lmysqlclient

CFLAGS := 
CPPFLAGS := -std=c++0x -O2 -g -fPIC
SHARED_LDFLAGS := -shared -fPIC

LIBCFILES := $(wildcard ./util/*.c)
LIBCPPFILES := $(wildcard ./util/*.cc ./util/*.cpp)
LIBOBJECTS := $(addsuffix .o, $(basename $(LIBCFILES)) $(basename $(LIBCPPFILES)))
LIBRARY := libdbpool.a
SHARED := libdbpool.so

CFILES := $(wildcard ./src/*.c)
CPPFILES := $(wildcard ./src/*.cc ./src/*.cpp)
OBJECTS := $(addsuffix .o, $(basename $(CFILES)) $(basename $(CPPFILES)))
TARGETS := $(basename $(OBJECTS))

ifeq ("$(origin V)", "command line")
   BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
   BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),0)
	QUIET_CC        = @echo '   ' CC $@;
	QUIET_CXX       = @echo '   ' CXX $@;
endif

all: $(LIBRARY) $(SHARED) 
	@echo "--------------------------make successful-----------------------"

check: all $(TARGETS)

$(LIBRARY): $(LIBOBJECTS)
	-rm -rf $@
	$(AR) crv $@ $(LIBOBJECTS)

$(SHARED):
	$(QUIET_CXX)$(CXX) $(SHARED_LDFLAGS) -o $@ $(LIBOBJECTS) 

$(TARGETS): $(OBJECTS)
	$(QUIET_CXX)$(CXX) -o $@ $(addsuffix .o, $@) $(LIBS) -L. -ldbpool

%.o:./util/%.c ./src/%.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c $<

%.o:./util/%.cc ./src/%.cc
	$(QUIET_CXX)$(CXX) $(CPPFLAGS) -c $<

%.o:./util/%.cpp ./src/%.cpp
	$(QUIET_CXX)$(CXX) $(CPPFLAGS) -c $<

.PHONY:clean

clean:
	-rm -f ./util/*.o ./src/*.o $(LIBRARY) $(SHARED) $(TARGETS)
	@echo "--------------------------make clean-----------------------"
