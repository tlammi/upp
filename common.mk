ROOT_DIR:=$(shell dirname $(realpath $(word 2, $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build

SUBPROJECT ?= ""

.PHONY: compile
compile: $(BUILD_DIR)
	meson compile -C $(BUILD_DIR)

.PHONY: test
test: $(BUILD_DIR)
	meson test -C $(BUILD_DIR) --suite unit --suite $(SUBPROJECT)

.PHONY: test-smoke
test-smoke: $(BUILD_DIR)
	meson test -C $(BUILD_DIR) --suite smoke --suite $(SUBPROJECT)

.PHONY: lint-all
lint-all: $(BUILD_DIR)
	run-clang-tidy -p $(BUILD_DIR) -warnings-as-errors='*' -q

.PHONY: format-check
format-check: $(BUILD_DIR)
	./script/run-clang-format -b $(BUILD_DIR)

.PHONY: format-check-meson
format-check-meson:
	meson format --check-diff

.PHONY: format-fix
format-fix: $(BUILD_DIR)
	./script/run-clang-format -b $(BUILD_DIR) --fix

.PHONY: format-fix-meson
format-fix-meson:
	meson format -i


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR): $(BUILD_DIR)/.tag

$(BUILD_DIR)/.tag:
	[ -d $(BUILD_DIR) ] && [ ! -f $(BUILD_DIR)/.tag ] && rm -rf $(BUILD_DIR) || true
	meson setup -Db_lundef=false -Db_sanitize=address,undefined $(BUILD_DIR) $(ROOT_DIR)
	touch $(BUILD_DIR)/.tag
