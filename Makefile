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
		export SPHINXBUILD="`pwd`/.venv27/bin/sphinx-build"; cd doc; make html

virtualenv:
		test -e .venv27/bin/python || `command -v virtualenv` --python=`command -v python2.7` --no-site-packages --verbose .venv27
		.venv27/bin/pip install --requirement requirements-dev.txt


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
	bumpversion $(bump)

push:
	git push && git push --tags

release: virtualenv bumpversion push 

