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

#include <glib-object.h>

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED_DATA             (summer_feed_data_get_type())
#define SUMMER_FEED_DATA(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED_DATA,SummerFeedData))
#define SUMMER_IS_FEED_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED_DATA))

#define SUMMER_TYPE_ITEM_DATA             (summer_item_data_get_type())
#define SUMMER_ITEM_DATA(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_ITEM_DATA,SummerItemData))
#define SUMMER_IS_ITEM_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_ITEM_DATA))

#define SUMMER_TYPE_DOWNLOADABLE_DATA     (summer_downloadable_data_get_type())
#define SUMMER_DOWNLOADABLE_DATA(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_DOWNLOADABLE_DATA,SummerDownloadableData))
#define SUMMER_IS_DOWNLOADABLE_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_DOWNLOADABLE_DATA))


typedef struct _SummerFeedData                 SummerFeedData;
typedef struct _SummerFeedDataClass            SummerFeedDataClass;
typedef struct _SummerItemData                 SummerItemData;
typedef struct _SummerItemDataClass            SummerItemDataClass;
typedef struct _SummerDownloadableData         SummerDownloadableData;
typedef struct _SummerDownloadableDataClass    SummerDownloadableDataClass;

struct _SummerFeedData {
	GObject parent;
	gchar *url;
	gchar *title;
	gchar *description;
	gchar *id;
	gchar *web_url;
	gchar *author;
	time_t updated;
	GList *items;
};

struct _SummerFeedDataClass {
	GObjectClass parent_class;
};

struct _SummerItemData {
	GObject parent;
	SummerFeedData *feed;
	gchar *title;
	gchar *description;
	gchar *id;
	gchar *web_url;
	gchar *author;
	time_t updated;
	GList *downloadables;
};

struct _SummerItemDataClass {
	GObjectClass parent_class;
};

struct _SummerDownloadableData {
	GObject parent;
	SummerItemData *item;
	gchar *url;
	gchar *mime;
	guint64 length;
};

struct _SummerDownloadableDataClass {
	GObjectClass parent_class;
};

SummerFeedData* summer_feed_data_new (void);

GType summer_feed_data_get_type (void) G_GNUC_CONST;
GType summer_item_data_get_type (void) G_GNUC_CONST;
GType summer_downloadable_data_get_type (void) G_GNUC_CONST;

gchar* summer_feed_data_get_title (SummerFeedData *self);
gchar* summer_feed_data_get_description (SummerFeedData *self);
gchar* summer_feed_data_get_id (SummerFeedData *self);
gchar* summer_feed_data_get_web_url (SummerFeedData *self);
gchar* summer_feed_data_get_author (SummerFeedData *self);
time_t summer_feed_data_get_updated (SummerFeedData *self);
GList* summer_feed_data_get_items (SummerFeedData *self);
SummerItemData* summer_feed_data_append_item (SummerFeedData *self);

gchar* summer_item_data_get_title (SummerItemData *self);
gchar* summer_item_data_get_description (SummerItemData *self);
gchar* summer_item_data_get_id (SummerItemData *self);
gchar* summer_item_data_get_web_url (SummerItemData *self);
gchar* summer_item_data_get_author (SummerItemData *self);
time_t summer_item_data_get_updated (SummerItemData *self);
GList* summer_item_data_get_downloadables (SummerItemData *self);
SummerDownloadableData* summer_item_data_append_downloadable (SummerItemData *self, gchar *url, gchar *mime, guint64 length);

gchar* summer_downloadable_data_get_url (SummerDownloadableData *self);
gchar* summer_downloadable_data_get_mime (SummerDownloadableData *self);
guint64 summer_downloadable_data_get_length (SummerDownloadableData *self);

G_END_DECLS

#endif /* __SUMMER_DATA_TYPES_H__ */
