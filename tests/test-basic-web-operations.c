#include <libsummer/summer.h>
#include <glib.h>
#include <libsoup/soup.h>

typedef struct {
	SoupServer *server;
} WebFixture;

static void
feed_server (SoupServer *server, SoupMessage *msg, const char *path,
		GHashTable *query, SoupClientContext *client, gpointer user_data)
{
	if (!g_strcmp0 (path, "/feed/epicfu")) {
		gchar *contents;
		gsize length;
		if (!g_file_get_contents ("epicfu", &contents, &length, NULL)) {
			g_error ("Couldn't serve feed");
		}
		const char *mime_type = "text/xml";
		soup_message_set_status (msg, SOUP_STATUS_OK);
		soup_message_set_response (msg, mime_type, SOUP_MEMORY_COPY, contents, length);
		return;
	}
	soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
}

static void
web_setup (WebFixture *fix, gconstpointer data)
{
	g_type_init ();
	g_thread_init (NULL);
	fix->server = soup_server_new (SOUP_SERVER_PORT, 52853, NULL);
	soup_server_add_handler (fix->server, "/feed", feed_server, NULL, NULL);
	soup_server_run_async (fix->server);
}

static void
web_teardown (WebFixture *fix, gconstpointer data)
{
	soup_server_quit (fix->server);
	g_object_unref (fix->server);
}

static gboolean feed_was_downloaded = FALSE;
static GMainLoop *loop;

static void
feed_downloaded_cb (LibsummerFeedDownloader *fdl, GList *list, gpointer *data)
{
	feed_was_downloaded = TRUE;
	g_assert (LIBSUMMER_IS_FEED_DOWNLOADER_RSS (fdl));
	g_assert_cmpint (g_list_length (list), ==, 20);
	gchar* title;
	gchar* description;
	g_object_get (fdl, "title", &title, "description", &description, NULL);
	g_assert_cmpstr (title, ==, "EPIC FU (Quicktime)");
	g_assert_cmpstr (description, ==, "art . music . tech");
	g_main_loop_quit (loop);
}

static void
test_get_feed (WebFixture *fix, gconstpointer test_data)
{
	g_type_init ();
	loop = g_main_loop_new (NULL, TRUE);
	LibsummerFeedDownloader *fdl = libsummer_create_feed_downloader ("http://127.0.0.1:52853/feed/epicfu");
	g_assert (LIBSUMMER_IS_FEED_DOWNLOADER_RSS (fdl));
	g_signal_connect (fdl, "feed-downloaded", G_CALLBACK (feed_downloaded_cb), NULL);
	libsummer_feed_downloader_run (fdl);
	g_main_loop_run (loop);
	g_assert (feed_was_downloaded);
	g_main_loop_unref (loop);
}

int main (int argc, char* argv[]) {
	g_test_init (&argc, &argv, NULL);

	g_test_add ("/BasicWebOperations/Get Feed", WebFixture, 0, web_setup, test_get_feed, web_teardown);
	return g_test_run ();
}
