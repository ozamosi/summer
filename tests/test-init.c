#include <glib.h>
#include <libsummer/summer.h>

void init_rss_test () {
	gint check_frequency = 900;
	gchar *cache_path = g_strdup (g_get_user_cache_dir ());
	summer_set ("feed",
			"cache-path", cache_path,
			"check-frequency", check_frequency,
			NULL);
	SummerFeed *feed = summer_create_feed ("http://foo.com");
	g_assert (SUMMER_IS_FEED_RSS (feed));
	gchar *res_cache;
	gint res_frequency;
	g_object_get (G_OBJECT (feed), 
		"cache-path", &res_cache, 
		"check-frequency", &res_frequency, NULL);
	g_assert_cmpstr (cache_path, ==, res_cache);
	g_assert_cmpint (check_frequency, ==, res_frequency);
	g_free (cache_path);
	g_free (res_cache);
}

void init_web_test () {
	gchar* tmp_path = g_strdup (g_get_user_cache_dir ());
	gchar* save_path = g_strdup (g_get_user_special_dir (G_USER_DIRECTORY_DESKTOP));
	summer_set ("download",
			"tmp-path", tmp_path,
			"save-path", save_path,
			NULL);
	SummerDownload *dl = summer_create_download ("http://foo.com", "video/mp4");
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
	gchar *res_tmp, *res_save;
	g_object_get (G_OBJECT (dl), "tmp-path", &res_tmp, "save-path", &res_save, NULL);
	g_assert_cmpstr (tmp_path, ==, res_tmp);
	g_assert_cmpstr (save_path, ==, res_save);
	g_free (tmp_path);
	g_free (res_tmp);
	g_free (save_path);
	g_free (res_save);
}

int main (int argc, char* argv[]) {
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/TestLibsummer/Init web downloader test", init_web_test);
	g_test_add_func ("/TestLibsummer/Init RSS feed downloader test", init_rss_test);

	return g_test_run ();
}
