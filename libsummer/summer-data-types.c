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

/**
 * SECTION:summer-data-types
 * @short_description: Data types for handling feed data.
 * @stability: Unstable
 *
 * This component contains a few datatypes for storing feed data.
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
	return g_slice_new0 (SummerFeedData);
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
	return g_slice_new0 (SummerItemData);
}

/**
 * SummerDownloadableData:
 * @url: The URL to the downloadable file.
 * @mime: The MIME type of the downloadable file.
 * @length: The length of the file.
 *
 * A datatype for representing a downloadable file.
 */

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
	return g_slice_new0 (SummerDownloadableData);
}

/**
 * summer_feed_data_free ():
 * @data: A %SummerFeedData object.
 *
 * Free a %SummerFeedData object.
 */
void
summer_feed_data_free (SummerFeedData *data)
{
	if (data->title)
		g_free (data->title);
	if (data->description)
		g_free (data->description);
	if (data->id)
		g_free (data->id);
	if (data->web_url)
		g_free (data->web_url);
	if (data->author)
		g_free (data->author);
	GList *list;
	for (list = data->items; list != NULL; list = list->next) {
		summer_item_data_free ((SummerItemData *)list->data);
	}
	g_list_free (data->items);
	g_slice_free (SummerFeedData, data);
}

/**
 * summer_item_data_free ():
 * @data: A %SummerItemData object.
 *
 * Free a %SummerItemData object.
 */
void
summer_item_data_free (SummerItemData *data)
{
	if (data->title)
		g_free (data->title);
	if (data->description)
		g_free (data->description);
	if (data->id)
		g_free (data->id);
	if (data->web_url)
		g_free (data->web_url);
	if (data->author)
		g_free (data->author);
	GList *list;
	for (list = data->downloadables; list != NULL; list = list->next) {
		summer_downloadable_data_free ((SummerDownloadableData *)list->data);
	}
	g_list_free (data->downloadables);
	g_slice_free (SummerItemData, data);
}

/**
 * summer_downloadable_data_free ():
 * @data: The %SummerDownloadableData object to free. 
 *
 * Free a %SummerDownloadableData object.
 */
void
summer_downloadable_data_free (SummerDownloadableData *data)
{
	g_free (data->url);
	g_free (data->mime);
	g_slice_free (SummerDownloadableData, data);
}
