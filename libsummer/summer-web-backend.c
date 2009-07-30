/* summer-web-backend.c */

/* This file is part of libsummer.
 * Copyright © 2008-2009 Robin Sonefors <ozamosi@flukkost.nu>
 * 
 * Libsummer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2 as published by the Free Software Foundation.
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
#include <string.h>

/**
 * SECTION:summer-web-backend
 * @short_description: Provides a base class for communicating with web servers
 * @stability: Private
 * @include: libsummer/summer-web-backend.h
 *
 * This component is only meant to be used by the downloaders and the feed
 * fetchers. It contains #SummerWebBackend - a base class for communicating with
 * web servers.
 *
 * There are two subclasses, #SummerWebBackendDisk and #SummerWebBackendRam. The
 * difference between them is that #SummerWebBackendDisk downloads a URL to a
 * specified directory, while #SummerWebBackendRam downloads to RAM and is thus
 * only suitable for small, temparary files.
 */

/**
 * SummerWebBackend:
 *
 * An abstract class for web communication. All remote data is fetched
 * asynchronously.
 */

static void summer_web_backend_class_init (SummerWebBackendClass *klass);
static void summer_web_backend_init       (SummerWebBackend *obj);
static void summer_web_backend_finalize   (GObject *obj);

struct _SummerWebBackendPrivate {
	gchar *url;
	gchar *remote_filename;
	guint64 length;
	guint64 received;
	gboolean fetch;
	SoupMessage *msg;
	gboolean head_emitted;
};
#define SUMMER_WEB_BACKEND_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                           SUMMER_TYPE_WEB_BACKEND, \
                                           SummerWebBackendPrivate))
static SoupSession *session = NULL;

enum {
	PROP_0,
	PROP_URL,
	PROP_REMOTE_FILENAME,
	PROP_LENGTH
};

G_DEFINE_ABSTRACT_TYPE (SummerWebBackend, summer_web_backend, G_TYPE_OBJECT);

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerWebBackendPrivate *priv = SUMMER_WEB_BACKEND (object)->priv;

	switch (prop_id) {
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
	case PROP_URL:
		g_value_set_string (value, priv->url);
		break;
	case PROP_REMOTE_FILENAME:
		g_value_set_string (value, priv->remote_filename);
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

	gchar *url = SUMMER_WEB_BACKEND (obj)->priv->url;
	g_return_val_if_fail (url != NULL && g_utf8_strlen (url, 1) != 0, obj);
	gchar** parts = g_strsplit (url, "/", 0);
	gchar** filename;
	for (filename = parts; *filename != NULL; filename++) {}
	if (!g_strcmp0 (*(--filename), ""))
		filename--;
	SUMMER_WEB_BACKEND (obj)->priv->remote_filename = g_strdup (*filename);
	g_strfreev (parts);

	return obj;
}

static gboolean
dummy_on_chunk (SummerWebBackend *self, SoupBuffer *chunk) {return TRUE;}
static void
dummy_on_init (SummerWebBackend *self, SoupMessage *msg, GError **error) {}
static void
dummy_on_error (SummerWebBackend *self) {}

