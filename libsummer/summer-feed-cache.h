/* summer-feed-cache.h */

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

#ifndef __SUMMER_FEED_CACHE_H__
#define __SUMMER_FEED_CACHE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED_CACHE             (summer_feed_cache_get_type())
#define SUMMER_FEED_CACHE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED_CACHE,SummerFeedCache))
#define SUMMER_FEED_CACHE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_FEED_CACHE,SummerFeedCacheClass))
#define SUMMER_IS_FEED_CACHE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED_CACHE))
#define SUMMER_IS_FEED_CACHE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_FEED_CACHE))
#define SUMMER_FEED_CACHE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_FEED_CACHE,SummerFeedCacheClass))

typedef struct _SummerFeedCache        SummerFeedCache;
typedef struct _SummerFeedCacheClass   SummerFeedCacheClass;
typedef struct _SummerFeedCachePrivate SummerFeedCachePrivate;

struct _SummerFeedCache {
	 GObject parent;
	 SummerFeedCachePrivate *priv;
};

struct _SummerFeedCacheClass {
	GObjectClass parent_class;
};

GType        summer_feed_cache_get_type    (void) G_GNUC_CONST;

SummerFeedCache*    summer_feed_cache_new         (gchar *cache_file);

void summer_feed_cache_filter_old_items (SummerFeedCache *self, GList **items);

G_END_DECLS

#endif /* __SUMMER_FEED_CACHE_H__ */
