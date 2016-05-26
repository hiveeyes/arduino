# ==========================================
#                 utilities
# ==========================================
#
# ------------------------------------------
#               documentation
# ------------------------------------------
#

# build sphinx documentation

docs-html: virtualenv
	export SPHINXBUILD="`pwd`/.venv/bin/sphinx-build"; cd doc; make html

virtualenv:

	@echo Setup or update virtualenv
	@test -e .venv/bin/python || `command -v virtualenv` --python=`command -v python2.7` --no-site-packages .venv

	@echo Install packages from requirements-dev.txt
	@.venv/bin/pip --quiet install --requirement requirements-dev.txt


# ------------------------------------------
#                 releasing
# ------------------------------------------
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

