/* summer-web-backend.c */

/* This file is part of libsummer.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
 * 
 * Libsummer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the license, or (at your option) any later version.
 * 
 * Libsummer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Palace - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "summer-web-backend.h"
#include "summer-marshal.h"
#include <libsoup/soup.h>

/**
 * SECTION:summer-web-backend
 * @short_description: Provides a class for communicating with web servers
 * @stability: Private
 *
 * This component is only meant to be used by the downloaders and the feed
 * fetchers. It contains %SummerWebBackend - a class for communicating with
 * web servers.
 *
 * <example>
 *  <title>Downloading a file with %SummerWebBackend</title>
 *  <programlisting>
 * gchar *directory = "/home/username/Desktop";
 * gchar *url = "http://video.boingboing.net/video/17138/bbtv_2008-07-16-195553.mp4"
 * SummerWebBackend *web = summer_web_backend_new (directory, url);
 * summer_web_backend_fetch (web);
 *  </programlisting>
 * </example>
 */

/**
 * SummerWebBackend:
 *
 * This class implements an interface to communicate with web servers. All 
 * remote data is fetched asynchronously, but local I/O is currently written 
 * synchronously. In the future, all I/O should be asynchronous.
 */

static void summer_web_backend_class_init (SummerWebBackendClass *klass);
static void summer_web_backend_init       (SummerWebBackend *obj);
static void summer_web_backend_finalize   (GObject *obj);

typedef struct _SummerWebBackendPrivate SummerWebBackendPrivate;
struct _SummerWebBackendPrivate {
	gchar *save_dir;
	gchar *url;
	gsize length;
	gsize received;
	gchar *filename;
	GFileOutputStream *outfile;
};
#define SUMMER_WEB_BACKEND_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                SUMMER_TYPE_WEB_BACKEND, \
                                                SummerWebBackendPrivate))
static GObjectClass *parent_class = NULL;
static SoupSession *session = NULL;

enum {
	PROP_0,
	PROP_SAVE_DIR,
	PROP_URL
};
/* uncomment the following if you have defined any signals */
/* static guint signals[LAST_SIGNAL] = {0}; */

GType
summer_web_backend_get_type (void)
{
	static GType my_type = 0;
	if (!my_type) {
		static const GTypeInfo my_info = {
			sizeof(SummerWebBackendClass),
			NULL,		/* base init */
			NULL,		/* base finalize */
			(GClassInitFunc) summer_web_backend_class_init,
			NULL,		/* class finalize */
			NULL,		/* class data */
			sizeof(SummerWebBackend),
			1,		/* n_preallocs */
			(GInstanceInitFunc) summer_web_backend_init,
			NULL
		};
		my_type = g_type_register_static (G_TYPE_OBJECT,
		                                  "SummerWebBackend",
		                                  &my_info, 0);
	}
	return my_type;
}

