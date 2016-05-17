# ==========================================
#                 utilities
# ==========================================
#
# ------------------------------------------
#                ino macros
# ------------------------------------------
#
# build sketches, flash binaries and show serial output
ino := ../.venv27/bin/ino

run: flash
	$(ino) serial

compile:
	$(ino) build

flash: compile
	$(ino) upload
