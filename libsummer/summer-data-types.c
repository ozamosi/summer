/* summer-data-types.c */

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

#include "summer-data-types.h"

static void summer_feed_data_class_init (SummerFeedDataClass *klass);
static void summer_item_data_class_init (SummerItemDataClass *klass);
static void summer_downloadable_data_class_init (SummerDownloadableDataClass *klass);

static void summer_feed_data_init (SummerFeedData *self);
static void summer_item_data_init (SummerItemData *self);
static void summer_downloadable_data_init (SummerDownloadableData *self);

static void summer_feed_data_finalize   (GObject *obj);
static void summer_item_data_finalize   (GObject *obj);
static void summer_downloadable_data_finalize   (GObject *obj);

/**
 * SECTION:summer-data-types
 * @short_description: Data types for handling feed data.
 * @stability: Unstable
 *
 * This component contains a few datatypes for storing feed data. Their 
 * relations are as follows:
 *
 * A %SummerFeedData represents a complete feed and contains global data about
 * it. It contains a list of several %SummerItemData objects.
 *
 * A %SummerItemData object represents an item, also known as a post or an
 * entry, in a feed. It contains a list of several %SummerDownloadableData
 * objects.
 *
 * A %SummerDownloadableData object represents something that a feed parser
 * thinks may be something worth downloading. Most commonly, this represents an
 * <enclosure/> tag in RSS2, and is some kind of multimedia file. It is not 
 * uncommon that one item contains several of these, where each differ for
 * instance in quality, format, or they may be the same, expressed in several 
 * different vocabularies.
 */

/**
 * SummerFeedData:
 * @title: The feed title
 * @description: A text string that is longer than the title. Some formats
 * call this a subtitle.
 * @id: A unique identifier for the feed.
 * @web_url: A URL where the same content is available as a web page.
 * @author: The author of the feed.
 * @updated: The last time the feed was updated.
 * @items: A list containing a %SummerItemData objects for each item in the 
 * feed.
 *
 * A type to store feed global data in.
 */

G_DEFINE_TYPE (SummerFeedData, summer_feed_data, G_TYPE_OBJECT);

static void
summer_feed_data_class_init (SummerFeedDataClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_feed_data_finalize;
}

static void
summer_feed_data_init (SummerFeedData *self)
{}

/**
 * summer_feed_data_new ():
 *
 * Create a new %SummerFeedData
 *
 * Returns: the newly created %SummerFeedData object.
 */
SummerFeedData*
summer_feed_data_new ()
{
	return SUMMER_FEED_DATA (g_object_new (SUMMER_TYPE_FEED_DATA, NULL));
}

static void
summer_feed_data_finalize (GObject *obj)
{
	SummerFeedData *self = SUMMER_FEED_DATA (obj);
	if (self->title)
		g_free (self->title);
	if (self->description)
		g_free (self->description);
	if (self->id)
		g_free (self->id);
	if (self->web_url)
		g_free (self->web_url);
	if (self->author)
		g_free (self->author);
	GList *list;
	for (list = self->items; list != NULL; list = list->next) {
		g_object_unref (G_OBJECT (list->data));
	}
	g_list_free (self->items);

	G_OBJECT_CLASS (summer_feed_data_parent_class)->finalize (obj);
}

/**
 * summer_feed_data_get_title:
 * @self: a %SummerFeedData object.
 *
 * Returns the title of the feed that this object represents. This string
 * should not be freed.
 *
 * Returns: The title of the feed.
 */
gchar *
summer_feed_data_get_title (SummerFeedData *self)
{
	return self->title;
}

/**
 * summer_feed_data_get_description:
 * @self: a %SummerFeedData object.
 *
 * Returns the description of the feed that this object represents. This 
 * string should not be freed.
 *
 * Returns: The description of the feed.
 */
gchar *
summer_feed_data_get_description (SummerFeedData *self)
{
	return self->description;
}

