BUILT_SOURCES = \
	$(srcdir)/libsummer/summer-marshal.c \
	$(srcdir)/libsummer/summer-marshal.h \
	$(srcdir)/libsummer/summer-enum-types.c \
	$(srcdir)/libsummer/summer-enum-types.h

$(srcdir)/libsummer/summer-marshal.h: $(srcdir)/libsummer/summer-marshal.list
	$(QUIET_GEN) ( $(GLIB_GENMARSHAL) --prefix=summer_marshal $(srcdir)/libsummer/summer-marshal.list --header > $(srcdir)/libsummer/summer-marshal.tmp \
	&& mv $(srcdir)/libsummer/summer-marshal.tmp $(srcdir)/libsummer/summer-marshal.h ) \
	|| ( rm -f $(srcdir)/libsummer/summer-marshal.tmp && exit 1 )

$(srcdir)/libsummer/summer-marshal.c: $(srcdir)/libsummer/summer-marshal.h
	$(QUIET_GEN) ( (echo '#include "summer-marshal.h"'; $(GLIB_GENMARSHAL) --prefix=summer_marshal $(srcdir)/libsummer/summer-marshal.list --body) > $(srcdir)/libsummer/summer-marshal.tmp \
	&& mv $(srcdir)/libsummer/summer-marshal.tmp $(srcdir)/libsummer/summer-marshal.c ) \
	|| ( rm -f $(srcdir)/libsummer/summer-marshal.tmp && exit 1 )

$(srcdir)/libsummer/summer-enum-types.c: $(srcdir)/libsummer/summer-enum-types.c.template
	$(QUIET_GEN) ($(GLIB_MKENUMS) \
			--template $(srcdir)/libsummer/summer-enum-types.c.template \
			$(INST_H_FILES) \
			> $(srcdir)/libsummer/summer-enum-types.c.tmp) && \
	mv $(srcdir)/libsummer/summer-enum-types.c.tmp \
		$(srcdir)/libsummer/summer-enum-types.c

$(srcdir)/libsummer/summer-enum-types.h: $(srcdir)/libsummer/summer-enum-types.h.template
	$(QUIET_GEN) ($(GLIB_MKENUMS) \
			--template $(srcdir)/libsummer/summer-enum-types.h.template \
			$(INST_H_FILES) \
			> $(srcdir)/libsummer/summer-enum-types.h.tmp) && \
	mv $(srcdir)/libsummer/summer-enum-types.h.tmp \
		$(srcdir)/libsummer/summer-enum-types.h

lib_LTLIBRARIES = libsummer.la

libsummer_la_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Summer\"

libsummer_la_CFLAGS = \
	$(GLIB_CFLAGS) \
    $(SOUP_CFLAGS) \
    $(LIBXML_CFLAGS)

libsummer_la_CXXFLAGS = \
	$(libsummer_la_CFLAGS) \
	$(LIBTORRENT_CFLAGS)

libsummer_la_LDFLAGS = \
	-export-symbols-regex '^summer_*'

libsummer_la_LIBADD = \
	$(LIB_LIBS) \
	$(SUMMER_LIBS) \
	$(GLIB_LIBS) \
	$(LIBTORRENT_LIBS) \
	$(LIBXML_LIBS) \
	$(SOUP_LIBS)

NOINST_H_FILES = \
	$(srcdir)/libsummer/summer-feed-parser.h \
	$(srcdir)/libsummer/summer-atom-parser.h \
	$(srcdir)/libsummer/summer-rss2-parser.h \
	$(srcdir)/libsummer/summer-transfer-speed.h

INST_H_FILES = \
	$(srcdir)/libsummer/summer.h \
	$(srcdir)/libsummer/summer-data-types.h \
	$(srcdir)/libsummer/summer-debug.h \
	$(srcdir)/libsummer/summer-download.h \
	$(srcdir)/libsummer/summer-download-torrent.h \
	$(srcdir)/libsummer/summer-download-youtube.h \
	$(srcdir)/libsummer/summer-download-web.h \
	$(srcdir)/libsummer/summer-feed.h \
	$(srcdir)/libsummer/summer-feed-cache.h \
	$(srcdir)/libsummer/summer-feed-cache-default.h \
	$(srcdir)/libsummer/summer-web-backend.h \
	$(srcdir)/libsummer/summer-web-backend-disk.h \
	$(srcdir)/libsummer/summer-web-backend-ram.h

headerdir = $(includedir)/libsummer-1.0/libsummer
header_DATA = $(INST_H_FILES)

libsummer_la_SOURCES = \
	$(BUILT_SOURCES) \
	$(srcdir)/libsummer/summer.c \
	$(srcdir)/libsummer/summer-atom-parser.c \
	$(srcdir)/libsummer/summer-data-types.c \
	$(srcdir)/libsummer/summer-debug.c \
	$(srcdir)/libsummer/summer-download.c \
	$(srcdir)/libsummer/summer-download-torrent.cc \
	$(srcdir)/libsummer/summer-download-youtube.c \
	$(srcdir)/libsummer/summer-download-web.c \
	$(srcdir)/libsummer/summer-feed.c \
	$(srcdir)/libsummer/summer-feed-cache.c \
	$(srcdir)/libsummer/summer-feed-cache-default.c \
	$(srcdir)/libsummer/summer-feed-parser.c \
	$(srcdir)/libsummer/summer-rss2-parser.c \
	$(srcdir)/libsummer/summer-transfer-speed.c \
	$(srcdir)/libsummer/summer-web-backend.c \
	$(srcdir)/libsummer/summer-web-backend-disk.c \
	$(srcdir)/libsummer/summer-web-backend-ram.c \
	$(NOINST_H_FILES) \
	$(INST_H_FILES)

EXTRA_DIST += libsummer/summer-marshal.list
