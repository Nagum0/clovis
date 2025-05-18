BUILD_DIR := build

TAB_CC := $(BUILD_DIR)/lang.tab.cpp
TAB_HH := $(BUILD_DIR)/lang.tab.hh
LEX_CC := $(BUILD_DIR)/lexer.cpp
POSITION_HH := $(BUILD_DIR)/position.hh
LOCATION_HH := $(BUILD_DIR)/location.hh
IO_C := io.c


SRC := main.cpp $(wildcard src/*.cpp)
OUT := lang

all: $(OUT)

$(OUT): $(LEX_CC) $(TAB_CC) $(TAB_HH) $(POSITION_HH) $(LOCATION_HH) $(SRC)
	g++ -I$(BUILD_DIR) $(LEX_CC) $(TAB_CC) $(SRC) $(IO_C) -o $(OUT)

$(LEX_CC): lang.l | $(BUILD_DIR)
	flex -o $(LEX_CC) lang.l

$(TAB_CC) $(TAB_HH) $(LOCATION_HH) $(POSITION_HH): lang.y | $(BUILD_DIR)
	bison -d lang.y
	mv lang.tab.cc $(TAB_CC)
	mv lang.tab.hh $(TAB_HH)
	mv location.hh $(LOCATION_HH)
	mv position.hh $(POSITION_HH)
	mv stack.hh $(BUILD_DIR)/stack.hh

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUT)
