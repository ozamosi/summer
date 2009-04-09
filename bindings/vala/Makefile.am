include $(top_srcdir)/gtest.mk

vapi_DATA=libsummer-1.0.vapi
vapidir=$(datadir)/vala/vapi

libsummer-1.0.vapi: libsummer-1.0.stamp
	@true

libsummer-1.0.stamp:
	if test $(srcdir) != $(builddir); then \
		if test ! -d $(builddir)/libsummer-1.0; then \
			mkdir $(builddir)/libsummer-1.0; \
			cp $(srcdir)/libsummer-1.0/* $(builddir)/libsummer-1.0; \
			chmod u+w $(builddir)/libsummer-1.0/*; \
		fi; \
	fi;
	$(VALA_GEN_INTROSPECT) libsummer-1.0 $(builddir)/libsummer-1.0
	$(VAPIGEN) --library libsummer-1.0 $(builddir)/libsummer-1.0/*

distclean-local:
	if test $(srcdir) != $(builddir); then \
		rm $(builddir)/libsummer-1.0/*; \
	fi;

build_files = \
	libsummer-1.0/libsummer-1.0-custom.vala \
	libsummer-1.0/libsummer-1.0.deps \
	libsummer-1.0/libsummer-1.0.files \
	libsummer-1.0/libsummer-1.0.metadata \
	libsummer-1.0/libsummer-1.0.namespace

EXTRA_DIST = $(build_files)

DISTCLEANFILES = libsummer-1.0.vapi \
	libsummer-1.0/libsummer-1.0.gi \
	libsummer-1.0.stamp
