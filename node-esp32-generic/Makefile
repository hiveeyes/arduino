$(eval venvpath  := .venv)
$(eval python    := $(venvpath)/bin/python)
$(eval pip       := $(venvpath)/bin/pip)
$(eval pio       := $(venvpath)/bin/pio)


build: setup-virtualenv
	$(pio) run

upload: setup-virtualenv
	$(pio) run --target upload --upload-port=${MCU_PORT}

setup-virtualenv:
	@test -e $(python) || `command -v virtualenv` --python=python3 $(venvpath)
	$(pip) install platformio
