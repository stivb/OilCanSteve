#!/usr/bin/make -f
PARENTDIR = $(shell basename $(dir $(abspath $(dir $$PWD))))

# this part (up until "else") is to set compilation just for locally (not for mod)
ifeq ($(PARENTDIR),lv2course)

$(info    THE VAR  is $(PARENTDIR))
CC=gcc
CFLAGS=-fvisibility=hidden -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2`
FOLDER=$(shell basename $(CURDIR))

compile: $(FOLDER).c
	$(CC) $(CFLAGS) $(FOLDER).c -o $(FOLDER).so

else
# this part compiles for mod
include Makefile.mk

# ---------replace _LC_LIB_NAME_with the actual dev folder name -----------------

STEM = audiorouter
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

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)

	install -m 644 *.so  $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)/
	install -m 644 *.ttl $(DESTDIR)$(PREFIX)/lib/lv2/$(BUNDLEDIR)/

# --------------------------------------------------------------
endif



