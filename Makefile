
MESON_OPTS ?=

ifeq ($(SANITIZE_ADDRESS),1)
	MESON_OPTS := $(MESON_OPTS) -Db_sanitize=address
endif

ifeq ($(COVERAGE),1)
	MESON_OPTS := $(MESON_OPTS) -Db_coverage=true
endif


.PHONY: compile
compile: build/.tag
	meson compile -C build

TEST_NAME ?= *
.PHONY: test
test: build/.tag
	meson test -C build "$(TEST_NAME)"

.PHONY: coverage-html
coverage-html: build/.tag
	ninja -C build coverage-html

.PHONY: clean
clean:
	rm -rf build/

build/.tag:
	meson setup $(MESON_OPTS) build
	touch build/.tag
