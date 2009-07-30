/* summer-download-youtube.c */

/* This file is part of libsummer.
 * Copyright © 2008-2009 Robin Sonefors <ozamosi@flukkost.nu>
 *
 * Inspiration taken from youtube-dl, 
 *     Copyright © 2006-2008 Ricardo Garcia Gonzalez
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

#include "summer-download-youtube.h"
#include "summer-web-backend.h"
#include "summer-web-backend-disk.h"
#include "summer-web-backend-ram.h"
#include "summer-feed-cache.h"
#include <glib.h>
#include <string.h>
#include <gio/gio.h>
#include <time.h>

/**
 * SECTION:summer-download-youtube
 * @short_description: Provides a way to download youtube videos
 * @stability: Unstable
 *
 * This is a special downloader for the youtube video sharing website.
 */

/**
 * SummerDownloadYoutube:
 *
 * A #SummerDownload-based class for youtube videos.
 */

static void summer_download_youtube_class_init (SummerDownloadYoutubeClass *klass);
static void summer_download_youtube_init       (SummerDownloadYoutube *obj);
static void summer_download_youtube_finalize   (GObject *obj);

#define NUM_FORMATS 4
static gchar *quality[NUM_FORMATS][2] =  {
	{"", "flv"},
	{"&fmt=6", "flv"},
	{"&fmt=18", "mp4"},
	{"&fmt=22", "mp4"}};

struct _SummerDownloadYoutubePrivate {
	gchar *t;
	gchar *v;
	gint quality;
	SummerWebBackend *web;
	time_t last_update;
};
#define SUMMER_DOWNLOAD_YOUTUBE_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                     SUMMER_TYPE_DOWNLOAD_YOUTUBE, \
                                                     SummerDownloadYoutubePrivate))

G_DEFINE_TYPE (SummerDownloadYoutube, summer_download_youtube, SUMMER_TYPE_DOWNLOAD);

static gchar*
create_youtube_url (SummerDownloadYoutube *self)
{
	return g_strdup_printf (
		"http://www.youtube.com/get_video?video_id=%s&t=%s%s",
		self->priv->v, self->priv->t, quality[self->priv->quality][0]);
}

static void
on_download_chunk (SummerWebBackend *web_backend, guint64 received, guint64 length, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (user_data));
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	SummerDownloadYoutubePrivate *priv = SUMMER_DOWNLOAD_YOUTUBE (self)->priv;
	time_t curr_time = time (NULL);
	if (priv->last_update == 0 || priv->last_update < curr_time) {
		priv->last_update = curr_time;
		g_signal_emit_by_name (self, "download-update", received, length);
	}
}

static void
on_headers_parsed (SummerWebBackend *web, gpointer data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (data));
	SummerDownloadYoutube *self = SUMMER_DOWNLOAD_YOUTUBE (data);
	guint64 length;
	gchar *url;
	g_object_get (web, "length", &length, "url", &url, NULL);
	SummerDownloadableData *dlable;
	g_object_get (self, "downloadable", &dlable, NULL);
	dlable->url = url;
	dlable->length = length;
	g_object_unref (dlable);

	g_signal_emit_by_name (self, "download-started");
}

static void
on_file_downloaded (SummerWebBackend *web, gchar *saved_path, gchar *saved_data, GError *error, gpointer data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (data));
	g_return_if_fail (saved_data == NULL);
	SummerDownload *self = SUMMER_DOWNLOAD (data);
	SummerDownloadYoutubePrivate *priv = SUMMER_DOWNLOAD_YOUTUBE (self)->priv;
	
	if (g_error_matches (error, SUMMER_WEB_BACKEND_ERROR, SUMMER_WEB_BACKEND_ERROR_REMOTE)) {
		priv->quality--;
		if (priv->quality < 0) {
			GError *error = g_error_new (
				SUMMER_DOWNLOAD_ERROR,
				SUMMER_DOWNLOAD_ERROR_INPUT,
				"Download Failed");
			g_signal_emit_by_name (self, "download-error", error);
			g_object_unref (self);
			g_object_unref (web);
			return;
		}
		
		SummerItemData *item;
		g_object_get (self, "item", &item, NULL);
		summer_download_set_filename (self, g_strdup_printf ("%s.%s",
			summer_item_data_get_title (item),
			quality[priv->quality][1]));
		g_object_unref (item);

		GError *e = NULL;
		g_object_set (web, 
			"url", create_youtube_url (SUMMER_DOWNLOAD_YOUTUBE (self)),
			NULL);
		summer_web_backend_fetch (web, &e);
		if (e != NULL) {
			g_signal_emit_by_name (self, "download-error", e);
			g_object_unref (self);
		}
		return;
	} else if (error != NULL) {
		g_signal_emit_by_name (self, "download-error", error);
		g_object_unref (self);
		return;
	}

	GFile *src = g_file_new_for_path (saved_path);
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
			g_object_unref (destdir);
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
	g_object_unref (item);

	g_object_unref (web);
	g_signal_emit_by_name (self, "download-complete");
	g_signal_emit_by_name (self, "download-done");
	g_object_unref (self);
}

