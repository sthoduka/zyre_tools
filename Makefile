LIBS=-lzyre -lczmq -lzmq -lreadline

all: zyre_tools

zyre_tools: zyre_tools.cpp
	g++ -o $@ $^ $(LIBS) -std=c++11