static void
set_property (GObject *object, guint property_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerWebBackendPrivate *priv;
	priv = SUMMER_WEB_BACKEND_GET_PRIVATE (object);

	switch (property_id) {
	case PROP_SAVE_DIR:
		if (priv->save_dir)
			g_free (priv->save_dir);
		priv->save_dir = g_value_dup_string (value);
		break;
	case PROP_URL:
		if (priv->url)
			g_free (priv->url);
		priv->url = g_value_dup_string (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
get_property (GObject *object, guint property_id, GValue *value, 
	GParamSpec *pspec)
{
	SummerWebBackendPrivate *priv;
	priv = SUMMER_WEB_BACKEND_GET_PRIVATE (object);

	switch (property_id) {
	case PROP_SAVE_DIR:
		g_value_set_string (value, priv->save_dir);
		break;
	case PROP_URL:
		g_value_set_string (value, priv->url);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
summer_web_backend_class_init (SummerWebBackendClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	parent_class            = g_type_class_peek_parent (klass);
	gobject_class->finalize = summer_web_backend_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	g_type_class_add_private (gobject_class, sizeof(SummerWebBackendPrivate));

	GParamSpec *pspec;
	pspec = g_param_spec_string ("save-dir",
		"Save directory",
		"The directory to save the file to. NULL to keep the file in memory",
		NULL,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	/**
	 * SummerWebBackend:save-dir:
	 *
	 * Specifies a directory to save the file to. If it is %NULL, the file will
	 * not be saved to disk, but only to RAM.
	 */
	g_object_class_install_property (gobject_class, PROP_SAVE_DIR, pspec);

	pspec = g_param_spec_string ("url",
		"URL",
		"The URL to download",
		NULL,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	/**
	 * SummerWebBackend:url:
	 *
	 * Specifies a URL to download from.
	 */
	g_object_class_install_property (gobject_class, PROP_URL, pspec);

	/**
	 * SummerWebBackend::download-complete:
	 * @web_backend: the %SummerWebBackend that emitted the signal
	 * @save_path: the path in the file system that the file was downloaded to.
	 * %NULL if it wasn't saved or if there was an error.
	 * @save_data: the actual file, as a string. %NULL if the file was saved to 
	 * disk or if there was an error.
	 *
	 * Signal that is emitted when the whole file has been downloaded, either
	 * to disk, or to RAM. Note that either @save_path or @save_data is always 
	 * %NULL, but both being %NULL signals an error.
	 */
	 g_signal_new (
	 	"download-complete",
		SUMMER_TYPE_WEB_BACKEND,
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (SummerWebBackendClass, download_complete),
		NULL, NULL,
		summer_marshal_VOID__STRING_STRING,
		G_TYPE_NONE,
		2,
		G_TYPE_STRING, G_TYPE_STRING);

	 /**
	  * SummerWebBackend::download-chunk:
	  * @web_backend: the %SummerWebBackend that emitted the signal
	  * @received: the number of bytes of the file that has been downloaded. 
	  * @length: the total number of bytes to be downloaded. %-1 if this is not
	  * known.
	  *
	  * ::download-chunk is emitted every time a new block of the file have been
	  * retrieved.
	  */
	 g_signal_new (
	 	"download-chunk",
		SUMMER_TYPE_WEB_BACKEND,
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (SummerWebBackendClass, download_chunk),
		NULL, NULL,
		summer_marshal_VOID__INT_INT,
		G_TYPE_NONE,
		2,
		G_TYPE_INT, G_TYPE_INT);
}

static void
summer_web_backend_init (SummerWebBackend *self)
{
	if (session == NULL)
		session = soup_session_async_new ();
	else
		g_object_ref (session);
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND_GET_PRIVATE (self);
	priv->filename = NULL;
	priv->outfile = NULL;
}

static void
summer_web_backend_finalize (GObject *self)
{
	g_object_unref (session);
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND_GET_PRIVATE(self);
	if (priv->save_dir != NULL)
		g_free (priv->save_dir);
	if (priv->url != NULL)
		g_free (priv->url);
	if (priv->filename != NULL)
		g_free (priv->filename);
	if (priv->outfile != NULL)
		g_object_unref (priv->outfile);
	G_OBJECT_CLASS (parent_class)->finalize (self);
}

/**
 * summer_web_backend_new:
 * @save_dir: the directory to save downloaded files to.
 * @url: the url to download from.
 *
 * Creates a new #SummerWebBackend.
 *
 * Returns: the newly created #SummerWebBackend object
 */
SummerWebBackend*
summer_web_backend_new (gchar *save_dir, gchar *url)
{
	return SUMMER_WEB_BACKEND(g_object_new(SUMMER_TYPE_WEB_BACKEND, "save-dir", g_strdup (save_dir), "url", g_strdup (url), NULL));
}

static void
on_downloaded (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_SESSION (session));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND_GET_PRIVATE(self);
	if (priv->outfile) {
		GError *error = NULL;
		g_output_stream_close (G_OUTPUT_STREAM (priv->outfile), NULL, &error);
		if (error) {
			g_warning ("%s", error->message);
			g_clear_error (&error);
		}
	}
	g_signal_emit_by_name (self, "download-complete", priv->filename, msg->response_body->data);
}

static void
on_got_chunk (SoupMessage *msg, SoupBuffer *chunk, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND_GET_PRIVATE(self);
	
	if (priv->outfile) {
		GError *error = NULL;
		gsize written;
		g_output_stream_write_all (G_OUTPUT_STREAM (priv->outfile), chunk->data, chunk->length, &written, NULL, &error);
		if (error) {
			g_warning ("%s", error->message);
			g_clear_error (&error);
		}
	}
	priv->received += chunk->length;
	g_signal_emit_by_name (self, "download-chunk", priv->received, priv->length);
}

static void
on_got_headers (SoupMessage *msg, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND_GET_PRIVATE (self);
	if (msg->status_code >= 400) {
		g_signal_emit_by_name (self, "download-complete", NULL, NULL);
		g_object_unref (msg); //FIXME: does this properly terminate the connection?
	}
	goffset length;
	length = soup_message_headers_get_content_length (msg->response_headers);
	if (length)
		priv->length = length;
}

/**
 * summer_web_backend_fetch:
 * @self: a #SummerWebBackend instance
 *
 * Starts a file transfer from #SummerWebBackend:url. Since the transfer is asynchronous, the
 * function will not return anything - instead, connect to the 
 * #SummerWebBackend::download-complete signal.
 *
 * Returns: void
 */
void
summer_web_backend_fetch (SummerWebBackend *self)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (self));
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND_GET_PRIVATE(self);

	if (priv->save_dir != NULL) {
		gchar** parts = g_strsplit (priv->url, "/", 0);
		priv->filename = g_build_filename (priv->save_dir, parts[sizeof (parts)], NULL);
		g_strfreev (parts);
		GFile *file = g_file_new_for_path (priv->filename);
		GError *error = NULL;
		priv->outfile = g_file_replace (file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);
		if (!priv->outfile) {
			g_warning ("%s", error->message);
			g_clear_error (&error);
		}
	}

	SoupMessage *msg;
	msg = soup_message_new ("GET", priv->url);

	if (priv->save_dir != NULL) {
		soup_message_body_set_accumulate (msg->response_body, FALSE);
	}
	g_signal_connect (msg, "got-chunk", G_CALLBACK (on_got_chunk), self);
	g_signal_connect (msg, "got-headers", G_CALLBACK (on_got_headers), self);
	soup_session_queue_message (session, msg, on_downloaded, self);
}
