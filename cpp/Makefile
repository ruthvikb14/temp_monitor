CXXFLAGS ?= -std=c++17 -fno-exceptions -fno-rtti \
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wold-style-cast \
            -Wnon-virtual-dtor -Woverloaded-virtual -Werror -O2 -g
INC := -Iinclude -Iport/pc

CORE := src/app/Application.cpp src/app/TempMonitor.cpp \
        src/drivers/ConfigReader.cpp src/drivers/TemperatureSensors.cpp \
        src/drivers/TempAcquisition.cpp src/drivers/LedIndicator.cpp \
        src/Isr.cpp

ifeq ($(origin CC),default)
CC := gcc
endif
ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif
_MKDIR_OUT := $(shell mkdir build 2>&1)

.PHONY: all run test clean

all: build/demo$(EXE) build/tests$(EXE)

build/demo$(EXE): $(CORE) port/pc/demo_main.cpp
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

build/tests$(EXE): $(CORE) tests/test_main.cpp
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

run: build/demo$(EXE)
	build/demo$(EXE)

test: build/tests$(EXE)
	build/tests$(EXE)

clean:
ifeq ($(OS),Windows_NT)
	-rmdir /s /q build 2>NUL || rm -rf build
else
	rm -rf build
endif
