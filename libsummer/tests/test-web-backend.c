#include <glib.h>
#include <glib-object.h>
#include <libsummer/summer-web-backend.h>
#include "server.h"

static GMainLoop *loop;

static gint last_received = 0;

static void
chunk_cb (SummerWebBackend *web, gint received, gint length, gpointer user_data)
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
	g_file_get_contents ("epicfu", &orig_contents, &orig_length, NULL);
	g_file_get_contents (save_path, &rec_contents, &rec_length, NULL);
	g_assert_cmpstr (orig_contents, ==, rec_contents);
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
	gchar *url =  "http://localhost:52853/feeds/epicfu";
	SummerWebBackend *web = summer_web_backend_new (path, url);
	g_free (path);
	g_object_get (G_OBJECT (web), "save-dir", &path, NULL);
	g_assert_cmpstr (path, ==, orig_path);

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
	g_file_get_contents ("epicfu", &contents, &length, NULL);
	g_assert_cmpstr (save_data, ==, contents);
	g_main_loop_quit (loop);
	last_received = 0.0;
}

static void
to_ram (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url =  "http://localhost:52853/feeds/epicfu";
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
	gchar *save_dir;
	g_object_get (G_OBJECT (web), "save-dir", &save_dir, NULL);
	g_assert_cmpstr (save_dir, ==, NULL);
	g_signal_connect (web, "download-chunk", G_CALLBACK (chunk_cb), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (ram_downloaded_cb), NULL);
	summer_web_backend_fetch (g_object_ref (web));
	g_object_unref (web);
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
response404 (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url =  "http://localhost:52853/doesnotexist";
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
	g_signal_connect (web, "download-chunk", G_CALLBACK (chunk_cb), NULL);
	g_signal_connect (web, "download-complete", G_CALLBACK (r404_response_cb), NULL);
	summer_web_backend_fetch (g_object_ref (web));
	g_object_unref (web);
	g_main_loop_run (loop);
	g_main_loop_unref (loop);
}

static void
serverdown ()
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url =  "http://localhost:52854";
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
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
	g_remove (save_path);
	g_main_loop_quit (loop);
	last_received = 0.0;
}

static void
redirect (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	gchar *url =  "http://localhost:52853/redirect/302";
	SummerWebBackend *web = summer_web_backend_new (g_get_tmp_dir (), url);
	g_signal_connect (web, "download-complete", G_CALLBACK (redirect_response_cb), NULL);
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
	g_test_add ("/web-backend/response404", WebFixture, 0, web_setup, response404, web_teardown);
	g_test_add_func ("/web-backend/serverdown", serverdown);
	g_test_add ("/web-backend/redirect", WebFixture, 0, web_setup, redirect, web_teardown);

	return g_test_run ();
}