static void
on_webpage_downloaded (SummerWebBackend *web, gchar *path, gchar *web_data,
	GError *error, gpointer data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (data));
	g_return_if_fail (web_data != NULL);
	SummerDownloadYoutube *self = SUMMER_DOWNLOAD_YOUTUBE (data);
	if (error != NULL) {
		GError *e = g_error_new (
			SUMMER_DOWNLOAD_ERROR,
			SUMMER_DOWNLOAD_ERROR_INPUT,
			"Could not download youtube webpage: %s", error->message);
		g_signal_emit_by_name (self, "download-error", e);
		g_clear_error (&error);
		g_object_unref (self);
		return;
	}
	GError *e = NULL;
	GRegex *t = g_regex_new (", \"t\": \"([^\"]+)\"", 0, 0, &e);
	if (e) {
		g_warning ("Error compiling regex: %s", e->message);
		g_clear_error (&e);
		g_object_unref (self);
		return;
	}
	GMatchInfo *info;
	g_regex_match (t, web_data, 0, &info);
	g_regex_unref (t);
	
	if (!g_match_info_matches (info)) {
		e = g_error_new (
			SUMMER_DOWNLOAD_ERROR,
			SUMMER_DOWNLOAD_ERROR_INPUT,
			"Not a youtube page");
		g_signal_emit_by_name (self, "download-error", e);
		g_object_unref (self);
		g_match_info_free (info);
		return;
	}
	self->priv->t = g_match_info_fetch (info, 1);
	self->priv->quality = NUM_FORMATS - 1;
	g_match_info_free (info);

	g_object_unref (web);
	gchar *save_dir;
	g_object_get (self, "tmp-dir", &save_dir, NULL);
	web = summer_web_backend_disk_new (
		create_youtube_url (self),
		save_dir);
	g_free (save_dir);

	SummerItemData *item;
	g_object_get (self, "item", &item, NULL);
	gchar *filename = g_strdup_printf ("%s.%s", 
		summer_item_data_get_title (item),
		quality[self->priv->quality][1]);
	g_object_set (self, "filename", filename, NULL);
	g_free (filename);
	g_object_unref (item);

	g_signal_connect (web, "download-chunk",
		G_CALLBACK (on_download_chunk), self);
	g_signal_connect (web, "download-complete", 
		G_CALLBACK (on_file_downloaded), self);
	g_signal_connect (web, "headers-parsed",
		G_CALLBACK (on_headers_parsed), self);

	summer_web_backend_fetch (web, &e);
	if (e != NULL) {
		g_signal_emit_by_name (self, "download-error", e);
		g_object_unref (self);
	}
}

static void
summer_download_youtube_abort (SummerDownload *self)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (self));
	SummerDownloadYoutubePrivate *priv = SUMMER_DOWNLOAD_YOUTUBE (self)->priv;
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
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (self));
	SummerDownloadYoutubePrivate *priv = SUMMER_DOWNLOAD_YOUTUBE (self)->priv;
	SummerItemData *item;
	g_object_get (self, "item", &item, NULL);
	priv->web = summer_web_backend_ram_new (summer_item_data_get_web_url (item));
	g_object_unref (item);
	g_signal_connect (priv->web, "download-complete", G_CALLBACK (on_webpage_downloaded), self);
	GError *e = NULL;
	summer_web_backend_fetch (priv->web, &e);
	if (e != NULL) {
		g_propagate_error (&e, *error);
		g_object_unref (self);
	}
}

static void
summer_download_youtube_class_init (SummerDownloadYoutubeClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_download_youtube_finalize;

	SummerDownloadClass *download_class;
	download_class = SUMMER_DOWNLOAD_CLASS (klass);
	download_class->abort = summer_download_youtube_abort;
	download_class->start = start;

	g_type_class_add_private (gobject_class, sizeof(SummerDownloadYoutubePrivate));
}

static void
summer_download_youtube_init (SummerDownloadYoutube *self)
{
	self->priv = SUMMER_DOWNLOAD_YOUTUBE_GET_PRIVATE(self);
}

static void
summer_download_youtube_finalize (GObject *obj)
{
	G_OBJECT_CLASS(summer_download_youtube_parent_class)->finalize (obj);
}

/**
 * summer_download_youtube_new:
 * @item: a SummerItemData, containing information about the download.
 *
 * Creates a new #SummerDownloadYoutube.
 *
 * You're not supposed to call this function directly - instead, use the
 * factory %summer_create_download, which will go through all downloaders,
 * looking for one that's suitable.
 *
 * Returns: a newly created #SummerDownloadYoutube object if the item's mime
 * and url is suitable, otherwise %NULL.
 */
SummerDownload*
summer_download_youtube_new (SummerItemData *item)
{
	if (item->web_url == NULL)
		return NULL;

	GError *error = NULL;
	GRegex *v = g_regex_new ("^((?:http://)?(?:\\w+\\.)?youtube\\.com/(?:v/|(?:watch(?:\\.php)?)?\\?(?:.+&)?v=))?([0-9A-Za-z_-]+)(?(1)[&/].*)?$", 0, 0, &error);
	if (error) {
		g_warning ("Error compiling regex: %s", error->message);
		g_clear_error (&error);
		return NULL;
	}

	GMatchInfo *info;
	g_regex_match (v, item->web_url, 0, &info);
	
	if (g_match_info_matches (info)) {
		g_regex_unref (v);
		SummerDownloadYoutube *dl;
		SummerDownloadableData *dlable = summer_item_data_append_downloadable (
			item, NULL, NULL, 0);
		dl = g_object_new (SUMMER_TYPE_DOWNLOAD_YOUTUBE, 
			"item", item, "downloadable", dlable, NULL);
		g_object_unref (dlable);
		dl->priv->v = g_match_info_fetch (info, 2);
		g_match_info_free (info);
		return SUMMER_DOWNLOAD (dl);
	}
	g_match_info_free (info);
	
	return NULL;
}
