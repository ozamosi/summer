include $(top_srcdir)/gtest.mk

SUBDIRS = . tests

MARSHAL_GENERATED = summer-marshal.c summer-marshal.h

summer-marshal.h: summer-marshal.list
	( $(GLIB_GENMARSHAL) --prefix=summer_marshal $(srcdir)/summer-marshal.list --header > summer-marshal.tmp \
	&& mv summer-marshal.tmp summer-marshal.h ) \
	|| ( rm -f summer-marshal.tmp && exit 1 )

summer-marshal.c: summer-marshal.h
	( (echo '#include "summer-marshal.h"'; $(GLIB_GENMARSHAL) --prefix=summer_marshal $(srcdir)/summer-marshal.list --body) > summer-marshal.tmp \
	&& mv summer-marshal.tmp summer-marshal.c ) \
	|| ( rm -f summer-marshal.tmp && exit 1 )

BUILT_SOURCES = $(MARSHAL_GENERATED)

CLEANFILES = $(MARSHAL_GENERATED)

INCLUDES = \
	$(SUMMER_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(SOUP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(DISABLE_DEPRECATED) \
	-include $(CONFIG_HEADER)

lib_LTLIBRARIES = libsummer.la

libsummer_la_LDFLAGS = -export-dynamic
libsummer_la_LIBADD = $(LIB_LIBS)

NOINST_H_FILES =

INST_H_FILES = \
	summer.h \
	summer-download.h \
	summer-download-web.h \
	summer-feed.h \
	summer-feed-parser.h \
	summer-atom-parser.h \
	summer-rss2-parser.h \
	summer-web-backend.h

headerdir = $(includedir)/libsummer-1.0/libsummer
header_DATA = $(INST_H_FILES)

libsummer_la_SOURCES = \
	$(BUILT_SOURCES) \
	summer.c \
	summer-download.c \
	summer-download-web.c \
	summer-feed.c \
	summer-feed-parser.c \
	summer-atom-parser.c \
	summer-rss2-parser.c \
	summer-web-backend.c \
	$(NOINST_H_FILES) \
	$(INST_H_FILES)

EXTRA_DIST = soup-marshal.list
