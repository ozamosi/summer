/* summer-feed-cache.h */

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

#ifndef __SUMMER_FEED_CACHE_H__
#define __SUMMER_FEED_CACHE_H__

#include <glib-object.h>
#include "summer-data-types.h"

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED_CACHE                 (summer_feed_cache_get_type ())
#define SUMMER_FEED_CACHE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED_CACHE,SummerFeedCache))
#define SUMMER_IS_FEED_CACHE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED_CACHE))
#define SUMMER_FEED_CACHE_GET_INTERFACE(inst)  (G_TYPE_INSTANCE_GET_INTERFACE((inst),SUMMER_TYPE_FEED_CACHE,SummerFeedCacheInterface))

typedef struct _SummerFeedCache           SummerFeedCache; /* dummy object */
typedef struct _SummerFeedCacheInterface  SummerFeedCacheInterface;

struct _SummerFeedCacheInterface
{
	GTypeInterface parent_iface;
	void (*filter_items) (SummerFeedCache *self, GList **items);
	void (*add_new_item) (SummerFeedCache *self, SummerItemData *item);
};

GType summer_feed_cache_get_type (void);

void summer_feed_cache_filter_items (SummerFeedCache *self, GList **items);
void summer_feed_cache_add_new_item (SummerFeedCache *self, SummerItemData *item);

SummerFeedCache* summer_feed_cache_get (void);
void summer_feed_cache_set (SummerFeedCache *obj);

G_END_DECLS

#endif /* __SUMMER_FEED_CACHE_H__ */
