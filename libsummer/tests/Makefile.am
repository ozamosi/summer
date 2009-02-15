include $(top_srcdir)/gtest.mk

noinst_PROGRAMS = $(TEST_PROGS)

INCLUDES = \
	$(GLIB_CFLAGS) \
	$(SOUP_CFLAGS) \
	-I$(top_srcdir)/libsummer \
	-include $(CONFIG_HEADER)

AM_LDFLAGS = \
	$(GLIB_LIBS) \
	$(SOUP_LIBS) \
	$(top_builddir)/libsummer/libsummer.la

check-local: test

TEST_PROGS += \
	test_web_backend \
	test_download \
	test_download_torrent \
	test_download_youtube \
	test_download_web \
	test_atom_feed \
	test_rss2_feed \
	test_feed \
	test_feed_cache

test_web_backend_SOURCES = test-web-backend.c server.c server.h
test_download_SOURCES = test-download.c server.c server.h
test_download_torrent_SOURCES = test-download-torrent.c
test_download_youtube_SOURCES = test-download-youtube.c server.c server.h
test_download_web_SOURCES = test-download-web.c server.c server.h
test_atom_feed_SOURCES = test-atom-feed.c
test_rss2_feed_SOURCES = test-rss2-feed.c
test_feed_SOURCES = test-feed.c server.c server.h
test_feed_cache_SOURCES = test-feed-cache.c

EXTRA_DIST = \
	atom_feed \
	atom_feed_nolink \
	epicfu \
	feed \
	rss2sample.xml
