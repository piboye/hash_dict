PROJECT_ROOT_DIR=/data/
CXXFLAGS+=-ggdb -Wall -MMD


GMOCK_PATH=$(PROJECT_ROOT_DIR)/gmock-1.6.0/
INC=-I../incl -I$(GMOCK_PATH) -I../
LIB=-ldl  -lpthread

ALL_TEST=$(patsubst %_unittest.cpp, %_unittest, $(wildcard *_unittest.cpp))

GMOCK_OBJ=gmock-gtest-all.o gmock_main.o

STUB_OBJ=stdc.stub.o

ALL_TEST_OBJ= $(patsubst %_unittest.cpp, %_unittest.o, $(wildcard *_unittest.cpp)) $(GMOCK_OBJ)

ALL=$(ALL_TEST) all_unittest



.PHONY: clean all test 

all: $(ALL)


test: all
	./all.gtest

include $(wildcard *.d)

%_unittest: %_unittest.o $(GMOCK_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^  $(LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INC)

gmock-gtest-all.o: $(GMOCK_PATH)/gmock-gtest-all.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INC)

gmock_main.o: $(GMOCK_PATH)/gmock_main.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INC)

all_unittest: $(ALL_TEST_OBJ)    
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC) $(LIB)
	
clean:
	rm -f *.o $(ALL) *.d

