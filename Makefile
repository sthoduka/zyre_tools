ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

LIBS=-lzyre -lczmq -lzmq -lreadline
BUILD_DIR=build
SOURCE_DIR=src

all: zyrecli

zyrecli: $(BUILD_DIR)/zyre_tools.o
	g++ -o $(BUILD_DIR)/$@ $^ $(LIBS) -std=c++11

$(BUILD_DIR)/zyre_tools.o: $(SOURCE_DIR)/zyre_tools.cpp
	g++ -o $@ -Iinclude -c $^
	
clean:
	rm $(BUILD_DIR)/*

install: zyrecli
	install $(BUILD_DIR)/$^ $(PREFIX)/bin/
