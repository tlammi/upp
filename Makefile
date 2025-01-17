
MESON_OPTS ?=

ifeq ($(SANITIZE_ADDRESS),1)
	MESON_OPTS := $(MESON_OPTS) -Db_sanitize=address
endif


.PHONY: compile
compile: build/.tag
	meson compile -C build

.PHONY: test
test: build/.tag
	meson test -C build

.PHONY: clean
clean:
	rm -rf build/

build/.tag:
	meson setup $(MESON_OPTS) build
	touch build/.tag
