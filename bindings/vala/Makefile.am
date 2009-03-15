include $(top_srcdir)/gtest.mk

vapi_DATA=libsummer-1.0.vapi
vapidir=$(VAPIDIR)

libsummer-1.0.vapi:
	$(VALA_GEN_INTROSPECT) libsummer-1.0 libsummer-1.0
	$(VAPIGEN) --library libsummer-1.0 libsummer-1.0/*

CLEANFILES=libsummer-1.0.vapi
