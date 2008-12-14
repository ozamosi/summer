/* summer-feed-parser.c */

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

#include "summer-feed-parser.h"

/**
 * SECTION:summer-feed-parser
 * @short_description: Provides utilities for parsing web feeds
 * @stability: Unstable
 * @include: libsummer/feed-parser.h
 *
 * This component provides an interface for parsing different types of web feeds
 * as well as a couple of datatypes to store the results.
 */

/**
 * SummerFeedParser:
 * 
 * An interface for parsing feeds.
 */

static void summer_feed_parser_base_init (gpointer g_class);

GType
summer_feed_parser_get_type (void)
{
	static GType my_type = 0;
	if (!my_type) {
		static const GTypeInfo my_info = {
			sizeof(SummerFeedParserInterface),
			summer_feed_parser_base_init,		/* base init */
			NULL,		/* base finalize */
			NULL,		/* class_init */
			NULL,		/* class finalize */
			NULL,		/* class data */
			0,
			0,		/* n_preallocs */
			NULL,		/* instance init */
		};
		my_type = g_type_register_static (G_TYPE_INTERFACE,
		                                  "SummerFeedParser",
		                                  &my_info, 0);
		g_type_interface_add_prerequisite (my_type, G_TYPE_OBJECT);
	}
	return my_type;
}

static void
summer_feed_parser_base_init (gpointer g_iface)
{
	static gboolean initialized = FALSE;
	if (!initialized) {
		GParamSpec *pspec;
		pspec = g_param_spec_string ("handled-namespace", 
			"Handled namespace",
			"A namespace that this object can handle",
			NULL, 
			G_PARAM_READABLE);
		pspec = g_param_spec_value_array ("handled-namespaces",
			"Handled namespaces",
			"An array of the namespaces that this object handles",
			pspec,
			G_PARAM_READABLE);
		g_object_interface_install_property (g_iface, pspec);

		initialized = TRUE;
	}
}

/**
 * summer_feed_parser_handle_feed_node ():
 * @self: A SummerFeedParser instance.
 * @node: A xmlTextReader instance. This must be a element start tag 
 * (%XML_READER_TYPE_ELEMENT).
 * @feed: A %SummerFeedData to store the results in.
 * @start_item: A boolean that will be set to %TRUE if the tag is the start of an item.
 *
 * Handle a feed node. A feed node is a node that is not part of a post - that
 * is, a node that describes the global scope in a feed. If the node was
 * handled, @node will be pointing att the next element node upon return.
 *
 * Returns: %0 if the node was unrecognized, %1 if the node was handled, and %-1
 * on error.
 */
gint 
summer_feed_parser_handle_feed_node (SummerFeedParser* self, 
	xmlTextReaderPtr node, SummerFeedData *feed, gboolean *start_item)
{
	g_return_val_if_fail (SUMMER_IS_FEED_PARSER (self), -1);
	g_return_val_if_fail (xmlTextReaderNodeType (node) == XML_READER_TYPE_ELEMENT, -1);
	gint ret;
	*start_item = FALSE;

	ret = SUMMER_FEED_PARSER_GET_INTERFACE (self)->handle_feed_node (self, node, feed, start_item);
	
	if (ret <= 0)
		return ret;

	while (xmlTextReaderNodeType (node) != XML_READER_TYPE_ELEMENT) {
		int r = xmlTextReaderRead (node);
		if (r <= 0)
			return -1;
	}
	return ret;
}

/**
 * summer_feed_parser_handle_item_node ():
 * @self: A SummerFeedParser instance.
 * @node: A xmlTextReader instance. This must be a element start tag (%XML_READER_TYPE_ELEMENT).
 * @item: A %SummerItemData to store the results in.
 *
 * Handle an item node. An item node is a node that is part of a post. If the 
 * node was handled, @node will be pointing att the next element node upon 
 * return.
 *
 * Returns: %0 if the node was unrecognized, %1 if the node was handled, and %-1
 * on error.
 */
gint
summer_feed_parser_handle_item_node (SummerFeedParser* self,
	xmlTextReaderPtr node, SummerItemData *item)
{
	g_return_val_if_fail (SUMMER_IS_FEED_PARSER (self), -1);
	g_return_val_if_fail (xmlTextReaderNodeType (node) == XML_READER_TYPE_ELEMENT, -1);
	gint ret;
	ret = SUMMER_FEED_PARSER_GET_INTERFACE (self)->handle_item_node (self, node, item);
	
	if (ret <= 0)
		return ret;

	while (xmlTextReaderNodeType (node) != XML_READER_TYPE_ELEMENT) {
		int r = xmlTextReaderRead (node);
		if (r <= 0)
			return -1;
	}
	return ret;
}

/**
 * SummerFeedData:
 * @title: The feed title
 * @description: A text string that is longer than the title. Some formats
 * call this a subtitle.
 * @id: A unique identifier for the feed.
 * @web_url: A URL where the same content is available as a web page.
 * @author: The author of the feed.
 * @updated: The last time the feed was updated.
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
 * summer_feed_data_free ():
 * @data: A %SummerFeedData object.
 *
 * Free a %SummerFeedData object.
 */
void
summer_feed_data_free (SummerFeedData *data)
{
	g_free (data->title);
	g_free (data->description);
	g_free (data->id);
	g_free (data->web_url);
	g_free (data->author);
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
	g_free (data->title);
	g_free (data->description);
	g_free (data->id);
	g_free (data->web_url);
	g_free (data->author);
	g_slice_free (SummerItemData, data);
}
