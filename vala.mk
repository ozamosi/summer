vapi_DATA=libsummer-1.0.vapi
vapidir=$(datadir)/vala/vapi

libsummer-1.0.vapi: 
	if test $(srcdir) != $(builddir); then \
		if test ! -d $(builddir)/bindings/vala/libsummer-1.0; then \
			mkdir -p $(builddir)/bindings/vala/libsummer-1.0; \
			cp $(srcdir)/bindings/vala/libsummer-1.0/* $(builddir)/bindings/vala/libsummer-1.0; \
			chmod u+w $(builddir)/bindings/vala/libsummer-1.0/*; \
		fi; \
	fi;
	$(VALA_GEN_INTROSPECT) libsummer-1.0 $(builddir)/bindings/vala/libsummer-1.0
	$(VAPIGEN) --library libsummer-1.0 $(builddir)/bindings/vala/libsummer-1.0/*

distclean-local:
	if test $(srcdir) != $(builddir); then \
		rm $(builddir)/bindings/vala/libsummer-1.0/*; \
		rmdir $(builddir)/bindings/vala/libsummer-1.0; \
	fi;

build_files = \
	bindings/vala/libsummer-1.0/libsummer-1.0-custom.vala \
	bindings/vala/libsummer-1.0/libsummer-1.0.deps \
	bindings/vala/libsummer-1.0/libsummer-1.0.files \
	bindings/vala/libsummer-1.0/libsummer-1.0.metadata \
	bindings/vala/libsummer-1.0/libsummer-1.0.namespace

EXTRA_DIST += $(build_files)

DISTCLEANFILES += libsummer-1.0.vapi \
	bindings/vala/libsummer-1.0/libsummer-1.0.gi
