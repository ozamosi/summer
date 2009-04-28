/* summer-download-youtube.h */

/* This file is part of libsummer.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
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

#ifndef __SUMMER_DOWNLOAD_YOUTUBE_H__
#define __SUMMER_DOWNLOAD_YOUTUBE_H__

#include "summer-download.h"
#include "summer-data-types.h"

G_BEGIN_DECLS

#define SUMMER_TYPE_DOWNLOAD_YOUTUBE             (summer_download_youtube_get_type())
#define SUMMER_DOWNLOAD_YOUTUBE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_DOWNLOAD_YOUTUBE,SummerDownloadYoutube))
#define SUMMER_DOWNLOAD_YOUTUBE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_DOWNLOAD_YOUTUBE,SummerDownloadYoutubeClass))
#define SUMMER_IS_DOWNLOAD_YOUTUBE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_DOWNLOAD_YOUTUBE))
#define SUMMER_IS_DOWNLOAD_YOUTUBE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_DOWNLOAD_YOUTUBE))
#define SUMMER_DOWNLOAD_YOUTUBE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_DOWNLOAD_YOUTUBE,SummerDownloadYoutubeClass))

typedef struct _SummerDownloadYoutube        SummerDownloadYoutube;
typedef struct _SummerDownloadYoutubeClass   SummerDownloadYoutubeClass;
typedef struct _SummerDownloadYoutubePrivate SummerDownloadYoutubePrivate;

struct _SummerDownloadYoutube {
	SummerDownload parent;
	SummerDownloadYoutubePrivate *priv;
};

struct _SummerDownloadYoutubeClass {
	SummerDownloadClass parent_class;
};

GType        summer_download_youtube_get_type    (void) G_GNUC_CONST;

SummerDownload*    summer_download_youtube_new (SummerItemData *item);

G_END_DECLS

#endif /* __SUMMER_DOWNLOAD_YOUTUBE_H__ */
