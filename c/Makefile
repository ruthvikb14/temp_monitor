CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Werror -O2 -g
INC    := -Iinclude -Iport/pc

CORE := src/app/app.c src/app/temp_monitor.c \
        src/drivers/eeprom_config.c src/drivers/temp_sensor.c \
        src/drivers/temp_acquisition.c src/drivers/led_indicator.c \
        src/isr.c
PORT := port/pc/mock_hal.c

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

build/demo$(EXE): $(CORE) $(PORT) port/pc/demo_main.c
	$(CC) $(CFLAGS) $(INC) $^ -o $@

build/tests$(EXE): $(CORE) $(PORT) tests/test_main.c
	$(CC) $(CFLAGS) $(INC) $^ -o $@

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
