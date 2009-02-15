#include <glib.h>
#include <libsummer/summer.h>
#include <libsummer/summer-download-torrent.h>

static void
init ()
{
	SummerDownload *dl;
	dl = summer_create_download ("application/x-bittorrent", "http://localhost");
	g_assert (SUMMER_IS_DOWNLOAD_TORRENT (dl));
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/download-torrent/init", init);

	return g_test_run ();
}
