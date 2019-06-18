ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

LIBS=-lzyre -lczmq -lzmq -lreadline
BUILD_DIR=build
SOURCE_DIR=src

all: zyrecli zyrenode zyregroup

zyrenode: $(SOURCE_DIR)/zyrenode.cpp $(BUILD_DIR)/helper_functions.o
	g++ -o $(BUILD_DIR)/$@ -Iinclude $^ $(LIBS) -std=c++11

zyregroup: $(SOURCE_DIR)/zyregroup.cpp $(BUILD_DIR)/helper_functions.o
	g++ -o $(BUILD_DIR)/$@ -Iinclude $^ $(LIBS) -std=c++11

zyrecli: $(SOURCE_DIR)/zyre_tools.cpp $(BUILD_DIR)/helper_functions.o
	g++ -o $(BUILD_DIR)/$@ -Iinclude $^ $(LIBS) -std=c++11

$(BUILD_DIR)/helper_functions.o: $(SOURCE_DIR)/helper_functions.cpp
	g++ -o $@ -Iinclude -c $^ -std=c++11
	
clean:
	rm $(BUILD_DIR)/*

install: zyrecli zyrenode zyregroup
	install $(BUILD_DIR)/zyrecli $(PREFIX)/bin/
	install $(BUILD_DIR)/zyrenode $(PREFIX)/bin/
	install $(BUILD_DIR)/zyregroup $(PREFIX)/bin/
