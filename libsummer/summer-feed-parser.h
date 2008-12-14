/* summer-feed-parser.h */

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

#ifndef __SUMMER_FEED_PARSER_H__
#define __SUMMER_FEED_PARSER_H__

#include <libxml/xmlreader.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SUMMER_TYPE_FEED_PARSER             (summer_feed_parser_get_type())
#define SUMMER_FEED_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED_PARSER,SummerFeedParser))
#define SUMMER_IS_FEED_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED_PARSER))
#define SUMMER_FEED_PARSER_GET_INTERFACE(inst)  (G_TYPE_INSTANCE_GET_INTERFACE((inst),SUMMER_TYPE_FEED_PARSER,SummerFeedParserInterface))

typedef struct _SummerFeedParser          SummerFeedParser;
typedef struct _SummerFeedParserInterface SummerFeedParserInterface;
typedef struct _SummerFeedData            SummerFeedData;
typedef struct _SummerItemData            SummerItemData;

struct _SummerFeedParserInterface {
	GTypeInterface parent;

	gint (*handle_feed_node) (SummerFeedParser* self, xmlTextReaderPtr node, SummerFeedData *feed, gboolean *start_item);
	gint (*handle_item_node) (SummerFeedParser* self, xmlTextReaderPtr node, SummerItemData *feed);
};

GType        summer_feed_parser_get_type    (void) G_GNUC_CONST;

gint summer_feed_parser_handle_feed_node (SummerFeedParser* self, xmlTextReaderPtr node, SummerFeedData *feed, gboolean *start_item);
gint summer_feed_parser_handle_item_node (SummerFeedParser* self, xmlTextReaderPtr node, SummerItemData *item);

struct _SummerFeedData {
	gchar *title;
	gchar *description;
	gchar *id;
	gchar *web_url;
	gchar *author;
	GTimeVal updated;
};

struct _SummerItemData {
	gchar *title;
	gchar *description;
	gchar *id;
	gchar *web_url;
	gchar *author;
	GTimeVal updated;
};

SummerFeedData* summer_feed_data_new (void);
SummerItemData* summer_item_data_new (void);

void summer_feed_data_free (SummerFeedData *data);
void summer_item_data_free (SummerItemData *data);

G_END_DECLS

#endif /* __SUMMER_FEED_PARSER_H__ */