static void
summer_web_backend_class_init (SummerWebBackendClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_web_backend_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;
	gobject_class->constructor = constructor;

	klass->on_init = dummy_on_init;
	klass->on_chunk = dummy_on_chunk;
	klass->on_error = dummy_on_error;

	g_type_class_add_private (gobject_class, sizeof(SummerWebBackendPrivate));

	GParamSpec *pspec;

	pspec = g_param_spec_string ("url",
		"URL",
		"Specifies a URL to download from.",
		NULL,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	g_object_class_install_property (gobject_class, PROP_URL, pspec);

	pspec = g_param_spec_string ("remote-filename",
		"Remote filename",
		"The name of the file on the remote server",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_REMOTE_FILENAME, pspec);

	pspec = g_param_spec_uint64 ("length",
		"Length",
		"The reported length of the file",
		0, G_MAXUINT64, 0,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_LENGTH, pspec);

	/**
	 * SummerWebBackend::download-complete:
	 * @web_backend: the #SummerWebBackend that emitted the signal
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
		summer_marshal_VOID__STRING_STRING_POINTER,
		G_TYPE_NONE,
		3,
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);

	/**
	 * SummerWebBackend::download-chunk:
	 * @web_backend: the #SummerWebBackend that emitted the signal
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
	 * @web_backend: the #SummerWebBackend that emitted the signal
	 * @user_data: user-provided data.
	 *
	 * Emitted when the headers of a transfer has been downloaded and
	 * transfered. Only emitted once per #SummerWebBackend instance, so if you
	 * call #summer_web_backend_fetch_head(), it will not be emitted during
	 * subsequent calls to #summer_web_backend_fetch_head() or
	 * #summer_web_backend_fetch()
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
	if (SOUP_IS_SESSION (session))
		g_object_ref (session);
	else {
		session = soup_session_async_new_with_options (
			"max-conns-per-host", 1,
			"user-agent", "libsummer/" VERSION " ",
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
	if (priv->url != NULL)
		g_free (priv->url);
	if (priv->remote_filename != NULL)
		g_free (priv->remote_filename);
	G_OBJECT_CLASS (summer_web_backend_parent_class)->finalize (self);
}

static void
on_downloaded (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_SESSION (session));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	g_return_if_fail (SUMMER_WEB_BACKEND_GET_CLASS (self)->on_downloaded != NULL);


	if (!SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		SUMMER_WEB_BACKEND_GET_CLASS (self)->on_error (self);
		GError *error = g_error_new (
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_REMOTE,
			"Download failed: %s", msg->reason_phrase);
		g_signal_emit_by_name (self, "download-complete", NULL, NULL, error);
		return;
	}

	SUMMER_WEB_BACKEND_GET_CLASS (self)->on_downloaded (self, session, msg);
}

static void
on_got_chunk (SoupMessage *msg, SoupBuffer *chunk, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	g_return_if_fail (SUMMER_WEB_BACKEND_GET_CLASS (self)->on_chunk != NULL);
	g_return_if_fail (SUMMER_WEB_BACKEND_GET_CLASS (self)->on_error != NULL);

	if (!self->priv->fetch)
		return;

	if (SUMMER_WEB_BACKEND_GET_CLASS (self)->on_chunk (self, chunk)) {
		self->priv->received += chunk->length;
		g_signal_emit_by_name (self, "download-chunk", self->priv->received, self->priv->length);
	} else {
		SUMMER_WEB_BACKEND_GET_CLASS (self)->on_error (self);
		summer_web_backend_abort (self);
	}
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
		return; //on_downloaded will send an error, so we shouldn't
	}
	else if (SOUP_STATUS_IS_REDIRECTION (msg->status_code))
		return;
	
	priv->fetch = TRUE;

	goffset length;
	length = soup_message_headers_get_content_length (msg->response_headers);
	if (length)
		priv->length = length;

	gchar *new_filename = get_filename (msg);
	if (new_filename != NULL) {
		if (priv->remote_filename) {
			g_free (priv->remote_filename);
		}
		priv->remote_filename = new_filename;
	}

	if (!priv->head_emitted) {
		priv->head_emitted = TRUE;
		g_signal_emit_by_name (self, "headers-parsed");
	}
}

/**
 * summer_web_backend_fetch:
 * @self: a #SummerWebBackend instance
 * @error: a #GError, or %NULL
 *
 * Starts a file transfer from #SummerWebBackend:url. Since the transfer is asynchronous, the
 * function will not return anything - instead, connect to the 
 * #SummerWebBackend::download-complete signal.
 */
void
summer_web_backend_fetch (SummerWebBackend *self, GError **error)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (self));
	SummerWebBackendPrivate *priv = self->priv;
	g_return_if_fail (SUMMER_WEB_BACKEND_GET_CLASS (self)->on_init != NULL);

	priv->msg = soup_message_new ("GET", priv->url);
	if (priv->msg == NULL) {
		g_set_error (error,
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_INPUT,
			"Could not parse URL: %s", priv->url);
		return;
	}

	GError *e = NULL;
	SUMMER_WEB_BACKEND_GET_CLASS (self)->on_init (self, priv->msg, &e);
	if (e != NULL) {
		g_propagate_error (&e, *error);
		return;
	}

	g_signal_connect (priv->msg, "got-chunk", G_CALLBACK (on_got_chunk), self);
	g_signal_connect (priv->msg, "got-headers", G_CALLBACK (on_got_headers), self);
	soup_session_queue_message (session, priv->msg, on_downloaded, self);
}

static void
on_head_done (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (user_data));
	g_return_if_fail (SUMMER_WEB_BACKEND_GET_CLASS (user_data)->on_error != NULL);

	SummerWebBackend *self = SUMMER_WEB_BACKEND (user_data);
	if (!SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		SUMMER_WEB_BACKEND_GET_CLASS (self)->on_error (self);
		GError *error = g_error_new (
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_REMOTE,
			"Download failed: %s", msg->reason_phrase);
		g_signal_emit_by_name (self, "download-complete", NULL, NULL, error);
		return;
	}
}

/**
 * summer_web_backend_fetch_head:
 * @self: a #SummerWebBackend instance
 * @error: a #GError instance, or %NULL
 *
 * Performs a HEAD request against #SummerWebBackend:url.
 * This function will not return anything - connect to the
 * #SummerWebBackend::headers-parsed signal to get the result.
 */
void
summer_web_backend_fetch_head (SummerWebBackend *self, GError **error)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (self));
	g_return_if_fail (!SOUP_IS_MESSAGE (self->priv->msg));

	SoupMessage *msg = soup_message_new ("HEAD", self->priv->url);
	if (msg == NULL) {
		g_set_error (error,
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_INPUT,
			"Could not parse URL: %s", self->priv->url);
		return;
	}

	g_signal_connect (msg, "got-headers", G_CALLBACK (on_got_headers), self);
	soup_session_queue_message (session, msg, on_head_done, self);
}

/**
 * summer_web_backend_abort:
 * @self: a #SummerWebBackend instance
 *
 * If there is a transfer in progress, abort it.
 */
void
summer_web_backend_abort (SummerWebBackend *self)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (self));
	if (SOUP_IS_MESSAGE (self->priv->msg)) {
		soup_session_cancel_message (session,
			self->priv->msg,
			SOUP_STATUS_CANCELLED);
	}
}

/**
 * summer_web_backend_get_remote_filename:
 * @self: a #SummerWebBackend object
 *
 * Returns the download's filename on the remote server. This is taken either
 * from the Content-Disposition header, or, if there is none, the URL itself.
 *
 * Returns: a filename
 */
gchar*
summer_web_backend_get_remote_filename (SummerWebBackend *self)
{
	gchar* ret;
	g_object_get (self, "remote-filename", &ret, NULL);
	return ret;
}

GQuark
summer_web_backend_error_quark (void)
{
	return g_quark_from_static_string ("summer-web-backend-error-quark");
}
