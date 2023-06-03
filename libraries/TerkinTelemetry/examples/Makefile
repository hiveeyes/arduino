.DEFAULT_GOAL := all


# =============
# Prerequisites
# =============

include util.mk


# =============
# Configuration
# =============

export PLATFORMIO_WORKSPACE_DIR := $(PWD)/.pio
export BUILD_DIR := $(PWD)/.build


# =================
# Workhorse targets
# =================

# Main entrypoint targets.
all: setup-virtualenv run build
run: posix-run

# Run all POSIX program examples.
posix-run: http-cratedb http-kotori


# ==================================================
# Build all examples for all PlatformIO environments
# ==================================================

build: setup-virtualenv
	$(MAKE) build-generic NAME=http-cratedb
	$(MAKE) build-generic NAME=http-kotori

.PHONY: build-generic
build-generic:
	PLATFORMIO_SRC_DIR=$(NAME) $(pio) run  # --verbose


# =========================================
# Build and run individual examples (POSIX)
# =========================================

.PHONY: http-cratedb
http-cratedb:
	$(MAKE) generic NAME=$@

.PHONY: http-kotori
http-kotori:
	$(MAKE) generic NAME=$@

.PHONY: generic
generic:
	PLATFORMIO_SRC_DIR=$(NAME) $(pio) run -e native -t exec
