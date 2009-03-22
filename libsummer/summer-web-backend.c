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
#include "summer-debug.h"
#include <libsoup/soup.h>
#include <string.h>

/**
 * SECTION:summer-web-backend
 * @short_description: Provides a class for communicating with web servers
 * @stability: Private
 * @include: libsummer/summer-web-backend.h
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

struct _SummerWebBackendPrivate {
	gchar *save_dir;
	gchar *url;
	const gchar *filename;
	gchar *pretty_filename;
	guint64 length;
	guint64 received;
	gboolean fetch;
	GFileOutputStream *outfile;
	GFile *filehandle;
	gboolean head_emitted;
};
#define SUMMER_WEB_BACKEND_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                           SUMMER_TYPE_WEB_BACKEND, \
                                           SummerWebBackendPrivate))
static SoupSession *session = NULL;

enum {
	PROP_0,
	PROP_SAVE_DIR,
	PROP_URL,
	PROP_FILENAME,
	PROP_LENGTH
};

G_DEFINE_TYPE (SummerWebBackend, summer_web_backend, G_TYPE_OBJECT);

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND (object)->priv;

	switch (prop_id) {
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
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND (object)->priv;

	switch (prop_id) {
	case PROP_SAVE_DIR:
		g_value_set_string (value, priv->save_dir);
		break;
	case PROP_URL:
		g_value_set_string (value, priv->url);
		break;
	case PROP_FILENAME:
		if (priv->pretty_filename != NULL)
			g_value_set_string (value, priv->pretty_filename);
		else
			g_value_set_string (value, priv->filename);
		break;
	case PROP_LENGTH:
		g_value_set_uint64 (value, priv->length);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static GObject *
constructor (GType gtype, guint n_properties, GObjectConstructParam *properties)
{
	GObject *obj;
	GObjectClass *parent_class;
	parent_class = G_OBJECT_CLASS (summer_web_backend_parent_class);
	obj = parent_class->constructor (gtype, n_properties, properties);
	
	/* Discover filename part of URL */
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND (obj)->priv;
	gchar** parts = g_strsplit (priv->url, "/", 0);
	gchar** filename;
	for (filename = parts; *filename != NULL; filename++) {}
	if (!g_strcmp0 (*(--filename), ""))
		filename--;
	priv->filename = g_strdup (*filename);
	g_strfreev (parts);

	return obj;
}

