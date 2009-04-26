#include <glib.h>
#include <glib-object.h>
#include <libsummer/summer-web-backend.h>
#include "server.h"

static GMainLoop *loop;

static gint last_received = 0;

static void
not_reached (SummerWebBackend *web, ...)
{
	g_assert_not_reached ();
}

static void
chunk_cb (SummerWebBackend *web, guint64 received, guint64 length, gpointer user_data)
{
	g_assert_cmpint (received, >, 0);
	g_assert_cmpint (received, >=, last_received);
	g_assert_cmpint (received, <=, length);
	last_received = received;
}

static void
disk_downloaded_cb (SummerWebBackend *web, gchar *save_path, gchar *save_data, gpointer user_data)
{
	g_assert_cmpstr (save_path, !=, NULL);
	g_assert_cmpstr (save_data, ==, NULL);
	gchar *orig_contents = NULL;
	gchar *rec_contents = NULL;
	gsize orig_length;
	gsize rec_length;
	g_file_get_contents (BASEFILEPATH "/epicfu", &orig_contents, &orig_length, NULL);
	g_file_get_contents (save_path, &rec_contents, &rec_length, NULL);
	g_assert_cmpstr (orig_contents, ==, rec_contents);
	g_free (orig_contents);
	g_free (rec_contents);
	g_remove (save_path);
	g_main_loop_quit (loop);
	last_received = 0.0;
}

