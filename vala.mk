vapi_DATA=bindings/vala/libsummer-1.0.vapi
vapidir=$(datadir)/vala/vapi

regen-vala:
	t=`pwd` \
	cd $(srcdir)/bindings/vala \
	$(VALA_GEN_INTROSPECT) libsummer-1.0 libsummer-1.0 \
	$(VAPIGEN) --library libsummer-1.0 libsummer-1.0/* \
	cd $t

build_files = \
	bindings/vala/libsummer-1.0/libsummer-1.0-custom.vala \
	bindings/vala/libsummer-1.0/libsummer-1.0.deps \
	bindings/vala/libsummer-1.0/libsummer-1.0.files \
	bindings/vala/libsummer-1.0/libsummer-1.0.metadata \
	bindings/vala/libsummer-1.0/libsummer-1.0.namespace \
	bindings/vala/libsummer-1.0/libsummer-1.0.gi \
	bindings/vala/libsummer-1.0.vapi

EXTRA_DIST += $(build_files)
