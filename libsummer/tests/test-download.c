#include <glib.h>
#include <libsummer/summer.h>
#include "server.h"

static void
create (WebFixture *fix, gconstpointer data)
{
	SummerDownload *dl;
	dl = summer_create_download ("text/xml", "http://localhost:52853/feeds/epicfu");
	g_assert (dl == NULL);
	dl = summer_create_download ("text/html", "http://localhost:52853/does_not-exist");
	g_assert (dl == NULL);
	dl = summer_create_download ("video/mp4", "http://localhost:52853/video/dummy_mp4");
	g_assert (dl != NULL);
	g_object_unref (dl);
}

static void
settings ()
{
	summer_download_set_default (g_get_tmp_dir (), g_get_home_dir ());
	SummerDownload *dl;
	dl = summer_create_download ("video/mp4", "http://localhost:52853/video/dummy_mp4");
	gchar *save_dir, *tmp_dir;
	g_object_get (G_OBJECT (dl), "save-dir", &save_dir, "tmp-dir", &tmp_dir, NULL);
	g_assert_cmpstr (save_dir, ==, g_get_home_dir ());
	g_assert_cmpstr (tmp_dir, ==, g_get_tmp_dir ());
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
