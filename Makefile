BUILD_DIR  := build
CORES      := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
SOURCES    := $(shell find src -name '*.cpp' -o -name '*.hpp') $(shell find tests -name '*.cpp' 2>/dev/null)

LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null)
CLANG_FMT   := $(if $(shell command -v clang-format 2>/dev/null),clang-format,$(LLVM_PREFIX)/bin/clang-format)
CLANG_TIDY  := $(if $(shell command -v clang-tidy 2>/dev/null),clang-tidy,$(LLVM_PREFIX)/bin/clang-tidy)

LLVM_COV    := $(if $(shell command -v llvm-cov 2>/dev/null),llvm-cov,$(LLVM_PREFIX)/bin/llvm-cov)

.PHONY: build run test clean format format-check lint coverage

build:
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@cmake --build $(BUILD_DIR) -j$(CORES)
	@ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

run: build
	@$(BUILD_DIR)/bin/snake

test: build
	@ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	@rm -rf $(BUILD_DIR) compile_commands.json

format:
	@$(CLANG_FMT) -i $(SOURCES)
	@echo "Formatted $(words $(SOURCES)) files."

format-check:
	@$(CLANG_FMT) --dry-run --Werror $(SOURCES)
	@echo "Format check passed."

lint: build
	@$(CLANG_TIDY) -p $(BUILD_DIR) --extra-arg=-isysroot$(shell xcrun --show-sdk-path 2>/dev/null) $(filter %.cpp,$(SOURCES))

coverage:
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
	@cmake --build $(BUILD_DIR) -j$(CORES)
	@ctest --test-dir $(BUILD_DIR) --output-on-failure
	@echo "\n--- Coverage (project sources) ---"
	@$(LLVM_COV) gcov -n $(BUILD_DIR)/CMakeFiles/snake_tests.dir/src/*.gcda 2>/dev/null \
		| grep -A1 "snake/src/" | grep -E "^File|^Lines"