static void
summer_web_backend_class_init (SummerWebBackendClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_web_backend_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;
	gobject_class->constructor = constructor;

	g_type_class_add_private (gobject_class, sizeof(SummerWebBackendPrivate));

	GParamSpec *pspec;
	/**
	 * SummerWebBackend:save-dir:
	 *
	 * Specifies a directory to save the file to. If it is %NULL, the file will
	 * not be saved to disk, but only to RAM.
	 */
	pspec = g_param_spec_string ("save-dir",
		"Save directory",
		"The directory to save the file to. NULL to keep the file in memory",
		NULL,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROP_SAVE_DIR, pspec);

	pspec = g_param_spec_string ("url",
		"URL",
		"Specifies a URL to download from.",
		NULL,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	g_object_class_install_property (gobject_class, PROP_URL, pspec);

	/**
	 * SummerWebBackend:filename:
	 *
	 * The pretty name of the file. This may or may not be the same as the name
	 * of the temp file.
	 */
	pspec = g_param_spec_string ("filename",
		"Filename",
		"The pretty name of the file",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_FILENAME, pspec);

	pspec = g_param_spec_uint64 ("length",
		"Length",
		"The reported length of the file",
		0, G_MAXUINT64, 0,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_LENGTH, pspec);

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
	 * @user_data: user-provided data.
	 *
	 * ::download-chunk is emitted when a new block of the file has been
	 * retrieved.
	 */
	g_signal_new (
		"download-chunk",
		SUMMER_TYPE_WEB_BACKEND,
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (SummerWebBackendClass, download_chunk),
		NULL, NULL,
		summer_marshal_VOID__UINT64_UINT64,
		G_TYPE_NONE,
		2,
		G_TYPE_UINT64, G_TYPE_UINT64);

	/**
	 * SummerWebBackend::headers-parsed:
	 * @web_backend: the %SummerWebBackend that emitted the signal
	 * @user_data: user-provided data.
	 *
	 * Emitted when the headers of a transfer has been downloaded and
	 * transfered. Only emitted once per %SummerWebBackend instance, so if you
	 * call %summer_web_backend_fetch_head(), it will not be emitted during
	 * subsequent calls to %summer_web_backend_fetch_head() or 
	 * %summer_web_backend_fetch()
	 */
	g_signal_new (
		"headers-parsed",
		SUMMER_TYPE_WEB_BACKEND,
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (SummerWebBackendClass, headers_parsed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE,
		0);
}

static void
summer_web_backend_init (SummerWebBackend *self)
{
	if (G_IS_OBJECT (session))
		g_object_ref (session);
	else {
		session = soup_session_async_new_with_options (
			"max-conns-per-host", 2,
			"user-agent", "libsummer/0.1 ",
			NULL);
		if (summer_debug (NULL) == TRUE) {
			SoupLogger *log = soup_logger_new (SOUP_LOGGER_LOG_MINIMAL, -1);
			soup_session_add_feature (session, SOUP_SESSION_FEATURE (log));
		}
	}
	self->priv = SUMMER_WEB_BACKEND_GET_PRIVATE (self);
}

static void
summer_web_backend_finalize (GObject *self)
{
	g_object_unref (session);
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND (self)->priv;
	if (priv->save_dir != NULL)
		g_free (priv->save_dir);
	if (priv->url != NULL)
		g_free (priv->url);
	if (priv->filename != NULL)
		g_free ((gchar *)priv->filename);
	if (priv->pretty_filename != NULL)
		g_free (priv->pretty_filename);
	if (G_IS_OBJECT (priv->outfile))
		g_object_unref (priv->outfile);
	G_OBJECT_CLASS (summer_web_backend_parent_class)->finalize (self);
}

/**
 * summer_web_backend_new:
 * @save_dir: the directory to save downloaded files to.
 * @url: the url to download from.
 *
 * Creates a new #SummerWebBackend.
 *
 * Returns: a newly created #SummerWebBackend object
 */
SummerWebBackend*
summer_web_backend_new (const gchar *save_dir, const gchar *url)
{
	g_return_val_if_fail (url != NULL, NULL);
	return SUMMER_WEB_BACKEND(g_object_new(SUMMER_TYPE_WEB_BACKEND, "save-dir", save_dir, "url", url, NULL));
}

static void
on_downloaded (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_SESSION (session));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	SummerWebBackendPrivate *priv = self->priv;
	GError *error = NULL;
	
	if (G_IS_OUTPUT_STREAM (priv->outfile) && !g_output_stream_is_closed (G_OUTPUT_STREAM (priv->outfile))) {
		g_output_stream_close (G_OUTPUT_STREAM (priv->outfile), NULL, &error);
		if (error) {
			g_warning ("%s", error->message);
			g_clear_error (&error);
		}
	}
	if (G_IS_OUTPUT_STREAM (priv->outfile))
		g_object_unref (priv->outfile);
	
	gchar *filepath = NULL;
	if (priv->save_dir)
		filepath = g_build_filename (priv->save_dir, priv->filename, NULL);

	if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		g_signal_emit_by_name (self, "download-complete", filepath, msg->response_body->data);
	} else {
		if (filepath) {
			g_file_delete (priv->filehandle, NULL, &error);
			if (error) {
				g_warning ("%s", error->message);
				g_clear_error (&error);
			}
		}
		g_signal_emit_by_name (self, "download-complete", NULL, NULL);
	}
	if (filepath)
		g_free (filepath);
}

