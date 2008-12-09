include $(top_srcdir)/gtest.mk

SUBDIRS = \
	libsummer \
	tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsummer-1.0.pc

$(pkgconfig_DATA) : config.status

EXTRA_DIST = libsummer-1.0.pc.in

DISTCLEANFILES =
