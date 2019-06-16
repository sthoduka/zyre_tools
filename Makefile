ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

LIBS=-lzyre -lczmq -lzmq -lreadline
BUILD_DIR=build
SOURCE_DIR=src

all: zyrecli zyrenode

zyrenode: $(SOURCE_DIR)/zyrenode.cpp $(BUILD_DIR)/helper_functions.o
	g++ -o $(BUILD_DIR)/$@ -Iinclude $^ $(LIBS) -std=c++11

zyrecli: $(BUILD_DIR)/zyre_tools.o $(BUILD_DIR)/helper_functions.o
	g++ -o $(BUILD_DIR)/$@ $^ $(LIBS) -std=c++11

$(BUILD_DIR)/zyre_tools.o: $(SOURCE_DIR)/zyre_tools.cpp
	g++ -o $@ -Iinclude -c $^ -std=c++11
	
$(BUILD_DIR)/helper_functions.o: $(SOURCE_DIR)/helper_functions.cpp
	g++ -o $@ -Iinclude -c $^ -std=c++11
	
clean:
	rm $(BUILD_DIR)/*

install: zyrecli
	install $(BUILD_DIR)/$^ $(PREFIX)/bin/
