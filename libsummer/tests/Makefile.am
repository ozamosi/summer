include $(top_srcdir)/gtest.mk

noinst_PROGRAMS = $(TEST_PROGS)

INCLUDES = \
	$(SUMMER_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(SOUP_CFLAGS) \
	-I$(top_srcdir)/libsummer \
	-include $(CONFIG_HEADER)

AM_LDFLAGS = \
	$(SUMMER_LIBS) \
	$(GLIB_LIBS) \
	$(SOUP_LIBS) \
	$(top_builddir)/libsummer/libsummer.la

check-local: test

TEST_PROGS += test_web_backend
test_web_backend_SOURCES = test-web-backend.c server.c server.h
