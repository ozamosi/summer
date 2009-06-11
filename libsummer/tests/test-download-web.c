#include <glib.h>
#include <glib/gstdio.h>
#include <libsummer/summer.h>
#include <libsummer/summer-download-web.h>
#include <time.h>
#include "server.h"

static GMainLoop *loop;
static gint last_received = 0;
static time_t last_timestamp = 0;

static void
on_complete_fail (SummerDownload *dl, gconstpointer user_data)
{
	g_assert_not_reached ();
}

static void
on_fail_noserver (SummerDownload *dl, GError *error, gconstpointer user_data)
{
#if GLIB_CHECK_VERSION(2, 20, 0)
	g_assert_error (error, SUMMER_DOWNLOAD_ERROR, SUMMER_DOWNLOAD_ERROR_INPUT);
#endif
	g_main_loop_quit (loop);
}

static void
on_fail_cannotwrite (SummerDownload *dl, GError *error, gconstpointer user_data)
{
#if GLIB_CHECK_VERSION(2, 20, 0)
	g_assert_error (error, SUMMER_DOWNLOAD_ERROR, SUMMER_DOWNLOAD_ERROR_OUTPUT);
#endif
	g_main_loop_quit (loop);
}

static void
noserver (WebFixture *fix, gconstpointer data)
{
	summer_download_set_default (g_get_tmp_dir (), g_get_home_dir ());
	loop = g_main_loop_new (NULL, TRUE);
	SummerDownload *dl;
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i", PORT+1);
	summer_item_data_append_downloadable (item, url, "video/mp4", 0);
	g_free (url);
	url = NULL;
	dl = summer_create_download (item);
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
	g_object_unref (feed);
	g_signal_connect (dl, "download-complete", G_CALLBACK (on_complete_fail), NULL);
	g_signal_connect (dl, "download-error", G_CALLBACK (on_fail_noserver), NULL);
	summer_download_start (dl);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
cannotwrite (WebFixture *fix, gconstpointer data)
{
	summer_download_set_default (g_get_tmp_dir (), "/");
	loop = g_main_loop_new (NULL, TRUE);
	SummerDownload *dl;
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu", PORT);
	summer_item_data_append_downloadable (item, url, "video/mp4", 0);
	g_free (url);
	url = NULL;
	dl = summer_create_download (item);
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
	g_object_unref (feed);
	g_signal_connect (dl, "download-complete", G_CALLBACK (on_complete_fail), NULL);
	g_signal_connect (dl, "download-error", G_CALLBACK (on_fail_cannotwrite), NULL);
	summer_download_start (dl);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
fail_cb_nofail (SummerDownload *obj, GError* error, gconstpointer user_data)
{
	g_assert_not_reached ();
}

static void
update_cb (SummerDownload *obj, guint64 received, guint64 length, gpointer user_data)
{
	g_assert_cmpint (received, >, 0);
    g_assert_cmpint (received, >=, last_received);
    g_assert_cmpint (received, <=, length);
	last_received = received;

	g_assert_cmpint (last_timestamp, <, time (NULL));
	last_timestamp = time (NULL);

	gchar *contents;
	gsize file_length;
	gchar *tmp_filename = g_build_filename (g_get_tmp_dir (), "epicfu", NULL);
	g_file_get_contents (tmp_filename, &contents, &file_length, NULL);
	g_free (tmp_filename);
	g_assert_cmpint (received, <=, file_length);
	g_free (contents);
}

static void
complete_cb (SummerDownload *obj, gpointer user_data)
{
	gchar *save_path = summer_download_get_save_path (obj);
	g_assert_cmpstr (save_path, !=, NULL);
	gchar *filename = g_build_filename (g_get_home_dir (), "epicfu", NULL);
	g_assert_cmpstr (save_path, ==, filename);
	g_free (filename);
	gchar *rec_contents = NULL, *orig_contents = NULL;
	gsize rec_length, orig_length;
	g_file_get_contents (BASEFILEPATH "/epicfu", &orig_contents, &orig_length, NULL);
    g_file_get_contents (save_path, &rec_contents, &rec_length, NULL);
	g_assert_cmpstr (orig_contents, ==, rec_contents);
	g_assert (summer_download_delete (obj, NULL));
	g_assert (!g_file_test (save_path, G_FILE_TEST_EXISTS));
	g_free (rec_contents);
	g_free (orig_contents);

	gchar *tmp_filename = g_build_filename (g_get_tmp_dir (), "epicfu", NULL);
	g_assert (!g_file_test (tmp_filename, G_FILE_TEST_EXISTS));
	g_free (tmp_filename);

	g_main_loop_quit (loop);
}

static void
started_cb (SummerDownload *dl, gconstpointer data)
{
	gchar *filename;
	g_object_get (dl, "filename", &filename, NULL);
	g_assert_cmpstr (filename, ==, "epicfu");
	g_free (filename);
}

static void
basic (WebFixture *fix, gconstpointer data)
{
	summer_download_set_default (g_get_tmp_dir (), g_get_home_dir ());
	loop = g_main_loop_new (NULL, TRUE);
	SummerDownload *dl;
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu", PORT);
	summer_item_data_append_downloadable (item, url, "video/mp4", 0);
	g_free (url);
	url = NULL;
	dl = summer_create_download (item);
	g_object_unref (feed);
	gchar *filename;
	g_object_get (dl, "filename", &filename, NULL);
	g_assert_cmpstr (filename, ==, NULL);
	g_free (filename);
	filename = NULL;
	g_signal_connect (dl, "download-started", G_CALLBACK (started_cb), NULL);
	g_signal_connect (dl, "download-complete", G_CALLBACK (complete_cb), NULL);
	g_signal_connect (dl, "download-update", G_CALLBACK (update_cb), NULL);
	g_signal_connect (dl, "download-error", G_CALLBACK (fail_cb_nofail), NULL);
	summer_download_start (dl);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}


static void
mimes ()
{
	SummerDownload *dl;
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/", PORT);
	summer_item_data_append_downloadable (item, url, "application/xml", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_object_unref (feed);
	g_assert (!SUMMER_IS_DOWNLOAD_WEB (dl));

	feed = summer_feed_data_new ();
	item = summer_feed_data_append_item (feed);
	url = g_strdup_printf ("http://127.0.0.1:%i/", PORT);
	summer_item_data_append_downloadable (item, url, "application/flac", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_object_unref (feed);
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
	g_object_unref (dl);
	
	feed = summer_feed_data_new ();
	item = summer_feed_data_append_item (feed);
	url = g_strdup_printf ("http://127.0.0.1:%i/video/dummy_mp4", PORT);
	summer_item_data_append_downloadable (item, url, "video/mp4", 0);
	g_free (url);
	dl = summer_create_download (item);
	g_object_unref (feed);
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/download-web/mimes", mimes);
	g_test_add ("/download-web/basic", WebFixture, 0, web_setup, basic, web_teardown);
	g_test_add ("/download-web/noserver", WebFixture, 0, web_setup, noserver, web_teardown);
	g_test_add ("/download-web/cannotwrite", WebFixture, 0, web_setup, cannotwrite, web_teardown);
	return g_test_run ();
}
