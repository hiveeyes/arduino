$(eval venvpath  := .venv)
$(eval python    := $(venvpath)/bin/python)
$(eval pip       := $(venvpath)/bin/pip)
$(eval pio       := $(venvpath)/bin/pio)


build: setup-virtualenv
	$(pio) run

upload: setup-virtualenv
	$(pio) run --target upload --upload-port=${MCU_PORT}

setup-virtualenv:
	@echo Installing or updating prerequisites
	@test -e $(python) || python3 -m venv $(venvpath)
	$(pip) --quiet install platformio