/**
 * summer_feed_data_get_id:
 * @self: a %SummerFeedData object.
 *
 * Returns a unique identifier for the feed that this object represents. This 
 * string should not be freed.
 *
 * Returns: The title of the feed.
 */
gchar *
summer_feed_data_get_id (SummerFeedData *self)
{
	return self->id;
}

/**
 * summer_feed_data_get_web_url:
 * @self: a %SummerFeedData object.
 *
 * Returns a URL, pointing to a web resource for the feed that this object 
 * represents. This string should not be freed.
 *
 * Returns: A URL to a web representation of the feed.
 */
gchar *
summer_feed_data_get_web_url (SummerFeedData *self)
{
	return self->web_url;
}

/**
 * summer_feed_data_get_author:
 * @self: a %SummerFeedData object.
 *
 * Returns the author of the feed that this object represents. This 
 * string should not be freed.
 *
 * Returns: The author of the feed.
 */
gchar *
summer_feed_data_get_author (SummerFeedData *self)
{
	return self->author;
}

/**
 * summer_feed_data_get_updated:
 * @self: a %SummerFeedData object.
 *
 * Returns when, in seconds since the epoch, this feed was most recently
 * updated.
 *
 * Returns: The last time this feed was updated.
 */
time_t
summer_feed_data_get_updated (SummerFeedData *self)
{
	return self->updated;
}

/**
 * summer_feed_data_get_items:
 * @self: a %SummerFeedData object.
 *
 * Returns all items in this feed. This list should not be freed.
 *
 * Returns: The items in the feed.
 */
GList *
summer_feed_data_get_items (SummerFeedData *self)
{
	return self->items;
}

/**
 * SummerItemData:
 * @title: The item title
 * @description: A longer text string. This is the text content of the post.
 * @id: A unique identifier for the post.
 * @web_url: A URL where the same content is available as a web page.
 * @author: The author of the post.
 * @updated: The last time the post was updated.
 * @downloadables: A list of %SummerDownloadableData objects for each 
 * downloadable file found in this item. All of these are usually the same 
 * content, but may differ in quality or format.
 *
 * A feed consists of several items. Each item can be represented by a
 * %SummerItemData object.
 */

G_DEFINE_TYPE (SummerItemData, summer_item_data, G_TYPE_OBJECT);

static void
summer_item_data_class_init (SummerItemDataClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_item_data_finalize;
}

static void
summer_item_data_init (SummerItemData *self)
{}

/**
 * summer_item_data_new ():
 *
 * Create a new %SummerItemData
 *
 * Returns: the newly created %SummerItemData
 */
SummerItemData*
summer_item_data_new ()
{
	return SUMMER_ITEM_DATA (g_object_new (SUMMER_TYPE_ITEM_DATA, NULL));
}

void
summer_item_data_finalize (GObject *obj)
{
	SummerItemData *self = SUMMER_ITEM_DATA (obj);
	if (self->title)
		g_free (self->title);
	if (self->description)
		g_free (self->description);
	if (self->id)
		g_free (self->id);
	if (self->web_url)
		g_free (self->web_url);
	if (self->author)
		g_free (self->author);
	GList *list;
	for (list = self->downloadables; list != NULL; list = list->next) {
		g_object_unref (G_OBJECT (list->data));
	}
	g_list_free (self->downloadables);
	
	G_OBJECT_CLASS (summer_item_data_parent_class)->finalize (obj);
}

/**
 * summer_item_data_get_title:
 * @self: a %SummerItemData object.
 *
 * Returns the title of the item that this object represents. You should not
 * free the returned string.
 *
 * Returns: The title of the item.
 */
gchar *
summer_item_data_get_title (SummerItemData *self)
{
	return self->title;
}

/**
 * summer_item_data_get_description:
 * @self: a %SummerItemData object.
 *
 * Returns the description of the item that this object represents. You should
 * not free the returned string.
 *
 * Returns: The description of the item.
 */
gchar *
summer_item_data_get_description (SummerItemData *self)
{
	return self->description;
}

