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
 * @stability: Private
 * @include: libsummer/summer-feed-parser.h
 *
 * This component provides a base class for parsing different types of web feeds
 * as well as a couple of datatypes to store the results.
 */

/**
 * SummerFeedParser:
 * 
 * A base class for parsing feeds. You should not use this class directly - look
 * at %SummerFeed instead.
 */

static void summer_feed_parser_class_init (SummerFeedParserClass *klass);
static void summer_feed_parser_init       (SummerFeedParser *obj);
static void summer_feed_parser_finalize   (GObject *self);

G_DEFINE_ABSTRACT_TYPE (SummerFeedParser, summer_feed_parser, G_TYPE_OBJECT);

static void
summer_feed_parser_class_init (SummerFeedParserClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_feed_parser_finalize;
}

static void
summer_feed_parser_init (SummerFeedParser *obj)
{}

static void
summer_feed_parser_finalize (GObject *self)
{
	G_OBJECT_CLASS (summer_feed_parser_parent_class)->finalize (self);
}

/**
 * summer_feed_parser_handle_node ():
 * @self: A SummerFeedParser instance.
 * @node: A xmlTextReader instance. This must be a element start tag 
 * (%XML_READER_TYPE_ELEMENT).
 * @feed: A %SummerFeedData to store the results in.
 * @is_item: A boolean that will be set to %TRUE if the tag is the start of an item.
 *
 * Handle a feed node. A feed node is a node that is not part of a post - that
 * is, a node that describes the global scope in a feed. If the node was
 * handled, @node will be pointing att the next element node upon return.
 *
 * Returns: %-1 if there was a XML error, %0 if we've reached the end of the 
 * XML node, %1 if the node wasn't handled, and %2 if the node was handled.
 */
gint 
summer_feed_parser_handle_node (SummerFeedParser* self, 
	xmlTextReaderPtr node, SummerFeedData *feed, gboolean *is_item)
{
	g_return_val_if_fail (SUMMER_IS_FEED_PARSER (self), -1);
	g_return_val_if_fail (xmlTextReaderNodeType (node) == XML_READER_TYPE_ELEMENT, -1);
	return SUMMER_FEED_PARSER_GET_CLASS (self)->handle_node (self, node, feed, is_item);
}

/**
 * summer_feed_parser_parse:
 * @parsers: an array of %SummerFeedParser objects
 * @num_parsers: the number of elements in @parsers
 * @reader: the xmlTextReader
 *
 * Parses a given XML reader stream, with the given parsers.
 *
 * Returns: a %SummerFeedData, representing the parsed feed.
 */

SummerFeedData*
summer_feed_parser_parse (SummerFeedParser **parsers, int num_parsers, xmlTextReaderPtr reader)
{
	SummerFeedData *feed_data = summer_feed_data_new ();
	if (reader != NULL) {
		int ret = 1, depth = -1;
		gboolean item = FALSE;
		while (ret > 0) {
			while (ret > 0 && xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT)
				ret = xmlTextReaderRead (reader);

			if (depth != -1 && depth >= xmlTextReaderDepth (reader)) {
				depth = -1;
				item = FALSE;
			}
			int i;
			for (i = 0; ret == 1 && i < num_parsers; i++)
				ret = summer_feed_parser_handle_node (parsers[i], reader, feed_data, &item);
			if (depth == -1 && item)
				depth = xmlTextReaderDepth (reader);
			if (ret == 1)
				ret = xmlTextReaderRead (reader);
		}
	}
	return feed_data;
}
