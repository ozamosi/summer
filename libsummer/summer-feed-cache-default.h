/* summer-feed-cache-default.h */

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

#ifndef __SUMMER_FEED_CACHE_DEFAULT_H__
#define __SUMMER_FEED_CACHE_DEFAULT_H__

#include <glib-object.h>
#include "summer-data-types.h"

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED_CACHE_DEFAULT             (summer_feed_cache_default_get_type())
#define SUMMER_FEED_CACHE_DEFAULT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED_CACHE_DEFAULT,SummerFeedCacheDefault))
#define SUMMER_FEED_CACHE_DEFAULT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_FEED_CACHE_DEFAULT,SummerFeedCacheDefaultClass))
#define SUMMER_IS_FEED_CACHE_DEFAULT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED_CACHE_DEFAULT))
#define SUMMER_IS_FEED_CACHE_DEFAULT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_FEED_CACHE_DEFAULT))
#define SUMMER_FEED_CACHE_DEFAULT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_FEED_CACHE_DEFAULT,SummerFeedCacheDefaultClass))

typedef struct _SummerFeedCacheDefault        SummerFeedCacheDefault;
typedef struct _SummerFeedCacheDefaultClass   SummerFeedCacheDefaultClass;
typedef struct _SummerFeedCacheDefaultPrivate SummerFeedCacheDefaultPrivate;

struct _SummerFeedCacheDefault {
	 GObject parent;
	 SummerFeedCacheDefaultPrivate *priv;
};

struct _SummerFeedCacheDefaultClass {
	GObjectClass parent_class;
};

GType        summer_feed_cache_default_get_type    (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __SUMMER_FEED_CACHE_DEFAULT_H__ */
