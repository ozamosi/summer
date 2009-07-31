/* summer-feed-cache.c */

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

#include "summer-feed-cache.h"
#include "summer-feed-cache-default.h"

/**
 * SECTION:summer-feed-cache
 * @short_description: Interface for implementing feed cachers
 * @stability: Unstable
 *
 * This component contains an interface for implementing feed cacher.
 *
 * A feed cacher is a class that will keep track of what feed items has
 * previously been processed, that is, make sure files are only downloaded once.
 * It will need to keep track of this information during a session, as well as
 * between restarts of the application.
 *
 * Applications may want to store extra information on a per-download basis, in
 * which case it could write a custom feed cacher to store this information
 * with the cache information.
 *
 * A feed cache object needs to be a singleton. The method
 * %summer_feed_cache_set exists to set the feed cache. This method needs to be
 * called right at the application startup, before any feeds have been processed
 * or downloads completed. All subsequent calls to %summer_feed_cache_get will
 * then return a pointer to this object. If %summer_feed_cache_set is not
 * called before %summer_feed_cache_get, %summer_feed_cache_get will create and
 * return an instance of the default feed cache.
 */

/**
 * SummerFeedCache:
 *
 * Interface for building feed cache objects.
 */

static SummerFeedCache *cache = NULL;

static void
summer_feed_cache_base_init (gpointer g_class)
{
	static gboolean is_initialized = FALSE;

	if (!is_initialized) {
		is_initialized = TRUE;
	}
}

GType
summer_feed_cache_get_type ()
{
	static GType iface_type = 0;
	if (iface_type == 0) {
		static const GTypeInfo info = {
			sizeof (SummerFeedCacheInterface),
			summer_feed_cache_base_init,
			NULL,
		};

		iface_type = g_type_register_static (G_TYPE_INTERFACE,
			"SummerFeedCache",
			&info,
			0);
	}

	return iface_type;
}

/**
 * summer_feed_cache_filter_items:
 * @self: a #SummerFeedCache object.
 * @items: a list of #SummerItemData objects to be filtered.
 *
 * From a list of #SummerItemData objects, this function removes all nodes that
 * have previously been seen.
 */
void
summer_feed_cache_filter_items (SummerFeedCache *self, GList **items)
{
	g_return_if_fail (SUMMER_IS_FEED_CACHE (self));

	SUMMER_FEED_CACHE_GET_INTERFACE (self)->filter_items (self, items);
}

/**
 * summer_feed_cache_add_new_item:
 * @self: the #SummerFeedCache object
 * @item: the item to add to the cache
 *
 * After a download is finnished, this function must be called, to make sure
 * that the download won't be redownloaded after the library has been
 * reinitialized.
 */
void
summer_feed_cache_add_new_item (SummerFeedCache *self, SummerItemData *item)
{
	g_return_if_fail (SUMMER_IS_FEED_CACHE (self));
	g_return_if_fail (SUMMER_IS_ITEM_DATA (item));

	SUMMER_FEED_CACHE_GET_INTERFACE (self)->add_new_item (self, item);
}

/**
 * summer_feed_cache_get:
 *
 * Returns the global #SummerFeedCache instance, or creates one if there isn't
 * one.
 *
 * Note that you should unref the returned object when you're done with it.
 *
 * Returns: The #SummerFeedCache object.
 */
SummerFeedCache*
summer_feed_cache_get ()
{
	if (cache == NULL)
		cache = SUMMER_FEED_CACHE(g_object_new(SUMMER_TYPE_FEED_CACHE_DEFAULT, NULL));
	// Give each client one reference each, and keep the first one to ourselves.
	// This means this object won't be destroyed if nobody's using it 
	// temporarily.
	g_object_ref (G_OBJECT (cache));
	return cache;
}

/**
 * summer_feed_cache_set:
 * @obj: an object that implements #SummerFeedCache
 *
 * Sets the feed cache implementation to use for this session. This can only be
 * used if there isn't already an implementation installed. As soon as you
 * start to use feeds or downloads, one will be automatically created, so if
 * wish to call this function, you need to do so directly at startup.
 */
void
summer_feed_cache_set (SummerFeedCache *obj)
{
	g_return_if_fail (SUMMER_IS_FEED_CACHE (obj));
	g_return_if_fail (cache == NULL);

	cache = obj;
}
