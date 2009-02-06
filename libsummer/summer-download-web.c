/* summer-download-web.c */

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

#include "summer-download-web.h"
#include "summer-web-backend.h"
#include "summer-debug.h"
#include <string.h>
#include <gio/gio.h>

/**
 * SECTION:summer-download-web
 * @short_description: Provides a way to download audio and video files from
 * regular web servers.
 * @stability: Unstable
 * @see_also: %SummerDownloadTorrent
 *
 * This is the downloader for regular web servers - that is, the one used by 
 * most podcasts and vidcasts.
 */

/**
 * SummerDownloadWeb:
 *
 * A %SummerDownload-based class for files that's available on regular web
 * servers.
 */

static void summer_download_web_class_init (SummerDownloadWebClass *klass);
static void summer_download_web_init       (SummerDownloadWeb *obj);
static void summer_download_web_finalize   (GObject *obj);

struct _SummerDownloadWebPrivate {
	SummerWebBackend *web;
};
#define SUMMER_DOWNLOAD_WEB_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                 SUMMER_TYPE_DOWNLOAD_WEB, \
                                                 SummerDownloadWebPrivate))

G_DEFINE_TYPE (SummerDownloadWeb, summer_download_web, SUMMER_TYPE_DOWNLOAD);

static void
on_download_chunk (SummerWebBackend *web_backend, gint received, gint length, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (user_data));
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	g_signal_emit_by_name (self, "download-update", received, length);
}

static void
on_download_complete (SummerWebBackend *web_backend, gchar *save_path, gchar *save_data, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (user_data));
	g_return_if_fail (save_data == NULL);
	g_return_if_fail (save_path != NULL); //FIXME: This is perfectly legal (connection failed, for instance), and should be handled in a proper way
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);

	GFile *src = g_file_new_for_path (save_path);
	gchar *destpath, *save_dir;
	g_object_get (self, "save-dir", &save_dir, NULL);
	gchar *final_filename;
	g_object_get (self, "filename", &final_filename, NULL);
	destpath = g_build_filename (save_dir, final_filename, NULL);
	g_free (final_filename);
	
	GFile *dest = g_file_new_for_path (destpath);
	GError *error = NULL;
	
	GFile *destdir = g_file_new_for_path (save_dir);
	if (!g_file_query_exists (destdir, NULL)) {
		if (!g_file_make_directory_with_parents (destdir, NULL, &error)) {
			g_warning ("Error creating directory to put file in: %s", error->message);
			g_clear_error (&error);
		}
	}
	g_object_unref (destdir);

	if (!g_file_move (src, dest, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error)) {
		g_warning ("%s", error->message);
		g_clear_error (&error);
	}
	g_object_unref (src);
	g_object_unref (dest);

	g_free (save_dir);
	g_signal_emit_by_name (self, "download-complete", destpath);
	g_object_unref (self);
}

static void
start (SummerDownload *self)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (self));
	SummerDownloadWebPrivate *priv = SUMMER_DOWNLOAD_WEB (self)->priv;

	gchar *filename, *save_dir;
	gsize length;
	g_object_get (web, "filename", &filename, "length", &length, NULL);
	g_object_set (self, "filename", filename, NULL);
	g_object_get (self, "save-dir", &save_dir, NULL);
	gchar *final_path = g_build_filename (save_dir, filename, NULL);

	g_signal_connect (priv->web, "download-chunk", G_CALLBACK (on_download_chunk), self);
	g_signal_connect (priv->web, "download-complete", G_CALLBACK (on_download_complete), self);
	gchar *url, *tmp_dir;
	g_object_get (self, "tmp-dir", &tmp_dir, "url", &url, NULL);
	summer_debug ("Downloading from %s to %s via %s\n", url, save_dir, tmp_dir);
	g_free (url);
	g_free (tmp_dir);
	g_free (save_dir);
	summer_web_backend_fetch (priv->web);
}

static GObject *
constructor (GType gtype, guint n_properties, GObjectConstructParam *properties)
{
	GObject *obj;
	GObjectClass *parent_class;
	parent_class = G_OBJECT_CLASS (summer_download_web_parent_class);
	obj = parent_class->constructor (gtype, n_properties, properties);
	
	gchar *url, *tmp_dir;
	g_object_get (obj, "tmp-dir", &tmp_dir, "url", &url, NULL);
	SummerDownloadWebPrivate *priv = SUMMER_DOWNLOAD_WEB (obj)->priv;
	priv->web = summer_web_backend_new (tmp_dir, url);
	g_free (tmp_dir);
	g_free (url);

	return obj;
}

static void
summer_download_web_class_init (SummerDownloadWebClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_download_web_finalize;
	gobject_class->constructor = constructor;

	SummerDownloadClass *download_class;
	download_class = SUMMER_DOWNLOAD_CLASS (klass);
	download_class->start = start;

	g_type_class_add_private (gobject_class, sizeof(SummerDownloadWebPrivate));
}

static void
summer_download_web_init (SummerDownloadWeb *self)
{
	self->priv = SUMMER_DOWNLOAD_WEB_GET_PRIVATE (self);
}

static void
summer_download_web_finalize (GObject *obj)
{
	if (G_IS_OBJECT (SUMMER_DOWNLOAD_WEB (obj)->priv->web))
		g_object_unref (SUMMER_DOWNLOAD_WEB (obj)->priv->web);
	G_OBJECT_CLASS(summer_download_web_parent_class)->finalize (obj);
}

/**
 * summer_download_web_new:
 * @mime: the mime type of the file that's going to be downloaded. This hould be
 * available in the feed.
 * @url: the URL of the file that's going to be downloaded.
 *
 * Creates a new #SummerDownloadWeb.
 *
 * You're not supposed to call this function directly - instead, use the
 * factory %summer_create_download, which will go through all downloaders,
 * looking for one that's suitable.
 *
 * Returns: a newly created #SummerDownloadWeb object if @mime and @url is
 * suitable, otherwise %NULL.
 */
SummerDownload*
summer_download_web_new (gchar *mime, gchar *url)
{
	// There are ridiculously many application/* mime types, and we want to
	// download quite a few of them - I'm going for limited blacklisting
	// for now.
	if (mime == NULL || ((
				(g_str_has_prefix (mime, "application")
				&& (strstr (mime, "xml") == NULL)
				&& g_strcmp0 (mime, "application/x-bittorrent"))
			|| g_str_has_prefix (mime, "video") 
			|| g_str_has_prefix (mime, "audio")) 
		&& g_str_has_prefix (url, "http"))) {
		return SUMMER_DOWNLOAD (g_object_new (SUMMER_TYPE_DOWNLOAD_WEB, 
			"url", url, NULL));
	}
	return NULL;
}
