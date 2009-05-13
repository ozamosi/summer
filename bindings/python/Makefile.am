INCLUDES = \
	-I$(top_srcdir)/libsummer \
	$(DISABLE_DEPRECATED) \
	-include $(CONFIG_HEADER) \
	-Wall -Wextra \
	-Wno-missing-field-initializers -Wno-unused-parameter \
	-Werror -O0 -g

python_LTLIBRARIES = summer.la

summer_la_SOURCES = \
	summer.c \
	summermodule.c

summer_la_LDFLAGS = \
	-module -avoid-version

summer_la_LIBADD = \
	$(top_builddir)/libsummer.la \
	$(PYTHON_LIBS) \
	$(PYGOBJECT_LIBS) \
	$(GLIB_LIBS)

summer_la_CFLAGS = \
	$(CFLAGS) \
	$(SUMMER_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(PYGOBJECT_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(srcdir)/libsummer

summer_la_DEPENDENCIES = $(top_builddir)/libsummer.la

summer.c: summer.defs summer.override
	( cd $(builddir) && $(PYGOBJECT_CODEGEN) \
		--prefix summer \
		--override $*.override \
		summer.defs) > $@

built_sources = \
	summer.c

EXTRA_DIST = \
	summer.defs \
	summer.override

CLEANFILES = $(built_sources)

TESTS_ENVIRONMENT = PYTHONPATH=$(builddir)/.libs:$PYTHONPATH $(PYTHON)
TESTS = tests/runner.py

test:
