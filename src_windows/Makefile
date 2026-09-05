BUILD_DIR := build
CONFIG := Debug

.PHONY: configure build run clean rebuild

configure:
	cmake -S . -B $(BUILD_DIR)

build:
	cmake --build $(BUILD_DIR) --config $(CONFIG)

run: build
	.\$(BUILD_DIR)\$(CONFIG)\pcVitals.exe

clean:
	cmake --build $(BUILD_DIR) --target clean --config $(CONFIG)

rebuild:
	cmake --build $(BUILD_DIR) --config $(CONFIG) --clean-first
