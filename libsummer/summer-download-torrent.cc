/* summer-download-torrent.cc */

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

#include "summer-download-torrent.hh"
#include <libtorrent/session.hpp>

/**
 * SECTION:summer-download-torrent
 * @short_description: Provides a way to download audio and video files via
 * bittorrent.
 * @stability: Unstable
 * @see_also: %SummerDownloadWeb
 *
 * This is the downloader for bittorrent files.
 */

/**
 * SummerDownloadTorrent:
 *
 * A %SummerDownload-based class for files that's available on bittorrent.
 */

static void summer_download_torrent_class_init (SummerDownloadTorrentClass *klass);
static void summer_download_torrent_init       (SummerDownloadTorrent *obj);
static void summer_download_torrent_finalize   (GObject *obj);

struct _SummerDownloadTorrentPrivate {
	void* foo;
};
#define SUMMER_DOWNLOAD_TORRENT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                SUMMER_TYPE_DOWNLOAD_TORRENT, \
                                                SummerDownloadTorrentPrivate))

static libtorrent::session session;
static int session_refs = 0;

G_DEFINE_TYPE (SummerDownloadTorrent, summer_download_torrent, SUMMER_TYPE_DOWNLOAD);

static void
start (SummerDownload *dl)
{
}

static void
summer_download_torrent_class_init (SummerDownloadTorrentClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_download_torrent_finalize;

	SummerDownloadClass *download_class;
	download_class = SUMMER_DOWNLOAD_CLASS (klass);
	download_class->start = start;

	g_type_class_add_private (gobject_class, sizeof(SummerDownloadTorrentPrivate));
}

static void
summer_download_torrent_init (SummerDownloadTorrent *self)
{
	self->priv = SUMMER_DOWNLOAD_TORRENT_GET_PRIVATE(self);
}

static void
summer_download_torrent_finalize (GObject *obj)
{
	G_OBJECT_CLASS(summer_download_torrent_parent_class)->finalize (obj);
}

/**
 * summer_download_torrent_new:
 * @mime: the mime typ of the file that's going to be downloaded. This should be
 * available in the feed.
 * @url: the URL of the file that's going to be downloaded.
 *
 * Creates a new #SummerDownloadTorrent
 *
 * You're not supposed to call this function directly - instead, use the
 * factory %summer_create_download, which will go through all the downloaders,
 * looking for one that's suitable.
 *
 * Returns: a newly created #SummerDownloadTorrent object if @mime and @url is
 * suitable, otherwise %NULL.
 */
SummerDownload*
summer_download_torrent_new (gchar *mime, gchar *url)
{
	if (!g_strcmp0(mime, "application/x-bittorrent")) {
		return SUMMER_DOWNLOAD (g_object_new (SUMMER_TYPE_DOWNLOAD_TORRENT, 
			"url", url, NULL));
	}
	return NULL;
}
