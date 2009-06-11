/* summer-atom-parser.c */

/* This file is part of libsummer.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
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

#include "summer-atom-parser.h"

/**
 * SECTION:summer-atom-parser
 * @short_description: A #SummerFeedParser that parses Atom feeds
 * @stability: Unstable
 * @include: libsummer/summer-atom-parser.h
 *
 * A #SummerFeedParser that implements an Atom Syndication Format (RFC 4287)
 * parser. It is pretty liberal in it's interprentation of the standard.
 */

/**
 * SummerAtomParser:
 *
 * An Atom parser.
 */
static gchar* handled_namespaces[] = {
	"http://www.w3.org/2005/Atom",
	"http://purl.org/atom/ns#",
	NULL};

static void summer_atom_parser_class_init (SummerAtomParserClass *klass);
static void summer_atom_parser_init       (SummerAtomParser *obj);
static void summer_atom_parser_finalize   (GObject *obj);

static gint handle_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *data, gboolean *is_item);

G_DEFINE_TYPE (SummerAtomParser, summer_atom_parser, SUMMER_TYPE_FEED_PARSER);

static void
summer_atom_parser_class_init (SummerAtomParserClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_atom_parser_finalize;

	SummerFeedParserClass *feed_parser_class;
	feed_parser_class = SUMMER_FEED_PARSER_CLASS (klass);
	feed_parser_class->handle_node = handle_node;
}

static void
summer_atom_parser_init (SummerAtomParser *obj)
{}

static void
summer_atom_parser_finalize (GObject *obj)
{
	G_OBJECT_CLASS(summer_atom_parser_parent_class)->finalize (obj);
}

/**
 * summer_atom_parser_new ():
 *
 * Create a new #SummerAtomParser.
 *
 * Returns: The newly created #SummerAtomParser.
 */
SummerAtomParser*
summer_atom_parser_new (void)
{
	return SUMMER_ATOM_PARSER(g_object_new(SUMMER_TYPE_ATOM_PARSER, NULL));
}

static gint
handle_feed_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *feed, gboolean *is_item)
{
	gint ret = 1;

	SAVE_TEXT_CONTENTS ("title", node, feed->title, ret);
	SAVE_TEXT_CONTENTS ("subtitle", node, feed->description, ret);
	SAVE_TEXT_CONTENTS ("id", node, feed->id, ret);
	SAVE_TEXT_CONTENTS ("name", node, feed->author, ret);
	DO_WITH_TEXT_CONTENTS ("updated", node, ret, 
		GTimeVal time;
		g_time_val_from_iso8601 (text, &time);
		feed->updated = time.tv_sec);
	
	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST ("link"))) {
		int r = 1;
		gboolean html = TRUE;
		while (r > 0) {
			r = xmlTextReaderMoveToNextAttribute (node);
			if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node), 
					BAD_CAST ("type")))
				if (!xmlStrstr (xmlTextReaderConstValue (node), 
						BAD_CAST ("html"))) {
					html = FALSE;
					break;
				}
		}
		r = 1;
		xmlTextReaderMoveToElement (node);
		while (html && r > 0) {
			r = xmlTextReaderMoveToNextAttribute (node);
			if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node),
					BAD_CAST ("href"))) {
				feed->web_url = (gchar*) xmlTextReaderValue (node);
				ret = 2;
				break;
			}
		}
		if (!ret)
			xmlTextReaderMoveToElement (node);
	}

	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST ("entry"))) {
		*is_item = TRUE;
		summer_feed_data_append_item (feed);
	}

	return ret;
}

static gint
handle_item_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *feed)
{
	SummerItemData *item = (SummerItemData *)feed->items->data;
	gint ret = 1;
	
	SAVE_TEXT_CONTENTS ("title", node, item->title, ret);
	SAVE_TEXT_CONTENTS ("summary", node, item->description, ret);
	SAVE_TEXT_CONTENTS ("id", node, item->id, ret);
	DO_WITH_TEXT_CONTENTS ("updated", node, ret, 
		GTimeVal time;
		g_time_val_from_iso8601 (text, &time);
		item->updated = time.tv_sec);
	
	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST ("link"))) {
		int r = 1;
		gboolean html = TRUE;
		while (r > 0) {
			r = xmlTextReaderMoveToNextAttribute (node);
			if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node), 
					BAD_CAST ("type")))
				if (!xmlStrstr (xmlTextReaderConstValue (node), 
						BAD_CAST ("html"))) {
					html = FALSE;
					break;
				}
		}
		r = 1;
		xmlTextReaderMoveToElement (node);
		while (html && r > 0) {
			r = xmlTextReaderMoveToNextAttribute (node);
			if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node),
					BAD_CAST ("href"))) {
				item->web_url = (gchar*) xmlTextReaderValue (node);
				ret = 2;
				break;
			}
		}
		if (!ret)
			xmlTextReaderMoveToElement (node);
	}

	return ret;
}

static gint
handle_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *data, gboolean *is_item)
{
	gboolean valid_ns = FALSE;
	gchar **ns;
	for (ns = handled_namespaces; *ns != NULL; ns++) {
		if (xmlStrEqual (xmlTextReaderConstNamespaceUri (node), BAD_CAST (*ns))) {
			valid_ns = TRUE;
			break;
		}
	}
	if (!valid_ns) {
		return 1;
	}
	if (*is_item)
		return handle_item_node (self, node, data);
	else
		return handle_feed_node (self, node, data, is_item);
}
