include $(top_srcdir)/gtest.mk

noinst_PROGRAMS = $(TEST_PROGS)

AM_CPPFLAGS = \
	$(SUMMER_CFLAGS) \
	-I$(top_srcdir)/libsummer \
	-include $(CONFIG_HEADER)

AM_LDFLAGS = \
	$(SUMMER_LIBS) \
	$(top_builddir)/libsummer/libsummer.la

TEST_PROGS += test-init

TEST_PROGS += test-basic-web-operations

EXTRA_DIST = epicfu
