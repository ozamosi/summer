#include <glib.h>
#include <libsummer/summer.h>
#include "server.h"

static void
create (WebFixture *fix, gconstpointer data)
{
	SummerDownload *dl;
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	gchar *url = g_strdup_printf ("http://localhost:%i/feeds/epicfu", PORT);
	summer_item_data_append_downloadable (item, url, "text/html", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_assert (dl == NULL);
	g_object_unref (feed);

	feed = summer_feed_data_new ();
	item = summer_feed_data_append_item (feed);
	url = g_strdup_printf ("http://localhost:%i/does_not-exist", PORT);
	summer_item_data_append_downloadable (item, url, "text/html", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_assert (dl == NULL);
	g_object_unref (feed);

	feed = summer_feed_data_new ();
	item = summer_feed_data_append_item (feed);
	url = g_strdup_printf ("http://localhost:%i/video/dummy_mp4", PORT);
	summer_item_data_append_downloadable (item, url, "video/mp4", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_assert (dl != NULL);
	g_object_unref (dl);
	g_object_unref (feed);
}

static void
settings ()
{
	summer_download_set_default (g_get_tmp_dir (), g_get_home_dir ());
	SummerDownload *dl;
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	gchar *url = g_strdup_printf ("http://localhost:%i/video/dummy_mp4", PORT);
	summer_item_data_append_downloadable (item, url, "video/mp4", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_object_unref (feed);
	gchar *save_dir, *tmp_dir;
	g_object_get (G_OBJECT (dl), "save-dir", &save_dir, "tmp-dir", &tmp_dir, NULL);
	g_assert_cmpstr (save_dir, ==, g_get_home_dir ());
	g_assert_cmpstr (tmp_dir, ==, g_get_tmp_dir ());
	g_free (save_dir);
	g_free (tmp_dir);
	g_object_unref (dl);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add ("/download/create", WebFixture, 0, web_setup, create, web_teardown);
	g_test_add_func ("/download/settings", settings);
	return g_test_run ();
}
