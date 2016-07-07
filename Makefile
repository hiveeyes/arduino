SHELL = bash

# ==========================================
#                 environment
# ==========================================
#
# Miscellaneous tools:
# Software tests, Documentation builder, Virtual environment builder
#
docs-html: virtualenv
	export SPHINXBUILD="`pwd`/.venv/bin/sphinx-build"; cd doc; make html

virtualenv:

	@echo Setup or update virtualenv
	@test -e .venv/bin/python || `command -v virtualenv` --python=`command -v python2.7` --no-site-packages .venv

	@echo Install packages from requirements-dev.txt
	@.venv/bin/pip --quiet install --requirement requirements-dev.txt


# ==========================================
#                 releasing
# ==========================================
#
# Release targets for convenient release cutting.
#
# Synopsis::
#
#    make release bump={patch,minor,major}
#

bumpversion:
	.venv/bin/bumpversion $(bump)

push:
	git push && git push --tags

release: virtualenv bumpversion push


# ==========================================
#            ptrace.hiveeyes.org
# ==========================================

# Don't commit media assets (screenshots, etc.) to the repository.
# Instead, upload them to https://ptrace.hiveeyes.org/
ptrace_target := root@ptrace.hiveeyes.org:/var/www/ptrace.hiveeyes.org/htdocs/
ptrace_http   := https://ptrace.hiveeyes.org/
ptrace: check-ptrace-options
	$(eval prefix         := $(shell date --iso-8601))
	$(eval name           := $(shell basename '$(source)'))
	$(eval file_name      := $(prefix)_$(name))
	$(eval file_escaped   := $(shell printf %q "$(file_name)"))
	$(eval file_url       := $(shell echo -n "$(file_name)" | python -c "import sys, urllib; print urllib.quote(sys.stdin.read())"))

	$(eval upload_command := scp '$(source)' '$(ptrace_target)$(file_escaped)')
	$(eval media_url      := $(ptrace_http)$(file_url))

	@# debugging
	@#echo "name:         $(name)"
	@#echo "file_name:    $(file_name)"
	@#echo "file_escaped: $(file_escaped)"
	@#echo "command:      $(upload_command)"

	$(upload_command)

	@echo "Access URL: $(media_url)"

check-ptrace-options:
	@if test "$(source)" = ""; then \
		echo "ERROR: 'source' not set"; \
		exit 1; \
	fi