static void
on_got_chunk (SoupMessage *msg, SoupBuffer *chunk, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	SummerWebBackendPrivate *priv = self->priv;

	if (!priv->fetch)
		return;
	
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

static gchar *
get_filename (SoupMessage *msg)
{
	SoupMessageHeadersIter iter;
	const gchar *name;
	const gchar *value;
	soup_message_headers_iter_init (&iter, msg->response_headers);
	while (soup_message_headers_iter_next (&iter, &name, &value)) {
		if (!g_strcmp0 (name, "Content-Disposition")) {
			gchar *filename_start = strstr (value, "filename=");
			if (filename_start == NULL)
				continue;
			filename_start += 9; //strlen ("filename=")
			if (*filename_start == '"')
				filename_start++;
			gchar *filename_end = strstr (filename_start, ";");
			gsize length;
			if (filename_end != NULL)
				length = filename_end - filename_start;
			else
				length = strlen (filename_start);
			if (*(filename_start + length - 1) == '"')
				length--;
			
			gchar *result = g_strndup (filename_start, length);
			gchar *slash;
			while ((slash = g_utf8_strchr (result, length, '/'))) {
				*slash = '_';
			}
			return result;
		}
	}
	return NULL;
}

static void
on_got_headers (SoupMessage *msg, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	SummerWebBackendPrivate *priv = self->priv;
	if (msg->status_code >= 400 || msg->status_code < 100) {
		soup_session_cancel_message (session, msg, SOUP_STATUS_CANCELLED);
		return;
	}
	else if (! SOUP_STATUS_IS_REDIRECTION (msg->status_code))
		priv->fetch = TRUE;

	goffset length;
	length = soup_message_headers_get_content_length (msg->response_headers);
	if (length)
		priv->length = length;
	
	if (priv->pretty_filename)
		g_free (priv->pretty_filename);
	priv->pretty_filename = get_filename (msg);

	if (!priv->head_emitted) {
		priv->head_emitted = TRUE;
		g_signal_emit_by_name (self, "headers-parsed");
	}
}

/**
 * summer_web_backend_fetch:
 * @self: a #SummerWebBackend instance
 *
 * Starts a file transfer from #SummerWebBackend:url. Since the transfer is asynchronous, the
 * function will not return anything - instead, connect to the 
 * #SummerWebBackend::download-complete signal.
 */
void
summer_web_backend_fetch (SummerWebBackend *self)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (self));
	SummerWebBackendPrivate *priv = self->priv;

	SoupMessage *msg = soup_message_new ("GET", priv->url);

	if (priv->save_dir != NULL) {
		gchar *filepath = g_build_filename (priv->save_dir, priv->filename, NULL);
		priv->filehandle = g_file_new_for_path (filepath);
		g_free (filepath);
		GError *error = NULL;
		GFile *directory = g_file_get_parent (priv->filehandle);
		if (!g_file_query_exists (directory, NULL)) {
			if (!g_file_make_directory_with_parents (directory, NULL, &error)) {
				g_warning ("Error creating directory for download: %s", error->message);
				g_clear_error (&error);
			}
		}
		g_object_unref (directory);

		priv->outfile = g_file_replace (priv->filehandle, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);
		if (!priv->outfile) {
			g_warning ("Error opening output stream: %s", error->message);
			g_clear_error (&error);
		}
		priv->fetch = FALSE;

		soup_message_body_set_accumulate (msg->response_body, FALSE);
	}

	g_signal_connect (msg, "got-chunk", G_CALLBACK (on_got_chunk), self);
	g_signal_connect (msg, "got-headers", G_CALLBACK (on_got_headers), self);
	soup_session_queue_message (session, msg, on_downloaded, self);
}

/**
 * summer_web_backend_fetch_head:
 * @self: a #SummerWebBackend instance
 *
 * Performs a HEAD request against #SummerWebBackend:url.
 * function will not return anything - instead, connect to the 
 * #SummerWebBackend::headers-parsed signal.
 */
void
summer_web_backend_fetch_head (SummerWebBackend *self)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (self));

	SoupMessage *msg = soup_message_new ("HEAD", self->priv->url);

	g_signal_connect (msg, "got-headers", G_CALLBACK (on_got_headers), self);
	soup_session_queue_message (session, msg, NULL, NULL);
}
