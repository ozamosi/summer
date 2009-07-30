/* summer-download-web.c */

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

#include "summer-download-web.h"
#include "summer-web-backend.h"
#include "summer-web-backend-disk.h"
#include "summer-feed-cache.h"
#include "summer-debug.h"
#include <string.h>
#include <time.h>
#include <gio/gio.h>

/**
 * SECTION:summer-download-web
 * @short_description: Provides a way to download audio and video files from
 * regular web servers.
 * @stability: Unstable
 * @see_also: #SummerDownloadTorrent
 *
 * This is the downloader for regular web servers - that is, the one used by 
 * most podcasts and vidcasts.
 */

/**
 * SummerDownloadWeb:
 *
 * A #SummerDownload-based class for files that's available on regular web
 * servers.
 *
 * Note that this class steals a reference and frees itself after the download
 * is completed - thus you don't have to unref it yourself.
 */

static void summer_download_web_class_init (SummerDownloadWebClass *klass);
static void summer_download_web_init       (SummerDownloadWeb *obj);
static void summer_download_web_finalize   (GObject *obj);

struct _SummerDownloadWebPrivate {
	SummerWebBackend *web;
	time_t last_update;
};
#define SUMMER_DOWNLOAD_WEB_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                 SUMMER_TYPE_DOWNLOAD_WEB, \
                                                 SummerDownloadWebPrivate))

G_DEFINE_TYPE (SummerDownloadWeb, summer_download_web, SUMMER_TYPE_DOWNLOAD);

static void
on_download_chunk (SummerWebBackend *web_backend, guint64 received, guint64 length, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (user_data));
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	SummerDownloadWebPrivate *priv = SUMMER_DOWNLOAD_WEB (self)->priv;
	time_t curr_time = time (NULL);
	if (priv->last_update == 0 || priv->last_update < curr_time) {
		priv->last_update = curr_time;
		g_signal_emit_by_name (self, "download-update", received, length);
	}
}

static void
on_download_complete (SummerWebBackend *web_backend, gchar *save_path, gchar *save_data, GError *error, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (user_data));
	g_return_if_fail (save_data == NULL);

	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	if (error != NULL) {
		g_signal_emit_by_name (self, "download-error", error);
		g_object_unref (self);
		return;
	}

	GFile *src = g_file_new_for_path (save_path);
	gchar *destpath = summer_download_get_save_path (self);
	GFile *dest = g_file_new_for_path (destpath);
	g_free (destpath);
	destpath = NULL;
	GError *e = NULL;

	gchar *save_dir = summer_download_get_save_dir (self);
	GFile *destdir = g_file_new_for_path (save_dir);
	g_free (save_dir);
	save_dir = NULL;
	if (!g_file_query_exists (destdir, NULL)) {
		if (!g_file_make_directory_with_parents (destdir, NULL, &e)) {
			GError *e2 = g_error_new (
				SUMMER_DOWNLOAD_ERROR,
				SUMMER_DOWNLOAD_ERROR_OUTPUT,
				"Couldn't create output directory: %s", e->message);
			g_signal_emit_by_name (self, "download-error", e2);
			g_object_unref (self);
			g_clear_error (&e);
			g_object_unref (src);
			g_object_unref (dest);
			return;
		}
	}
	g_object_unref (destdir);

	if (!g_file_move (src, dest, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &e)) {
		GError *e2 = g_error_new (
			SUMMER_DOWNLOAD_ERROR,
			SUMMER_DOWNLOAD_ERROR_OUTPUT,
			"Couldn't move download to it's final destination: %s",
			e->message);
		g_signal_emit_by_name (self, "download-error", e2);
		g_object_unref (self);
		g_clear_error (&e);
		g_object_unref (src);
		g_object_unref (dest);
		return;
	}
	g_object_unref (src);
	g_object_unref (dest);

	g_free (save_dir);

	SummerItemData *item;
	g_object_get (self, "item", &item, NULL);
	SummerFeedCache *cache = summer_feed_cache_get ();
	summer_feed_cache_add_new_item (cache, item);
	g_object_unref (G_OBJECT (cache));
	g_object_unref (G_OBJECT (item));

	g_signal_emit_by_name (self, "download-complete");
	g_signal_emit_by_name (self, "download-done");
	g_object_unref (self);
}

