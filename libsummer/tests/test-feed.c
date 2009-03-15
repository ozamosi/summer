#include <glib.h>
#include <libsummer/summer.h>
#include "server.h"

static GMainLoop *loop;

static void
on_new_entries (SummerFeed *feed, gconstpointer data) {
	GList *items = summer_feed_get_items (feed);
	g_assert_cmpint (g_list_length (items), ==, 20);
	g_list_free (items);
	g_main_loop_quit (loop);
}

static void
create (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	SummerFeed *feed;
	feed = summer_feed_new ();
	g_signal_connect (feed, "new-entries", G_CALLBACK (on_new_entries), NULL);
	summer_feed_start (feed, "http://localhost:52853/feeds/epicfu");
	g_assert (feed != NULL);
	g_object_unref (feed);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
settings ()
{
	summer_feed_set_default (g_get_tmp_dir (), 900);
	SummerFeed *feed;
	feed = summer_feed_new ();
	summer_feed_start (feed, "http://localhost:52853/feeds/epicfu");
	gchar *cache_dir;
	gchar *url;
	gint frequency;
	g_object_get (feed, "cache-dir", &cache_dir, "frequency", &frequency, "url", &url, NULL);
	g_assert_cmpstr (cache_dir, ==, g_get_tmp_dir ());
	g_assert_cmpint (frequency, ==, 900);
	g_assert_cmpstr (url, ==, "http://localhost:52853/feeds/epicfu");
	g_free (cache_dir);
	g_free (url);
	g_object_unref (feed);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);

	g_test_add ("/feed/create", WebFixture, 0, web_setup, create, web_teardown);
	g_test_add ("/feed/settings", WebFixture, 0, web_setup, settings, web_teardown);
	summer_shutdown ();

	return g_test_run ();
}
