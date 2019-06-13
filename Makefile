ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

LIBS=-lzyre -lczmq -lzmq -lreadline

all: zyrecli

zyrecli: zyre_tools.cpp
	g++ -o $@ $^ $(LIBS) -std=c++11
	
install: zyrecli
	install zyrecli $(PREFIX)/bin/
