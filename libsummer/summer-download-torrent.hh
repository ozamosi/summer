/* summer-download-torrent.hh */

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

#ifndef __SUMMER_DOWNLOAD_TORRENT_H__
#define __SUMMER_DOWNLOAD_TORRENT_H__

#include "summer-download.h"

G_BEGIN_DECLS

#define SUMMER_TYPE_DOWNLOAD_TORRENT             (summer_download_torrent_get_type())
#define SUMMER_DOWNLOAD_TORRENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_DOWNLOAD_TORRENT,SummerDownloadTorrent))
#define SUMMER_DOWNLOAD_TORRENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_DOWNLOAD_TORRENT,SummerDownloadTorrentClass))
#define SUMMER_IS_DOWNLOAD_TORRENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_DOWNLOAD_TORRENT))
#define SUMMER_IS_DOWNLOAD_TORRENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_DOWNLOAD_TORRENT))
#define SUMMER_DOWNLOAD_TORRENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_DOWNLOAD_TORRENT,SummerDownloadTorrentClass))

typedef struct _SummerDownloadTorrent      SummerDownloadTorrent;
typedef struct _SummerDownloadTorrentClass SummerDownloadTorrentClass;
typedef struct _SummerDownloadTorrentPrivate SummerDownloadTorrentPrivate;

struct _SummerDownloadTorrent {
	 SummerDownload parent;
	 SummerDownloadTorrentPrivate *priv;
};

struct _SummerDownloadTorrentClass {
	SummerDownloadClass parent_class;
};

GType        summer_download_torrent_get_type    (void) G_GNUC_CONST;

SummerDownload* summer_download_torrent_new (gchar *url, gchar *mime);

G_END_DECLS

#endif /* __SUMMER_DOWNLOAD_TORRENT_H__ */
