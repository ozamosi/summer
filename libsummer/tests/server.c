#include "server.h"
#include <glib.h>

void
feed_server (SoupServer *server, SoupMessage *msg, const char *path,
		GHashTable *query, SoupClientContext *client, gpointer user_data)
{
	if (!g_strcmp0 (path, "/feeds/epicfu")) {
		gchar *contents;
		gsize length;
		if (!g_file_get_contents ("../../tests/epicfu", &contents, &length, NULL)) {
			g_error ("Couldn't serve feed");
		}
		const char *mime_type = "text/xml";
		soup_message_set_status (msg, SOUP_STATUS_OK);
		soup_message_set_response (msg, mime_type, SOUP_MEMORY_COPY, contents, length);
		return;
	}
	soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
}

void
web_setup (WebFixture *fix, gconstpointer data)
{
	fix->server = soup_server_new (SOUP_SERVER_PORT, 52853, NULL);
	soup_server_add_handler (fix->server, "/feed", feed_server, NULL, NULL);
	soup_server_run_async (fix->server);
}

void
web_teardown (WebFixture *fix, gconstpointer data)
{
	soup_server_quit (fix->server);
	g_object_unref (fix->server);
}
