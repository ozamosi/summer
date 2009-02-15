#include <glib.h>
#include <libsummer/summer.h>
#include <libsummer/summer-download-torrent.h>

static void
init ()
{
	SummerDownload *dl;
	SummerItemData *item = summer_item_data_new ();
	summer_item_data_append_downloadable (item, 
		"http://localhost", "application/x-bittorrent", 0);
	dl = summer_create_download (item);
	g_assert (SUMMER_IS_DOWNLOAD_TORRENT (dl));
	g_object_unref (item);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/download-torrent/init", init);

	return g_test_run ();
}
