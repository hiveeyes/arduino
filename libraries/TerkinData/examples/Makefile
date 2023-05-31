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
all: run build
run: posix-run

# Run all POSIX program examples.
posix-run: csv json urlencoded


# ==================================================
# Build all examples for all PlatformIO environments
# ==================================================

build: setup-virtualenv
	PLATFORMIO_SRC_DIR=csv $(pio) run  # --verbose
	PLATFORMIO_SRC_DIR=json $(pio) run  # --verbose
	PLATFORMIO_SRC_DIR=urlencoded $(pio) run  # --verbose


# =========================================
# Build and run individual examples (POSIX)
# =========================================

.PHONY: csv
csv:
	PLATFORMIO_SRC_DIR=csv pio run -e native -t exec

.PHONY: json
json:
	PLATFORMIO_SRC_DIR=json pio run -e native -t exec

.PHONY: urlencoded
urlencoded:
	PLATFORMIO_SRC_DIR=urlencoded pio run -e native -t exec
