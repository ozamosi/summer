include $(top_srcdir)/gtest.mk

noinst_PROGRAMS = $(TEST_PROGS)

AM_CPPFLAGS = \
	$(SUMMER_CFLAGS) \
	-I$(top_srcdir)/summer \
	-include $(CONFIG_HEADER)

AM_LDFLAGS = \
	$(SUMMER_LIBS) \
	$(top_builddir)/summer/libsummer.la

TEST_PROGS += test_web
test_web_SOURCES = test-web.c server.c
