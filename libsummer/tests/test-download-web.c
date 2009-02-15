#include <glib.h>
#include <libsummer/summer.h>
#include <libsummer/summer-download-web.h>
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

	gchar *contents;
	gsize file_length;
	gchar *tmp_filename = g_build_filename (g_get_tmp_dir (), "epicfu", NULL);
	g_file_get_contents (tmp_filename, &contents, &file_length, NULL);
	g_free (tmp_filename);
	g_assert_cmpint (received, <=, file_length);
	g_free (contents);
}

static void
complete_cb (SummerDownload *obj, gchar *save_path, gpointer user_data)
{
	g_assert_cmpstr (save_path, !=, NULL);
	gchar *filename = g_build_filename (g_get_home_dir (), "epicfu", NULL);
	g_assert_cmpstr (save_path, ==, filename);
	g_free (filename);
	gchar *rec_contents = NULL, *orig_contents = NULL;
	gsize rec_length, orig_length;
	g_file_get_contents ("epicfu", &orig_contents, &orig_length, NULL);
    g_file_get_contents (save_path, &rec_contents, &rec_length, NULL);
	g_assert_cmpstr (orig_contents, ==, rec_contents);
	g_remove (save_path);
	g_free (rec_contents);
	g_free (orig_contents);

	gchar *tmp_filename = g_build_filename (g_get_tmp_dir (), "epicfu", NULL);
	g_assert (!g_file_test (tmp_filename, G_FILE_TEST_EXISTS));
	g_free (tmp_filename);

	g_main_loop_quit (loop);
}

static void
basic (WebFixture *fix, gconstpointer data)
{
	summer_download_set_default (g_get_tmp_dir (), g_get_home_dir ());
	loop = g_main_loop_new (NULL, TRUE);
	SummerDownload *dl;
	SummerItemData *item = summer_item_data_new ();
	summer_item_data_append_downloadable (item, 
		"http://localhost:52853/feeds/epicfu", "video/mp4", 0);
	dl = summer_create_download (item);
	g_object_unref (item);
	g_signal_connect (dl, "download-complete", G_CALLBACK (complete_cb), NULL);
	g_signal_connect (dl, "download-update", G_CALLBACK (update_cb), NULL);
	summer_download_start (dl);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}


static void
mimes ()
{
	SummerDownload *dl;
	SummerItemData *item = summer_item_data_new ();
	summer_item_data_append_downloadable (item, 
		"http://localhost:52853/", "application/xml", 0);
	dl = summer_create_download (item);
	g_object_unref (item);
	g_assert (!SUMMER_IS_DOWNLOAD_WEB (dl));

	item = summer_item_data_new ();
	summer_item_data_append_downloadable (item, 
		"http://localhost:52853/", "application/flac", 0);
	dl = summer_create_download (item);
	g_object_unref (item);
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
	g_object_unref (dl);
	
	item = summer_item_data_new ();
	summer_item_data_append_downloadable (item, 
		"http://localhost:52853/video/dummy_mp4", "video/mp4", 0);
	dl = summer_create_download (item);
	g_object_unref (item);
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/download-web/mimes", mimes);
	g_test_add ("/download-web/basic", WebFixture, 0, web_setup, basic, web_teardown);
	return g_test_run ();
}
