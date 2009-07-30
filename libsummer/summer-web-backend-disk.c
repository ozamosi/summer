/* summer-web-backend-disk.c */

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

#include "summer-web-backend-disk.h"

/**
 * SECTION:summer-web-backend-disk
 * @short_description: Downloads files to disk
 * @stability: Private
 * @include: libsummer/summer-web-backend-disk.h
 *
 * This component is only meant to be used by the downloaders. It contains
 * #SummerWebBackendDisk which downloads files to local disk.
 */

/**
 * SummerWebBackendDisk:
 *
 * An class that downloads HTTP resources to disk. Local I/O is synchronous.
 */

struct _SummerWebBackendDiskPrivate
{
	gchar *save_dir;
	const gchar *filename;
	GFileOutputStream *outfile;
	GFile *filehandle;
};

#define WEB_BACKEND_DISK_PRIVATE(o) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SUMMER_TYPE_WEB_BACKEND_DISK, SummerWebBackendDiskPrivate))

enum {
	PROP_0,
	PROP_SAVE_DIR
};

G_DEFINE_TYPE (SummerWebBackendDisk, summer_web_backend_disk, SUMMER_TYPE_WEB_BACKEND)

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (object)->priv;
	switch (prop_id) {
	case PROP_SAVE_DIR:
		if (priv->save_dir)
			g_free (priv->save_dir);
		priv->save_dir = g_value_dup_string (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (object)->priv;
	switch (prop_id) {
	case PROP_SAVE_DIR:
		g_value_set_string (value, priv->save_dir);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
summer_web_backend_disk_finalize (GObject *object)
{
	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (object)->priv;
	if (G_IS_OUTPUT_STREAM (priv->outfile) && !g_output_stream_is_closed (
			G_OUTPUT_STREAM (priv->outfile))) {
		g_file_delete (priv->filehandle, NULL, NULL);
	}

	if (G_IS_OUTPUT_STREAM (priv->outfile)) {
		g_object_unref (priv->outfile);
	}

	if (G_IS_FILE (priv->filehandle)) {
		g_object_unref (priv->filehandle);
	}

	if (priv->filename != NULL)
		g_free ((gchar *)priv->filename);

	if (priv->save_dir != NULL)
		g_free (priv->save_dir);

	G_OBJECT_CLASS (summer_web_backend_disk_parent_class)->finalize (object);
}

static void
on_downloaded (SummerWebBackend *self, SoupSession *session, SoupMessage *msg)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND_DISK (self));
	g_return_if_fail (SOUP_IS_SESSION (session));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	g_return_if_fail (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code));

	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (self)->priv;
	g_return_if_fail (G_IS_OUTPUT_STREAM (priv->outfile));
	g_return_if_fail (!g_output_stream_is_closed (
		G_OUTPUT_STREAM (priv->outfile)));

	GError *e = NULL;
	g_output_stream_close (G_OUTPUT_STREAM (priv->outfile), NULL, &e);
	if (e) {
		GError *error = g_error_new (
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_LOCAL,
			"Couldn't complete download: %s", e->message);
		g_clear_error (&e);
		g_signal_emit_by_name (self, "download-complete", NULL, NULL, error);
		return;
	}

	g_object_unref (priv->outfile);

	gchar *filepath = g_file_get_path (priv->filehandle);

	g_signal_emit_by_name (self, "download-complete", filepath, NULL, NULL);

	g_free (filepath);
}

static gboolean
on_chunk (SummerWebBackend *self, SoupBuffer *chunk)
{
	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (self)->priv;
	GError *e = NULL;
	gsize written;
	g_output_stream_write_all (G_OUTPUT_STREAM (priv->outfile), chunk->data, chunk->length, &written, NULL, &e);
	if (e) {
		GError *error = g_error_new (
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_LOCAL,
			"Couldn't save download. Aborting. %s", e->message);
		g_clear_error (&e);
		g_signal_emit_by_name (self, "download-complete", NULL, NULL, error);
		summer_web_backend_abort (self);
		return FALSE;
	}
	return TRUE;
}

static void
on_init (SummerWebBackend *self, SoupMessage *msg, GError **error)
{
	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (self)->priv;

	gchar *filepath = g_build_filename (
		priv->save_dir,
		summer_web_backend_get_remote_filename (self),
		NULL);
	priv->filehandle = g_file_new_for_path (filepath);
	g_free (filepath);
	GError *e = NULL;
	GFile *directory = g_file_get_parent (priv->filehandle);
	if (!g_file_query_exists (directory, NULL)) {
		if (!g_file_make_directory_with_parents (directory, NULL, &e)) {
			g_set_error (error,
				SUMMER_WEB_BACKEND_ERROR,
				SUMMER_WEB_BACKEND_ERROR_LOCAL,
				"Error creating directory for download: %s",
					e->message);
			g_clear_error (&e);
			return;
		}
	}
	g_object_unref (directory);

	priv->outfile = g_file_replace (priv->filehandle, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &e);
	if (!priv->outfile) {
		g_set_error (error,
			SUMMER_WEB_BACKEND_ERROR,
			SUMMER_WEB_BACKEND_ERROR_LOCAL,
			"Error opening output stream: %s",
				e->message);
		g_clear_error (&e);
		return;
	}

	soup_message_body_set_accumulate (msg->response_body, FALSE);
}

static void
on_error (SummerWebBackend *self)
{
	/* Don't report error if one: we're only here because a critical error has
	 * already been sent */
	g_return_if_fail (SUMMER_IS_WEB_BACKEND_DISK (self));
	SummerWebBackendDiskPrivate *priv = SUMMER_WEB_BACKEND_DISK (self)->priv;

	if (G_IS_FILE (priv->filehandle)) {
		g_file_delete (priv->filehandle, NULL, NULL);
	}
}

static void
summer_web_backend_disk_class_init (SummerWebBackendDiskClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = get_property;
	gobject_class->set_property = set_property;
	gobject_class->finalize = summer_web_backend_disk_finalize;

	SummerWebBackendClass *wb_class = SUMMER_WEB_BACKEND_CLASS (klass);
	wb_class->on_downloaded = on_downloaded;
	wb_class->on_init = on_init;
	wb_class->on_chunk = on_chunk;
	wb_class->on_error = on_error;

	g_type_class_add_private (klass, sizeof (SummerWebBackendDiskPrivate));

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
}

static void
summer_web_backend_disk_init (SummerWebBackendDisk *self)
{
	self->priv = WEB_BACKEND_DISK_PRIVATE (self);
}

/**
 * summer_web_backend_disk_new:
 * @url: the url to download from.
 * @save_dir: the directory to save downloaded files to.
 *
 * Creates a new #SummerWebBackend that will save it's downloaded content to
 * disk.
 *
 * Returns: a newly created #SummerWebBackend object
 */
SummerWebBackend *
summer_web_backend_disk_new (const gchar *url, const gchar *save_dir)
{
	g_return_val_if_fail (url != NULL, NULL);
	g_return_val_if_fail (save_dir != NULL, NULL);
	return SUMMER_WEB_BACKEND (g_object_new (SUMMER_TYPE_WEB_BACKEND_DISK,
		"save-dir", save_dir,
		"url", url,
		NULL));
}
