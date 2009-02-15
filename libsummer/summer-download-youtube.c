/* summer-download-youtube.c */

/* This file is part of libsummer.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
 *
 * Inspiration taken from youtube-dl, 
 *     Copyright © 2006-2008 Ricardo Garcia Gonzalez
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

#include "summer-download-youtube.h"
#include "summer-web-backend.h"
#include <glib.h>
#include <string.h>
#include <gio/gio.h>

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
 * A %SummerDownload-based class for youtube videos.
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
on_download_chunk (SummerWebBackend *web_backend, gint received, gint length, gpointer user_data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (user_data));
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	g_signal_emit_by_name (self, "download-update", received, length);
}

static void
on_file_downloaded (SummerWebBackend *web, gchar *saved_path, gchar *saved_data, gpointer data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (data));
	g_return_if_fail (saved_data == NULL);
	SummerDownloadYoutube *self = SUMMER_DOWNLOAD_YOUTUBE (data);
	
	if (saved_path == NULL) {
		self->priv->quality--;
		if (self->priv->quality < 0) {
			g_signal_emit_by_name (self, "download-complete", NULL);
			g_object_unref (self);
			g_object_unref (web);
			return;
		}
		g_object_set (web, "url", create_youtube_url (self), NULL);
		summer_web_backend_fetch (web);
		return;
	}

	SummerItemData *item;
	g_object_get (self, "item", &item, NULL);
	gchar *filename = g_strdup_printf ("%s.%s",
		summer_item_data_get_title (item),
		quality[self->priv->quality][1]);
	g_object_unref (item);

	GFile *src = g_file_new_for_path (saved_path);
	gchar *destpath, *save_dir;
	g_object_get (self, "save-dir", &save_dir, NULL);
	destpath = g_build_filename (save_dir, filename, NULL);
	g_free (filename);

	GFile *dest = g_file_new_for_path (destpath);

	GError *error = NULL;
	GFile *destdir = g_file_new_for_path (save_dir);
	if (!g_file_query_exists (destdir, NULL)) {
		if (!g_file_make_directory_with_parents (destdir, NULL, &error)) {
			g_warning ("Error creating directory to put file in: %s", error->message);
			g_clear_error (&error);
		}
	}

	if (!g_file_move (src, dest, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error)) {
		g_warning ("Couldn't move file: %s", error->message);
		g_clear_error (&error);
	}
	g_object_unref (src);
	g_object_unref (dest);
	g_free (save_dir);
	g_object_unref (web);
	g_signal_emit_by_name (self, "download-complete", destpath);
}

static void
on_webpage_downloaded (SummerWebBackend *web, gchar *path, gchar *web_data, 
	gpointer data)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (data));
	g_return_if_fail (web_data != NULL);
	SummerDownloadYoutube *self = SUMMER_DOWNLOAD_YOUTUBE (data);
	GError *error = NULL;
	GRegex *t;
	t = g_regex_new (", \"t\": \"([^\"]+)\"", 0, 0, &error);
	if (error) {
		g_warning ("Error compiling regex: %s", error->message);
		g_clear_error (&error);
		g_object_unref (self);
		return;
	}
	GMatchInfo *info;
	g_regex_match (t, web_data, 0, &info);
	g_regex_unref (t);
	
	if (!g_match_info_matches (info)) {
		g_warning ("Not a youtube page. Content: %s", web_data);
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
	web = summer_web_backend_new (
		save_dir, 
		create_youtube_url (self));
	g_free (save_dir);

	g_signal_connect (web, "download-chunk",
		G_CALLBACK (on_download_chunk), self);
	g_signal_connect (web, "download-complete", 
		G_CALLBACK (on_file_downloaded), self);
	summer_web_backend_fetch (web);
}

static void
start (SummerDownload *self)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_YOUTUBE (self));
	SummerItemData *item;
	g_object_get (self, "item", &item, NULL);
	SummerWebBackend *web = summer_web_backend_new (NULL, 
		summer_item_data_get_web_url (item));
	g_object_unref (item);
	g_signal_connect (web, "download-complete", G_CALLBACK (on_webpage_downloaded), self);
	summer_web_backend_fetch (web);
}

static void
summer_download_youtube_class_init (SummerDownloadYoutubeClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_download_youtube_finalize;

	SummerDownloadClass *download_class;
	download_class = SUMMER_DOWNLOAD_CLASS (klass);
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
		dl = g_object_new (SUMMER_TYPE_DOWNLOAD_YOUTUBE, 
			"item", item, "downloadable", NULL, NULL);
		dl->priv->v = g_match_info_fetch (info, 2);
		g_match_info_free (info);
		return SUMMER_DOWNLOAD (dl);
	}
	g_match_info_free (info);
	
	return NULL;
}
