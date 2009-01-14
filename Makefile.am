include $(top_srcdir)/gtest.mk

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

SUBDIRS = \
	libsummer \
	reference

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsummer-1.0.pc

$(pkgconfig_DATA) : config.status

EXTRA_DIST = libsummer-1.0.pc.in

DISTCLEANFILES =
