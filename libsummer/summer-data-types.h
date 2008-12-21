/* summer-feed-parser.h */

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

#ifndef __SUMMER_DATA_TYPES_H__
#define __SUMMER_DATA_TYPES_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _SummerFeedData            SummerFeedData;
typedef struct _SummerItemData            SummerItemData;
typedef struct _SummerDownloadableData    SummerDownloadableData;

struct _SummerFeedData {
	gchar *title;
	gchar *description;
	gchar *id;
	gchar *web_url;
	gchar *author;
	time_t updated;
	GList *items;
};

struct _SummerItemData {
	gchar *title;
	gchar *description;
	gchar *id;
	gchar *web_url;
	gchar *author;
	time_t updated;
	GList *downloadables;
};

struct _SummerDownloadableData {
	gchar *url;
	gchar *mime;
	gint length;
};

SummerFeedData* summer_feed_data_new (void);
SummerItemData* summer_item_data_new (void);
SummerDownloadableData* summer_downloadable_data_new (void);

void summer_feed_data_free (SummerFeedData *data);
void summer_item_data_free (SummerItemData *data);
void summer_downloadable_data_free (SummerDownloadableData *data);

G_END_DECLS

#endif /* __SUMMER_DATA_TYPES_H__ */
