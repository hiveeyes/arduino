# =============
# Configuration
# =============

$(eval venv         := .venv)
$(eval bumpversion  := $(venv)/bin/bumpversion)
$(eval pip          := $(venv)/bin/pip)
$(eval pytest       := $(venv)/bin/pytest)
$(eval python       := $(venv)/bin/python)
$(eval sphinx       := $(venv)/bin/sphinx-build)
$(eval sphinx-autobuild := $(venv)/bin/sphinx-autobuild)
$(eval twine        := $(venv)/bin/twine)


# =============
# Miscellaneous
# =============
all:
	@echo "Nothing to do."

clean:
	@echo "Nothing to do."


# =====
# Setup
# =====

# Setup Python virtualenv
setup-virtualenv:
	@test -e $(python) || python3 -m venv $(venv)

# Install requirements for development.
virtualenv-dev: setup-virtualenv
	@test -e $(pytest) || $(pip) install --upgrade --requirement=requirements-test.txt

# Install requirements for releasing.
install-releasetools: setup-virtualenv
	@$(pip) install --quiet --requirement requirements-release.txt --upgrade


# =======
# Release
# =======

# Release this piece of software.
# Uses the fine ``bumpversion`` utility.
#
# Synopsis::
#
#    make release bump={patch,minor,major}

release: bumpversion push build pypi-upload

bumpversion: install-releasetools
	@$(bumpversion) $(bump)

push:
	git push && git push --tags

build:
	@$(python) -m build

pypi-upload: install-releasetools
	twine upload --verbose --skip-existing dist/*{.tar.gz,.whl}


# ==============
# Software tests
# ==============
# Invoke pytest.

.PHONY: test
pytest: setup-package
	$(pytest)

test: pytest
