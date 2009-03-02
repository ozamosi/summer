include $(top_srcdir)/gtest.mk

INCLUDES = \
	-I$(top_srcdir)/libsummer \
	$(SUMMER_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(PYGOBJECT_CFLAGS)


python_LTLIBRARIES = summer.la


summer_la_SOURCES = \
	summer.c \
	summermodule.c

summer_la_LDFLAGS = \
	-module -avoid-version \
	$(top_builddir)/libsummer/libsummer.la \
	$(PYGOBJECT_LIBS) \
	$(GLIB_LIBS) \
	$(SOUP_LIBS)

summer.c: summer.defs summer.override
	( cd $(srcdir) && $(PYGOBJECT_CODEGEN) \
		--prefix summer \
		--override $*.override \
		summer.defs) > $@

BUILT_SOURCES = \
	summer.c

EXTRA_DIST = \
	summer.defs \
	summer.override

CLEANFILES = $(BUILT_SOURCES)