static void
to_disk (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *path = g_strdup (g_get_tmp_dir ());
	gchar *orig_path = g_strdup (path);
	gchar *url =  g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu", PORT);
	SummerWebBackend *web = summer_web_backend_new (path, url);
	g_free (url);
	g_free (path);
	g_object_get (G_OBJECT (web), "save-dir", &path, NULL);
	g_assert_cmpstr (path, ==, orig_path);
	g_free (orig_path);
	g_free (path);

	g_signal_connect (web, "download-chunk", G_CALLBACK (chunk_cb), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (disk_downloaded_cb), NULL);
	summer_web_backend_fetch (g_object_ref (web));
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
ram_downloaded_cb (SummerWebBackend *web, gchar *save_path, gchar *save_data, gpointer user_data)
{
	g_assert_cmpstr (save_path, ==, NULL);
	g_assert_cmpstr (save_data, !=, NULL);
	gchar *contents = NULL;
	gsize length = 0;
	g_file_get_contents (BASEFILEPATH "/epicfu", &contents, &length, NULL);
	g_assert_cmpstr (save_data, ==, contents);
	g_free (contents);
	g_main_loop_quit (loop);
	last_received = 0.0;
}

static void
to_ram (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url =  g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu", PORT);
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
	g_free (url);
	gchar *save_dir;
	g_object_get (G_OBJECT (web), "save-dir", &save_dir, NULL);
	g_assert_cmpstr (save_dir, ==, NULL);
	g_signal_connect (web, "download-chunk", G_CALLBACK (chunk_cb), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (ram_downloaded_cb), NULL);
	summer_web_backend_fetch (web);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
r404_response_cb (SummerWebBackend *web, gchar *save_path, gchar *save_data, gpointer user_data)
{
	g_assert_cmpstr (save_path, ==, NULL);
	g_assert_cmpstr (save_data, ==, NULL);
	g_main_loop_quit (loop);
	last_received = 0.0;
}

static void
response404_ram (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url =  g_strdup_printf ("http://127.0.0.1:%i/doesnotexist", PORT);
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
	g_free (url);
	g_signal_connect (web, "download-chunk", G_CALLBACK (not_reached), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (r404_response_cb), NULL);
	summer_web_backend_fetch (web);
	g_main_loop_run (loop);
	g_object_unref (web);
	g_main_loop_unref (loop);
}

static void
response404_disk_complete (SummerWebBackend *web, gchar *save_path, gchar *save_data, gpointer user_data) {
	gchar *filename = g_build_filename (g_get_tmp_dir (), "doesnotexist", NULL);
	g_assert (!g_file_test (filename, G_FILE_TEST_EXISTS));
	g_free (filename);
}

static void
response404_disk (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *path = g_strdup (g_get_tmp_dir ());
	gchar *url =  g_strdup_printf ("http://127.0.0.1:%i/doesnotexist", PORT);
	SummerWebBackend *web = summer_web_backend_new (path, url);
	g_free (url);
	g_signal_connect (web, "download-chunk", G_CALLBACK (not_reached), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (r404_response_cb), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (response404_disk_complete), NULL);
	summer_web_backend_fetch (web);
	g_main_loop_run (loop);
	g_object_unref (web);
	g_remove (path);
	g_free (path);
	g_main_loop_unref (loop);
}

static void
serverdown ()
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i", PORT+1);
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
	g_free (url);
	g_signal_connect (web, "download-chunk", G_CALLBACK (chunk_cb), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (r404_response_cb), NULL);
	summer_web_backend_fetch (g_object_ref (web));
	g_object_unref (web);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
redirect_response_cb (SummerWebBackend *web, gchar *save_path, gchar *save_data, gpointer user_data)
{
	g_assert_cmpstr (save_path, !=, NULL);
	g_assert_cmpstr (save_data, ==, NULL);
	gchar *contents = NULL;
	gsize length;
	g_file_get_contents (save_path, &contents, &length, NULL);
	g_assert_cmpstr (contents, ==, "dummy_video_content");
	g_free (contents);
	g_remove (save_path);
	g_main_loop_quit (loop);
	last_received = 0.0;
}

static void
redirect (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/redirect/302", PORT);
	SummerWebBackend *web = summer_web_backend_new (g_get_tmp_dir (), url);
	g_free (url);
	g_signal_connect (web, "download-complete", G_CALLBACK (redirect_response_cb), NULL);
	summer_web_backend_fetch (web);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static gulong head_complete_id;
static gulong head_chunk_id;

static void
head_got_headers (SummerWebBackend *web, gpointer user_data)
{
	static gboolean run = FALSE;
	g_assert_cmpint (run, ==, FALSE);
	run = TRUE;
	g_signal_connect (web, "download-complete", G_CALLBACK (disk_downloaded_cb), NULL);
	g_signal_connect (web, "headers-parsed", G_CALLBACK (not_reached), NULL);
	g_signal_handler_disconnect (web, head_complete_id);
	g_signal_handler_disconnect (web, head_chunk_id);
	summer_web_backend_fetch (web);

}

static void
head (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/feeds/epicfu", PORT);
	SummerWebBackend *web = summer_web_backend_new (g_get_tmp_dir (), url);
	g_free (url);
	head_complete_id = g_signal_connect_data (web, "download-complete", G_CALLBACK (not_reached), NULL, NULL, 0);
	head_chunk_id = g_signal_connect (web, "download-chunk", G_CALLBACK (not_reached), NULL);
	g_signal_connect (web, "headers-parsed", G_CALLBACK (head_got_headers), NULL);
	summer_web_backend_fetch_head (web);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
slash_complete (SummerWebBackend *web, gchar *path, gchar *data, gpointer *user_data)
{
	gchar *correct_name = "dir_file.vid";
	gchar *reported_name;
	g_object_get (web, "filename", &reported_name, NULL);
	g_assert_cmpstr (correct_name, ==, reported_name);
	g_remove (path);
	g_free (reported_name);
	g_main_loop_quit (loop);
}

static void
slash (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url = g_strdup_printf ("http://127.0.0.1:%i/video/file_with_slash",
		PORT);
	SummerWebBackend *web = summer_web_backend_new (g_get_tmp_dir (), url);
	g_free (url);
	g_signal_connect (web, "download-complete", G_CALLBACK (slash_complete), NULL);
	summer_web_backend_fetch (web);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);

	g_test_add ("/web-backend/to-ram", WebFixture, 0, web_setup, to_ram, web_teardown);
	g_test_add ("/web-backend/to-disk", WebFixture, 0, web_setup, to_disk, web_teardown);
	g_test_add ("/web-backend/response404-ram", WebFixture, 0, web_setup, response404_ram, web_teardown);
	g_test_add ("/web-backend/response404-disk", WebFixture, 0, web_setup, response404_disk, web_teardown);
	g_test_add_func ("/web-backend/serverdown", serverdown);
	g_test_add ("/web-backend/redirect", WebFixture, 0, web_setup, redirect, web_teardown);
	g_test_add ("/web-backend/head", WebFixture, 0, web_setup, head, web_teardown);
	g_test_add ("/web-backend/slash", WebFixture, 0, web_setup, slash, web_teardown);
	return g_test_run ();
}
