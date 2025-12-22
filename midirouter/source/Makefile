#!/usr/bin/make -f

include Makefile.mk

# ---------replace _LC_LIB_NAME_with the actual dev folder name -----------------

STEM = midirouter
PREFIX  ?= /usr/local
DESTDIR ?=
BUNDLEDIR = $(STEM).lv2

# --------------------------------------------------------------
# Default target is to build all plugins

all: build
build: output

# --------------------------------------------------------------
# _LC_LIB_NAME_ build rules

output: $(STEM).so

$(STEM).so: $(STEM).c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@


# --------------------------------------------------------------

clean:
	rm -f $(STEM)$(LIB_EXT) 

# --------------------------------------------------------------

# Install target: Compiles the project and installs LV2 plugin files
# Creates the destination directory structure for LV2 plugins
# Copies compiled shared objects (.so files) with read permissions (644)
# Copies LV2 Turtle definition files (.ttl) with read permissions (644)
# to the LV2 plugin bundle directory at $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)
# This is typically used for installing audio plugin bundles on Linux systems
install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)
	install -m 644 *.so  $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)/
	install -m 644 *.ttl $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)/

# --------------------------------------------------------------




