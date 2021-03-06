#include "server.h"
#include <glib.h>
#include <string.h>

void
redirect_server (SoupServer *server, SoupMessage *msg, const char *path,
		GHashTable *query, SoupClientContext *client, gpointer user_data)
{
	const gchar *contents = "error message";
	gsize length = strlen (contents);
	if (!g_strcmp0 (path, "/redirect/302"))
		soup_message_set_status (msg, 302);
	soup_message_headers_append (msg->response_headers, "Location", g_strdup_printf ("http://127.0.0.1:%i/video/dummy_mp4", PORT));
	soup_message_set_response (msg, "text/plain", SOUP_MEMORY_COPY, contents, length);
}

void
video_server (SoupServer *server, SoupMessage *msg, const char *path,
		GHashTable *query, SoupClientContext *client, gpointer user_data)
{
	const gchar *contents = "dummy_video_content";
	gsize length = strlen (contents);
	gchar *mime_type = NULL;

	if (!g_strcmp0 (path, "/video/dummy_mp4"))
		mime_type = "video/mp4";
	else if (!g_strcmp0 (path, "/video/dummy_mov"))
		mime_type = "video/mov";

	if (mime_type) {
		soup_message_set_status (msg, SOUP_STATUS_OK);
		if (g_strcmp0 (msg->method, "HEAD"))
			soup_message_set_response (msg, 
				mime_type, 
				SOUP_MEMORY_COPY, 
				contents, 
				length);
		return;
	}

	if (!g_strcmp0 (path, "/video/youtube")) {
		gchar *page_contents;
		gsize length;
		if (!g_file_get_contents (BASEFILEPATH "/watch?v=SiYurfwzyuY", &page_contents, &length, NULL)) {
			g_error ("Couldn't serve video file");
		}
		soup_message_set_status (msg, SOUP_STATUS_OK);
		soup_message_set_response (msg, "text/html", SOUP_MEMORY_COPY, page_contents, length);
		return;
	}

	if (!g_strcmp0 (path, "/video/file_with_slash")) {
		gchar *page_contents = "dummy";
		gsize length = g_utf8_strlen ("dummy", -1);
		soup_message_set_status (msg, SOUP_STATUS_OK);
		soup_message_set_response (msg, "video/none", SOUP_MEMORY_COPY, page_contents, length);
		soup_message_headers_replace (msg->response_headers, "Content-Disposition", "attachment; filename=\"dir/file.vid\";");
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
		if (!g_file_get_contents (BASEFILEPATH "/epicfu", &contents, &length, NULL)) {
			g_error ("Couldn't serve feed");
		}
		const char *mime_type = "text/xml";
		soup_message_set_status (msg, SOUP_STATUS_OK);
		if (g_strcmp0 (msg->method, "HEAD"))
			soup_message_set_response (msg, 
				mime_type, 
				SOUP_MEMORY_COPY, 
				contents, 
				length);
		g_free (contents);
		return;
	}
	if (!g_strcmp0 (path, "/feeds/atom")) {
		gchar *contents;
		gsize length;
		if (!g_file_get_contents (BASEFILEPATH "/atom_feed", &contents, &length, NULL)) {
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
		if (!g_file_get_contents (BASEFILEPATH "/rss2sample.xml", &contents, &length, NULL)) {
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
	if (!PORT)
		PORT = 52853;
	fix->server = soup_server_new (SOUP_SERVER_PORT, PORT, NULL);
	while (!SOUP_IS_SERVER (fix->server)) {
		PORT++;
		fix->server = soup_server_new (SOUP_SERVER_PORT, PORT, NULL);
	}

	soup_server_add_handler (fix->server, "/feeds/", feed_server, NULL, NULL);
	soup_server_add_handler (fix->server, "/video/", video_server, NULL, NULL);
	soup_server_add_handler (fix->server, "/redirect/", redirect_server, NULL, NULL);
	soup_server_run_async (fix->server);
}

void
web_teardown (WebFixture *fix, gconstpointer data)
{
	soup_server_quit (fix->server);
	g_object_unref (fix->server);
}
