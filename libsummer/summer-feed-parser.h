/* summer-feed-parser.h */

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

#ifndef __SUMMER_FEED_PARSER_H__
#define __SUMMER_FEED_PARSER_H__

#include <libxml/xmlreader.h>
#include <glib-object.h>
#include "summer-data-types.h"

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED_PARSER             (summer_feed_parser_get_type())
#define SUMMER_FEED_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED_PARSER,SummerFeedParser))
#define SUMMER_FEED_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_FEED_PARSER,SummerFeedParserClass))
#define SUMMER_IS_FEED_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED_PARSER))
#define SUMMRE_IS_FEED_PARSER_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_FEED_PARSER))
#define SUMMER_FEED_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_FEED_PARSER,SummerFeedParserClass))

typedef struct _SummerFeedParser      SummerFeedParser;
typedef struct _SummerFeedParserClass SummerFeedParserClass;

struct _SummerFeedParser {
	GObject parent;
};

struct _SummerFeedParserClass {
	GObjectClass parent_class;
	gint (*handle_node) (SummerFeedParser *self, xmlTextReader *node, SummerFeedData *feed, gboolean *is_item);
};

GType        summer_feed_parser_get_type    (void) G_GNUC_CONST;

gint summer_feed_parser_handle_node (SummerFeedParser *self, xmlTextReader *node, SummerFeedData *feed, gboolean *is_item);
SummerFeedData* summer_feed_parser_parse (SummerFeedParser **parsers, int num_parsers, xmlTextReader* reader);
/**
 * SAVE_TEXT_CONTENTS():
 * @element_name: the name of the element you're looking for
 * @node: an xmlTextReader object
 * @result: a variable to store the result in
 * @ret: status variable - set to 1 if the element was found
 *
 * A macro that checks if the current @node has the name @element_name, and
 * if so saves the text content of @node to @result.
 */

#define SAVE_TEXT_CONTENTS(element_name, node, result, ret) \
	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST (element_name))) { \
		while (xmlTextReaderNodeType (node) != XML_READER_TYPE_TEXT) { \
			ret = xmlTextReaderRead (node); \
			if (ret <= 0) \
				return ret; \
			result = (gchar*) xmlTextReaderValue (node); \
			ret = 2; \
			break; \
		} \
	}

/**
 * SAVE_ATTRIBUTE():
 * @element_name: the name of the element you're looking for
 * @attribute_name: the name of the attribute you're looking for
 * @node: an xmlTextReader object
 * @result: a variable to store the result in
 * @ret: status variable - set to 1 if the element was found
 *
 * A macro that checks if the current @node has the name @element_name, and
 * if so looks for the attribute @attribute_name and saves it's value to
 * @result.
 */

#define SAVE_ATTRIBUTE(element_name, attribute_name, node, result, ret) \
	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST (element_name))) { \
		int r = 1; \
		while (r > 0) {\
			r = xmlTextReaderMoveToNextAttribute (node); \
			if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node), \
					BAD_CAST (attribute_name))) { \
				result = (gchar *) xmlTextReaderValue (node); \
				ret = 2; \
				break; \
			} \
		} \
		if (!ret) \
			xmlTextReaderMoveToElement (node); \
	}

/**
 * DO_WITH_TEXT_CONTENTS():
 * @element_name: the name of the element you're looking for
 * @node: an xmlTextReader object
 * @ret: status variable - set to 1 if the element was found
 * @code: code to process the text.
 *
 * A macro that checks if the current @node has the name @element_name, and
 * if so lets you do something with it. The text contents of @element_name
 * is available from @code as the %gchar* %text. %text will be freed after code
 * is run, so you must copy it if you want to save it.
 */


#define DO_WITH_TEXT_CONTENTS(element_name, node, ret, code) \
	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST (element_name))) { \
		while (xmlTextReaderNodeType (node) != XML_READER_TYPE_TEXT) { \
			ret = xmlTextReaderRead (node); \
			if (ret <= 0) \
				return ret; \
			gchar *text = (gchar*) xmlTextReaderValue (node); \
			code; \
			g_free (text); \
			ret = 2; \
			break; \
		} \
	}

G_END_DECLS

#endif /* __SUMMER_FEED_PARSER_H__ */
