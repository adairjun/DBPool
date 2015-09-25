# # Use 'make V=1' to see the full commands
#使用安静模式可以不看gcc,g++的参数
CC := gcc 
CXX := g++
AR := ar

LIBRARY := libdbpool.a
SHARED := libdbpool.so

INCLUDE := -I/usr/local/include
LIBS := -L/usr/local/lib -L/usr/lib64/mysql -lpthread -lmysqlclient -lgtest

CFLAGS := 
CPPFLAGS := -std=c++0x -O2 -g -fPIC
SHARED_LDFLAGS := -shared -fPIC -Wl,-soname,${SHARED}

LIBCFILES := $(wildcard ./util/*.c)
LIBCPPFILES := $(wildcard ./util/*.cc ./util/*.cpp)
LIBOBJECTS := $(addsuffix .o, $(basename $(LIBCFILES)) $(basename $(LIBCPPFILES)))

CFILES := $(wildcard ./gtest/*.c)
CPPFILES := $(wildcard ./gtest/*.cc ./gtest/*.cpp)
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

%.o:./util/%.c ./gtest/%.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c $<

%.o:./util/%.cc ./gtest/%.cc
	$(QUIET_CXX)$(CXX) $(CPPFLAGS) -c $<

%.o:./util/%.cpp ./gtest/%.cpp
	$(QUIET_CXX)$(CXX) $(CPPFLAGS) -c $<

.PHONY:clean

clean:
	-rm -f ./util/*.o ./gtest/*.o $(LIBRARY) $(SHARED) $(TARGETS)
	@echo "--------------------------make clean-----------------------"
