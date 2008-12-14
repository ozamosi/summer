/* summer-atom-parser.c */

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

#include "summer-atom-parser.h"

/**
 * SECTION:summer-atom-parser
 * @short_description: A %summer-feed-parser that parses Atom feeds
 * @stability: Private
 * @include: libsummer/summer-atom-parser.h
 *
 * A %summer-feed-parser that implements an Atom Syndication Format (RFC 4287)
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
static void summer_feed_parser_interface_init (SummerFeedParserInterface *iface);

enum {
	PROP_,
	PROP_HANDLED_NAMESPACES
};

G_DEFINE_TYPE_WITH_CODE (SummerAtomParser, summer_atom_parser, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (SUMMER_TYPE_FEED_PARSER, 
		summer_feed_parser_interface_init));

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	GValueArray *array;
	switch (prop_id) {
	case PROP_HANDLED_NAMESPACES:
		array = g_value_array_new (0);
		gchar **ns;
		for (ns = handled_namespaces; *ns != NULL; ns++) {
			GValue tmp_val = {0};
			g_value_init (&tmp_val, G_TYPE_STRING);
			g_value_set_static_string (&tmp_val, *ns);
			array = g_value_array_append (array, &tmp_val);
		}

		g_value_set_boxed (value, array);
		g_value_array_free (array);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
summer_atom_parser_class_init (SummerAtomParserClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_atom_parser_finalize;
	gobject_class->get_property = get_property;

	g_object_class_override_property (gobject_class, PROP_HANDLED_NAMESPACES, "handled-namespaces");
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
 * Create a new %SummerAtomParser.
 *
 * Returns: The newly created %SummerAtomParser.
 */
SummerAtomParser*
summer_atom_parser_new (void)
{
	return SUMMER_ATOM_PARSER(g_object_new(SUMMER_TYPE_ATOM_PARSER, NULL));
}

#define SAVE_TEXT_CONTENTS(element_name, node, result) \
	if (!g_strcmp0 (xmlTextReaderConstLocalName (node), BAD_CAST (element_name))) { \
		while (xmlTextReaderNodeType (node) != XML_READER_TYPE_TEXT) { \
			gint ret; \
			ret = xmlTextReaderRead (node); \
			if (ret <= 0) \
				return -1; \
			result = xmlTextReaderValue (node); \
			return 1; \
		} \
	}

#define SAVE_ATTRIBUTE(element_name, attribute_name, node, result) \
	if (!g_strcmp0 (xmlTextReaderConstLocalName (node), BAD_CAST (element_name))) { \
		while (xmlTextReaderMoveToNextAttribute (node) > 0) { \
			if (!g_strcmp0 (xmlTextReaderConstLocalName (node), BAD_CAST (attribute_name))) { \
				result = xmlTextReaderValue (node); \
				return 1; \
			} \
		} \
		return -1; \
	}

#define DO_WITH_TEXT_CONTENTS(element_name, node, code) \
	if (!g_strcmp0 (xmlTextReaderConstLocalName (node), BAD_CAST (element_name))) { \
		while (xmlTextReaderNodeType (node) != XML_READER_TYPE_TEXT) { \
			gint ret; \
			ret = xmlTextReaderRead (node); \
			if (ret <= 0) \
				return -1; \
			gchar *text = xmlTextReaderValue (node); \
			code; \
			g_free (text); \
			return 1; \
		} \
	}

static gint
handle_feed_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *feed, gboolean *start_item)
{
	gboolean valid_ns = FALSE;
	gchar **ns;
	for (ns = handled_namespaces; *ns != NULL; ns++) {
		if (!g_strcmp0 (xmlTextReaderConstNamespaceUri (node), *ns)) {
			valid_ns = TRUE;
			break;
		}
	}
	if (!valid_ns)
		return 0;
	
	SAVE_TEXT_CONTENTS ("title", node, feed->title);
	SAVE_TEXT_CONTENTS ("subtitle", node, feed->description);
	SAVE_TEXT_CONTENTS ("id", node, feed->id);
	SAVE_TEXT_CONTENTS ("name", node, feed->author);
	DO_WITH_TEXT_CONTENTS ("updated", node, g_time_val_from_iso8601 (text, &feed->updated));
	SAVE_ATTRIBUTE ("link", "href", node, feed->web_url);

	if (!g_strcmp0 (xmlTextReaderConstLocalName (node), BAD_CAST ("entry"))) {
		*start_item = TRUE;
	}

	return 0;
}

static gint
handle_item_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerItemData *item)
{
	gboolean valid_ns = FALSE;
	gchar **ns;
	for (ns = handled_namespaces; *ns != NULL; ns++) {
		if (!g_strcmp0 (xmlTextReaderConstNamespaceUri (node), *ns)) {
			valid_ns = TRUE;
			break;
		}
	}
	if (!valid_ns) {
		return 0;
	}
	
	SAVE_TEXT_CONTENTS ("title", node, item->title);
	SAVE_TEXT_CONTENTS ("summary", node, item->description);
	SAVE_TEXT_CONTENTS ("id", node, item->id);
	DO_WITH_TEXT_CONTENTS ("updated", node, g_time_val_from_iso8601 (text, &item->updated));
	SAVE_ATTRIBUTE ("link", "href", node, item->web_url);


	return 0;
}

static void
summer_feed_parser_interface_init (SummerFeedParserInterface *iface)
{
	iface->handle_feed_node = handle_feed_node;
	iface->handle_item_node = handle_item_node;
}
