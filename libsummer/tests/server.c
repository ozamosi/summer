#include "server.h"
#include <glib.h>

void
video_server (SoupServer *server, SoupMessage *msg, const char *path,
		GHashTable *query, SoupClientContext *client, gpointer user_data)
{
	const gchar *contents = "dummy_video_content";
	gsize length = sizeof (contents);
	gchar *mime_type = NULL;

	if (!g_strcmp0 (path, "/video/dummy_mp4"))
		mime_type = "video/mp4";
	else if (!g_strcmp0 (path, "/video/dummy_mov"))
		mime_type = "video/mov";

	if (mime_type) {
		soup_message_set_status (msg, SOUP_STATUS_OK);
		soup_message_set_response (msg, mime_type, SOUP_MEMORY_COPY, contents, length);
		return;
	}
	soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
}

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
	if (!g_strcmp0 (path, "/feeds/atom")) {
		gchar *contents;
		gsize length;
		if (!g_file_get_contents ("atom_feed", &contents, &length, NULL)) {
			g_error ("Couldn't serve feed");
		}
		const char *mime_type = "text/xml";
		soup_message_set_status (msg, SOUP_STATUS_OK);
		soup_message_set_response (msg, mime_type, SOUP_MEMORY_COPY, contents, length);
		return;
	}
	if (!g_strcmp0 (path, "/feeds/rss2sample.xml")) {
		gchar *contents;
		gsize length;
		if (!g_file_get_contents ("rss2sample.xml", &contents, &length, NULL)) {
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
	soup_server_add_handler (fix->server, "/feeds/", feed_server, NULL, NULL);
	soup_server_add_handler (fix->server, "/video/", video_server, NULL, NULL);
	soup_server_run_async (fix->server);
}

void
web_teardown (WebFixture *fix, gconstpointer data)
{
	soup_server_quit (fix->server);
	g_object_unref (fix->server);
}
