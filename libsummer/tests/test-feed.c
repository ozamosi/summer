#include <glib.h>
#include <libsummer/summer.h>
#include "server.h"

static GMainLoop *loop;

static void
on_new_entries (SummerFeed *feed, gconstpointer data) {
	GList *items = summer_feed_peek_items (feed);
	g_assert_cmpint (g_list_length (items), ==, 20);
	g_list_free (items);
	items = summer_feed_get_items (feed);
	g_assert_cmpint (g_list_length (items), ==, 20);
	g_list_free (items);
	items = summer_feed_get_items (feed);
	g_assert_cmpint (g_list_length (items), ==, 0);
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
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu", PORT);
	summer_feed_start (feed, url);
	g_free (url);
	g_assert (feed != NULL);
	g_object_unref (feed);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
on_invalid (SummerFeed *feed, gconstpointer data) {
	GList *items = summer_feed_get_items (feed);
	g_assert_cmpint (g_list_length (items), ==, 0);
	g_main_loop_quit (loop);
}

static void
invalid (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	SummerFeed *feed;
	feed = summer_feed_new ();
	g_signal_connect (feed, "new-entries", G_CALLBACK (on_invalid), NULL);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/video/dummy_mp4", PORT);
	summer_feed_start (feed, url);
	g_free (url);
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
	gchar *orig_url = g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu",
		PORT);
	summer_feed_start (feed, orig_url);
	gchar *cache_dir = summer_feed_get_cache_dir (feed);
	gint frequency = summer_feed_get_frequency (feed);
	gchar *url = summer_feed_get_url (feed);
	g_assert_cmpstr (cache_dir, ==, g_get_tmp_dir ());
	g_assert_cmpint (frequency, ==, 900);
	g_assert_cmpstr (url, ==, orig_url);
	g_free (orig_url);
	g_free (cache_dir);
	g_free (url);
	g_object_unref (feed);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);

	g_test_add ("/feed/create", WebFixture, 0, web_setup, create, web_teardown);
	g_test_add ("/feed/invalid", WebFixture, 0, web_setup, invalid, web_teardown);
	g_test_add ("/feed/settings", WebFixture, 0, web_setup, settings, web_teardown);
	summer_shutdown ();

	return g_test_run ();
}