static gboolean
is_downloaded (gchar *filename, gsize reported_size)
{
	if (!filename)
		return FALSE;
	GFile *file = g_file_new_for_path (filename);
	if (g_file_query_exists (file, NULL)) {
		GError *e = NULL;
		GFileInfo *info = g_file_query_info (file,
			G_FILE_ATTRIBUTE_STANDARD_SIZE,
			G_FILE_QUERY_INFO_NONE,
			NULL,
			&e);
		if (e) {
			g_warning ("Error when checking filesize: %s", e->message);
			g_clear_error (&e);
			g_object_unref (file);
			return FALSE;
		}
		guint64 current_size = g_file_info_get_attribute_uint64 (info,
			G_FILE_ATTRIBUTE_STANDARD_SIZE);
		// If reported_size is 0, assume the file is downloaded
		if (current_size && (!reported_size || current_size == reported_size)) {
			summer_debug ("File %s already exists - not downloading", filename);
			g_object_unref (info);
			g_object_unref (file);
			return TRUE;
		} else {
			summer_debug ("Removing old copy of %s since it doesn't has the right size (is %i, should be %i)", filename, current_size, reported_size);
			g_file_delete (file, NULL, &e);
			if (e) {
				g_warning ("Error when deleting file: %s", e->message);
				g_clear_error (&e);
			}
			g_object_unref (info);
			g_object_unref (file);
			return FALSE;
		}
	}
	g_object_unref (file);
	return FALSE;
}

static void
on_headers_parsed (SummerWebBackend *web, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (user_data));
	g_return_if_fail (SUMMER_IS_WEB_BACKEND (web));
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	SummerDownloadWebPrivate *priv = SUMMER_DOWNLOAD_WEB (self)->priv;

	gchar *filename, *save_dir;
	guint64 length;
	g_object_get (web, "remote-filename", &filename, "length", &length, NULL);
	g_object_set (self, "filename", filename, NULL);
	g_object_get (self, "save-dir", &save_dir, NULL);
	if (length) {
		SummerDownloadableData *dlable;
		g_object_get (self, "downloadable", &dlable, NULL);
		dlable->length = length;
		g_object_unref (dlable);
	}
	gchar *final_path = g_build_filename (save_dir, filename, NULL);
	g_free (filename);
	filename = NULL;

	g_signal_connect (priv->web, "download-chunk", G_CALLBACK (on_download_chunk), self);
	
	g_signal_emit_by_name (self, "download-started");

	if (is_downloaded (final_path, length)) {
		g_signal_emit_by_name (self, "download-complete");
	} else {
		GError *error = NULL;
		summer_web_backend_fetch (priv->web, &error);
		if (error != NULL) {
			g_signal_emit_by_name (self, "download-error", error);
			g_object_unref (self);
			g_free (save_dir);
			g_free (final_path);
		}
	}

	g_free (save_dir);
	g_free (final_path);
}

static void
summer_download_web_abort (SummerDownload *self)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (self));
	SummerDownloadWebPrivate *priv = SUMMER_DOWNLOAD_WEB (self)->priv;
	summer_web_backend_abort (priv->web);

	SummerItemData *item;
	g_object_get (self, "item", &item, NULL);

	SummerFeedCache *cache = summer_feed_cache_get ();
	summer_feed_cache_add_new_item (cache, item);
	g_object_unref (G_OBJECT (cache));
	g_object_unref (G_OBJECT (item));
}

static void
start (SummerDownload *self, GError **error)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_WEB (self));
	SummerDownloadWebPrivate *priv = SUMMER_DOWNLOAD_WEB (self)->priv;
	g_signal_connect (priv->web, "headers-parsed", G_CALLBACK (on_headers_parsed), self);
	g_signal_connect (priv->web, "download-complete", G_CALLBACK (on_download_complete), self);
	GError *e = NULL;
	summer_web_backend_fetch_head (priv->web, &e);
	if (e != NULL) {
		g_propagate_error (&e, *error);
		g_object_unref (self);
	}
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
	priv->web = summer_web_backend_disk_new (url, tmp_dir);
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
	download_class->abort = summer_download_web_abort;
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
 * @item: a SummerItemData, containing information about the download.
 *
 * Creates a new #SummerDownloadWeb.
 *
 * You're not supposed to call this function directly - instead, use the
 * factory %summer_create_download, which will go through all downloaders,
 * looking for one that's suitable.
 *
 * Returns: a newly created #SummerDownloadWeb object if the item's mime and url
 * is suitable, otherwise %NULL.
 */
SummerDownload*
summer_download_web_new (SummerItemData *item)
{
	GList *dl;
	for (dl = summer_item_data_get_downloadables (item); dl != NULL;
			dl = dl->next) {
		SummerDownloadableData *downloadable;
		downloadable = SUMMER_DOWNLOADABLE_DATA (dl->data);
		gchar *mime = summer_downloadable_data_get_mime (downloadable);
		gchar *url = summer_downloadable_data_get_url (downloadable);
		// There are ridiculously many application/* mime types, and we want to
		// download quite a few of them - I'm going for limited blacklisting
		// for now.
		if ((mime == NULL || (
					(g_str_has_prefix (mime, "application")
					&& (strstr (mime, "xml") == NULL)
					&& g_strcmp0 (mime, "application/x-bittorrent"))
				|| g_str_has_prefix (mime, "video") 
				|| g_str_has_prefix (mime, "audio"))) 
			&& g_str_has_prefix (url, "http")) {
			return SUMMER_DOWNLOAD (g_object_new (SUMMER_TYPE_DOWNLOAD_WEB, 
				"item", item, "downloadable", downloadable, NULL));
		}
	}
	return NULL;
}
