#include <glib.h>
#include <libsummer/summer.h>
#include <libsummer/summer-download-torrent.h>
#include <libsummer/summer-download-web.h>
#include "server.h"

static GMainLoop *loop;

static void
init ()
{
	SummerDownload *dl;
	SummerItemData *item = summer_item_data_new ();
	summer_item_data_append_downloadable (item, 
		"http://localhost", "application/x-bittorrent", 0);
	dl = summer_create_download (item);
#ifndef ENABLE_BITTORRENT
	g_assert (SUMMER_IS_DOWNLOAD_WEB (dl));
#else
	g_assert (SUMMER_IS_DOWNLOAD_TORRENT (dl));
#endif
	g_object_unref (dl);
	g_object_unref (item);
}

#ifdef ENABLE_BITTORRENT
static void
invalid_torrent (WebFixture *fix, gconstpointer data)
{
	loop = g_main_loop_new (NULL, TRUE);
	summer_download_set_default (g_get_tmp_dir (), NULL);
	SummerDownload *dl;
	SummerItemData *item = summer_item_data_new ();
	gchar *url = g_strdup_printf ("http://localhost:%i/video/dummy_mp4", PORT);
	summer_item_data_append_downloadable (item, 
		url,
		"application/x-bittorrent",
		0);
	g_free (url);
	dl = summer_download_torrent_new (item);
	g_assert (SUMMER_IS_DOWNLOAD_TORRENT (dl));
// TODO: Need proper error handling before this works (#12)
/*	summer_download_start (dl);
	g_main_loop_run (loop);
	g_object_unref (dl);
	g_main_loop_unref (loop);
*/
}
#endif

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/download-torrent/init", init);
#ifdef ENABLE_BITTORRENT
	g_test_add ("/download-torrent/invalid_torrent", WebFixture, 0, web_setup, invalid_torrent, web_teardown);
#endif

	return g_test_run ();
}
