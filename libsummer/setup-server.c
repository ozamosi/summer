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
