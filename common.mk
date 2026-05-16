ROOT_DIR:=$(shell dirname $(realpath $(word 2, $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build

SUBPROJECT ?= ""

.PHONY: compile
compile: $(BUILD_DIR)
	meson compile -C $(BUILD_DIR)

.PHONY: test
test: $(BUILD_DIR)
	meson test -C $(BUILD_DIR) --suite unit --suite $(SUBPROJECT)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR): $(BUILD_DIR)/.tag

$(BUILD_DIR)/.tag:
	[ -d $(BUILD_DIR) ] && [ ! -f $(BUILD_DIR)/.tag ] && rm -rf $(BUILD_DIR) || true
	meson setup -Db_sanitize=address,undefined $(BUILD_DIR) $(ROOT_DIR)
	touch $(BUILD_DIR)/.tag
