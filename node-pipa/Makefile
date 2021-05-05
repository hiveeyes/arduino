# Build all Pipa examples

# Get the full path and directory of Makefile
# https://www.systutorials.com/how-to-get-the-full-path-and-directory-of-a-makefile-itself/
mkfile_path      := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir       := $(dir $(mkfile_path))

# Configure tool shortcuts
$(eval venvpath  := $(mkfile_dir)/.venv)
$(eval python    := $(venvpath)/bin/python)
$(eval pip       := $(venvpath)/bin/pip)
$(eval pio       := $(venvpath)/bin/pio)


build: setup-virtualenv
	cd examples/legacy-gprsbee; $(pio) run # --verbose
	cd examples/pipa-espressif; $(pio) run # --verbose

upload: setup-virtualenv
	$(pio) run --target upload --upload-port=${MCU_PORT}

setup-virtualenv:
	@test -e $(python) || `command -v virtualenv` --python=python3 $(venvpath)
	$(pip) install platformio
