$(eval venvpath  := $(PWD)/.venv)
$(eval python    := $(venvpath)/bin/python)
$(eval pip       := $(venvpath)/bin/pip)
$(eval pio       := $(venvpath)/bin/pio)

setup-virtualenv:
	@test -e $(python) || python3 -m venv $(venvpath)
	$(pip) --quiet install platformio

upload: setup-virtualenv
	$(pio) run --target upload --upload-port=${MCU_PORT}
