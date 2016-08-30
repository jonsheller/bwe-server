OBJ_DIR=obj
BIN_DIR=bin
SRC_DIR=src
CC=g++
CFLAGS=-I./inc
LINK=g++
LIBS=-lm
LD_FLAGS=

SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES=$(addprefix obj/,$(notdir $(SRC_FILES:.cpp=.o)))

$(BIN_DIR)/bwe-server: $(OBJ_FILES)
	$(LINK) -o $@ $^ $(LD_FLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf ./$(OBJ_DIR)
