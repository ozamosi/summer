#include <glib.h>
#include <libsummer/summer.h>
#include <libsummer/summer-download-youtube.h>
#include "server.h"

static GMainLoop *loop;

static gint last_received = 0;

static void
update_cb (SummerDownload *obj, gint received, gint length, gpointer user_data)
{
    g_assert_cmpint (received, >, 0);
    g_assert_cmpint (received, >=, last_received);
    g_assert_cmpint (received, <=, length);
    last_received = received;
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

    g_main_loop_quit (loop);
}

static void
basic (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	item->web_url = "http://www.youtube.com/watch?v=SiYurfwzyuY";
	SummerDownload *dl = summer_download_youtube_new (item);
	g_assert (SUMMER_IS_DOWNLOAD_YOUTUBE (dl));
	item->web_url = g_strdup_printf ("http://localhost:%i/video/youtube", PORT);
	g_signal_connect (dl, "download-complete", G_CALLBACK (complete_cb), NULL);
	g_signal_connect (dl, "download-update", G_CALLBACK (update_cb), NULL);
	//summer_download_start (dl);
	//g_main_loop_run (loop);
	g_main_loop_unref (loop);
	g_object_unref (dl);
	g_object_unref (feed);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add ("/download-youtube/basic", WebFixture, 0, web_setup, basic, web_teardown);
	return g_test_run ();
}
