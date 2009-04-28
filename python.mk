python_LTLIBRARIES = libpysummer.la

libpysummer_la_SOURCES = \
	$(srcdir)/bindings/python/pysummer.c \
	$(srcdir)/bindings/python/summermodule.c

libpysummer_la_LDFLAGS = \
	-module -avoid-version

libpysummer_la_LIBADD = \
	$(builddir)/libsummer.la \
	$(PYTHON_LIBS) \
	$(PYGOBJECT_LIBS) \
	$(GLIB_LIBS)

libpysummer_la_CFLAGS = \
	$(PYTHON_CFLAGS) \
	$(PYGOBJECT_CFLAGS) \
	-I$(srcdir)/libsummer

libpysummer_la_DEPENDENCIES = $(builddir)/libsummer.la

$(srcdir)/bindings/python/pysummer.c: $(srcdir)/bindings/python/summer.defs $(srcdir)/bindings/python/pysummer.override
	( cd $(builddir) && $(PYGOBJECT_CODEGEN) \
		--prefix summer \
		--override $*.override \
		$(srcdir)/bindings/python/summer.defs) > $@

built_sources = \
	$(srcdir)/bindings/python/pysummer.c

EXTRA_DIST += \
	$(srcdir)/bindings/python/summer.defs \
	$(srcdir)/bindings/python/pysummer.override

CLEANFILES += $(built_sources)
