noinst_PROGRAMS = $(TEST_PROGS)

TESTS_INCLUDES =  \
	$(SOUP_CFLAGS) \
	-DBASEFILEPATH=\"$(top_srcdir)/libsummer/tests\" \
	-I$(srcdir)

TESTSLDADD = \
	$(top_builddir)/libsummer.la

SERVER_FILES = libsummer/tests/server.c libsummer/tests/server.h

DEPENDENCIES = $(top_builddir)/libsummer.la

TEST_PROGS += \
	test_web_backend \
	test_debug \
	test_download \
	test_download_torrent \
	test_download_youtube \
	test_download_web \
	test_atom_feed \
	test_rss2_feed \
	test_feed \
	test_feed_cache

test_web_backend_SOURCES = libsummer/tests/test-web-backend.c $(SERVER_FILES)
test_web_backend_LDADD = $(TESTSLDADD)
test_web_backend_CFLAGS = $(TESTS_INCLUDES)
test_web_backend_DEPENDENCIES = $(DEPENDENCIES)

test_debug_SOURCES = libsummer/tests/test-debug.c
test_debug_LDADD = $(TESTSLDADD)
test_debug_CFLAGS = $(TESTS_INCLUDES)
test_debug_DEPENDENCIES = $(DEPENDENCIES)

test_download_SOURCES = libsummer/tests/test-download.c $(SERVER_FILES)
test_download_LDADD = $(TESTSLDADD)
test_download_CFLAGS = $(TESTS_INCLUDES)
test_download_DEPENDENCIES = $(DEPENDENCIES)

test_download_torrent_SOURCES = libsummer/tests/test-download-torrent.c $(SERVER_FILES)
test_download_torrent_LDADD = $(TESTSLDADD)
test_download_torrent_CFLAGS = $(TESTS_INCLUDES)
test_download_torrent_DEPENDENCIES = $(DEPENDENCIES)

test_download_youtube_SOURCES = libsummer/tests/test-download-youtube.c $(SERVER_FILES)
test_download_youtube_LDADD = $(TESTSLDADD)
test_download_youtube_CFLAGS = $(TESTS_INCLUDES)
test_download_youtube_DEPENDENCIES = $(DEPENDENCIES)

test_download_web_SOURCES = libsummer/tests/test-download-web.c $(SERVER_FILES)
test_download_web_LDADD = $(TESTSLDADD)
test_download_web_CFLAGS = $(TESTS_INCLUDES)
test_download_web_DEPENDENCIES = $(DEPENDENCIES)

test_atom_feed_SOURCES = libsummer/tests/test-atom-feed.c
test_atom_feed_LDADD = $(TESTSLDADD)
test_atom_feed_CFLAGS = $(TESTS_INCLUDES)
test_atom_feed_DEPENDENCIES = $(DEPENDENCIES)

test_rss2_feed_SOURCES = libsummer/tests/test-rss2-feed.c
test_rss2_feed_LDADD = $(TESTSLDADD)
test_rss2_feed_CFLAGS = $(TESTS_INCLUDES)
test_rss2_feed_DEPENDENCIES = $(DEPENDENCIES)

test_feed_SOURCES = libsummer/tests/test-feed.c $(SERVER_FILES)
test_feed_LDADD = $(TESTSLDADD)
test_feed_CFLAGS = $(TESTS_INCLUDES)
test_feed_DEPENDENCIES = $(DEPENDENCIES)

test_feed_cache_SOURCES = libsummer/tests/test-feed-cache.c
test_feed_cache_LDADD = $(TESTSLDADD)
test_feed_cache_CFLAGS = $(TESTS_INCLUDES)
test_feed_cache_DEPENDENCIES = $(DEPENDENCIES)

EXTRA_DIST += \
	libsummer/tests/atom_feed \
	libsummer/tests/atom_feed_nolink \
	libsummer/tests/epicfu \
	libsummer/tests/rss2sample.xml \
	libsummer/tests/watch?v=SiYurfwzyuY

check-local: test