/**
 * summer_item_data_get_id:
 * @self: a %SummerItemData object.
 *
 * Returns a unique identifier for the item that this object represents. If the
 * item doesn't specify one, the web URL is used for that purpose instead.
 * You should not free the returned string.
 *
 * Returns: A unique identifier for the item.
 */
gchar *
summer_item_data_get_id (SummerItemData *self)
{
	return self->id;
}

/**
 * summer_item_data_get_web_url:
 * @self: a %SummerItemData object.
 *
 * Returns a URL to a web representation of the item that this object 
 * represents. You should not free the returned string.
 *
 * Returns: A link to a web representation of the item.
 */
gchar *
summer_item_data_get_web_url (SummerItemData *self)
{
	return self->web_url;
}

/**
 * summer_item_data_get_author:
 * @self: a %SummerItemData object.
 *
 * Returns the author of the item that this object represents. You should not
 * free the returned string.
 *
 * Returns: The author of the item.
 */
gchar *
summer_item_data_get_author (SummerItemData *self)
{
	return self->author;
}

/**
 * summer_item_data_get_updated:
 * @self: a %SummerItemData object.
 *
 * Returns when this item was most recently updated, in seconds since the epoch.
 *
 * Returns: When this item was most recently updated.
 */
time_t
summer_item_data_get_updated (SummerItemData *self)
{
	return self->updated;
}

/**
 * summer_item_data_get_downloadables:
 * @self: a %SummerItemData object.
 *
 * Returns a list of all the downloadables the item that this object represents
 * contains. You should not free the returned list.
 *
 * Returns: A list of downloadables.
 */
GList *
summer_item_data_get_downloadables (SummerItemData *self)
{
	return self->downloadables;
}

/**
 * SummerDownloadableData:
 * @url: The URL to the downloadable file.
 * @mime: The MIME type of the downloadable file.
 * @length: The length of the file.
 *
 * A datatype for representing a downloadable file.
 */

G_DEFINE_TYPE (SummerDownloadableData, summer_downloadable_data, G_TYPE_OBJECT);

static void
summer_downloadable_data_class_init (SummerDownloadableDataClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_downloadable_data_finalize;
}

static void
summer_downloadable_data_init (SummerDownloadableData *self)
{}

/**
 * summer_downloadable_data_new ():
 *
 * Creates a new %SummerDownloadableData
 *
 * Returns: the newly created %SummerDownloadableData
 */
SummerDownloadableData*
summer_downloadable_data_new ()
{
	return (SummerDownloadableData *) (g_object_new 
		(SUMMER_TYPE_DOWNLOADABLE_DATA, NULL));
}

void
summer_downloadable_data_finalize (GObject *obj)
{
	SummerDownloadableData *self = (SummerDownloadableData *)obj;
	g_free (self->url);
	g_free (self->mime);

	G_OBJECT_CLASS (summer_downloadable_data_parent_class)->finalize (obj);
}

/**
 * summer_downloadable_data_get_url:
 * @self: a %SummerItemData object.
 *
 * Returns the URL where this downloadable object can be downloaded from.
 * You should not free the returned string.
 *
 * Returns: The URL of the downloadable.
 */
gchar *
summer_downloadable_data_get_url (SummerDownloadableData *self)
{
	return self->url;
}

/**
 * summer_downloadable_data_get_mime:
 * @self: a %SummerItemData object.
 *
 * Returns the reported mime type of this downloadable object.
 * You should not free the returned string.
 *
 * Returns: The mime type of the downloadable.
 */
gchar *
summer_downloadable_data_get_mime (SummerDownloadableData *self)
{
	return self->mime;
}

/**
 * summer_downloadable_data_get_length:
 * @self: a %SummerItemData object.
 *
 * Returns the reported length of the downloadable object. This data is usually
 * in bytes, but since it's fetched from a web feed, it could be something else.
 *
 * Returns: The length of the downloadable.
 */
gint
summer_downloadable_data_get_length (SummerDownloadableData *self)
{
	return self->length;
}
