/* summer-feed.h */

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

#ifndef __SUMMER_FEED_H__
#define __SUMMER_FEED_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED             (summer_feed_get_type())
#define SUMMER_FEED(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED,SummerFeed))
#define SUMMER_FEED_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_FEED,SummerFeedClass))
#define SUMMER_IS_FEED(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED))
#define SUMMER_IS_FEED_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_FEED))
#define SUMMER_FEED_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_FEED,SummerFeedClass))

typedef struct _SummerFeed        SummerFeed;
typedef struct _SummerFeedClass   SummerFeedClass;
typedef struct _SummerFeedPrivate SummerFeedPrivate;

struct _SummerFeed {
	 GObject parent;
	 SummerFeedPrivate *priv;
};

struct _SummerFeedClass {
	GObjectClass parent_class;
	void (*new_entries) (SummerFeed *obj, gpointer user_data);
};

GType        summer_feed_get_type    (void) G_GNUC_CONST;

SummerFeed*    summer_feed_new         (void);

void summer_feed_start (SummerFeed *self, gchar *url);

void summer_feed_set_default (const gchar *cache_dir, const gint frequency);

gchar* summer_feed_get_cache_dir (SummerFeed *self);
void summer_feed_set_cache_dir (SummerFeed *self, gchar *cache_dir);

gint summer_feed_get_frequency (SummerFeed *self);
void summer_feed_set_frequency (SummerFeed *self, gint frequency);

gchar* summer_feed_get_url (SummerFeed *self);
void summer_feed_set_url (SummerFeed *self, gchar *url);

gchar* summer_feed_get_title (SummerFeed *self);

gchar* summer_feed_get_description (SummerFeed *self);

gchar* summer_feed_get_id (SummerFeed *self);

gchar* summer_feed_get_web_url (SummerFeed *self);

gchar* summer_feed_get_author (SummerFeed *self);

time_t summer_feed_get_updated (SummerFeed *self);

GList* summer_feed_get_items (SummerFeed *self);

void summer_feed_shutdown (void);
G_END_DECLS

#endif /* __SUMMER_FEED_H__ */
