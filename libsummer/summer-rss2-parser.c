/* summer-rss2-parser.c */

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

#include "summer-rss2-parser.h"

#include <time.h>
#include <string.h>

/**
 * SECTION:summer-rss2-parser
 * @short_description: A %SummerFeedParser that parses RSS2 feeds
 * @stability: Unstable
 * @include: libsummer/summer-rss2-parser.h
 *
 * A %SummerFeedParser that parses RSS feeds of the RSS2 "format tree". That 
 * means it's supposed to be able to parse all RSS feeds, except RSS 0.90 and 
 * RSS 1.0 (so it should work with 0.91, 0.92, 0.93, 0.94, 2.0, 2.0.1).
 */

/** 
 * SummerRss2Parser:
 *
 * An RSS2 parser.
 */

static void summer_rss2_parser_class_init (SummerRss2ParserClass *klass);
static void summer_rss2_parser_init       (SummerRss2Parser *obj);
static void summer_rss2_parser_finalize   (GObject *obj);

static gint handle_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *data, gboolean *is_item);

G_DEFINE_TYPE (SummerRss2Parser, summer_rss2_parser, SUMMER_TYPE_FEED_PARSER);

static void
summer_rss2_parser_class_init (SummerRss2ParserClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_rss2_parser_finalize;

	SummerFeedParserClass *feed_parser_class;
	feed_parser_class = SUMMER_FEED_PARSER_CLASS (klass);
	feed_parser_class->handle_node = handle_node;
}

static void
summer_rss2_parser_init (SummerRss2Parser *obj)
{}

static void
summer_rss2_parser_finalize (GObject *obj)
{
	G_OBJECT_CLASS(summer_rss2_parser_parent_class)->finalize (obj);
}

/**
 * summer_rss2_parser_new ():
 *
 * Create a new %SummerRss2Parser.
 *
 * Returns: The newly created %SummerRss2Parser.
 */
SummerRss2Parser*
summer_rss2_parser_new (void)
{
	return SUMMER_RSS2_PARSER(g_object_new(SUMMER_TYPE_RSS2_PARSER, NULL));
}

static time_t
parse_rfc2822 (gchar *text) {
	//[day,] dd Mon yyyy hh:mm:ss TZ
	gchar *months[] = {
		"Jan", 
		"Feb", 
		"Mar", 
		"Apr", 
		"May", 
		"Jun", 
		"Jul", 
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"};
	// While deprecated, still in use. Ordered from -12 to +12
	gchar *timezones[25][4] = {
		{"M", NULL},
		{"L", NULL},
		{"K", NULL},
		{"I", NULL},
		{"H", "PST", NULL},
		{"G", "MST", "PDT", NULL},
		{"F", "CST", "MDT", NULL},
		{"E", "EST", "CDT", NULL},
		{"D", "EDT", NULL},
		{"C", NULL},
		{"B", NULL},
		{"A", NULL},
		{"Z", "UT", "GMT", NULL},
		{"N", NULL},
		{"O", NULL},
		{"P", NULL},
		{"Q", NULL},
		{"R", NULL},
		{"S", NULL},
		{"T", NULL},
		{"U", NULL},
		{"V", NULL},
		{"W", NULL},
		{"X", NULL},
		{"Y", NULL}};
	struct tm tm;
	if (strstr (text, ",")) {
		text = strstr (text, ",") + 1;
	}
	g_strchug (text);
	gchar **parts = g_strsplit (text, " ", 0);
	tm.tm_mday = atoi (parts[0]);
	int i;
	for (i = 0; i < 12; i++) {
		if (!g_strcmp0 (months[i], parts[1])) {
			tm.tm_mon = i;
			break;
		}
	}
	tm.tm_year = atoi (parts[2]) - 1900;
	gchar** time = g_strsplit (parts[3], ":", 0);
	tm.tm_hour = atoi (time[0]);
	tm.tm_min = atoi (time[1]);
	if (g_strv_length (time) > 2)
		tm.tm_sec = atoi (time[2]);
	tm.tm_isdst = 0;
	time_t t;
	t = mktime (&tm);
	if (parts[4][0] == '+' || parts[4][0] == '-') {
		gchar *tmp = g_strndup (parts[4], 3);
		t -= atoi (tmp) * 3600;
		if (g_utf8_strlen (parts[4], -1) == 5) {
			tmp[1] = parts[4][3]; // overwrite the digits, keep the sign
			tmp[2] = parts[4][4];
			t -= atoi (tmp) * 60;
		}
		g_free (tmp);
	} else {
		for (i = 0; i < 25; i++) {
			int j;
			for (j = 0; timezones[i][j]; j ++) {
				if (!g_strcmp0 (timezones[i][j], parts[4])) {
					t += (i - 12) * 3600;
					i = 26;
					break;
				}
			}
		}
	}
	g_strfreev (parts);
	g_strfreev (time);
	return t - timezone;
}

static gint
handle_feed_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *feed, gboolean *is_item)
{
	gint ret = 1;
	SAVE_TEXT_CONTENTS ("title", node, feed->title, ret);
	SAVE_TEXT_CONTENTS ("description", node, feed->description, ret);
	SAVE_TEXT_CONTENTS ("link", node, feed->web_url, ret);
	SAVE_TEXT_CONTENTS ("managingEditor", node, feed->author, ret);
	DO_WITH_TEXT_CONTENTS ("lastBuildDate", node, ret,
		feed->updated = parse_rfc2822 (text));

	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST ("item"))) {
		*is_item = TRUE;
		summer_feed_data_append_item (feed);
	}

	return ret;
}

static gint
handle_item_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *feed)
{
	SummerItemData *item = SUMMER_ITEM_DATA (feed->items->data);
	gint ret = 1;
	
	SAVE_TEXT_CONTENTS ("title", node, item->title, ret);
	SAVE_TEXT_CONTENTS ("description", node, item->description, ret);
	SAVE_TEXT_CONTENTS ("guid", node, item->id, ret);
	DO_WITH_TEXT_CONTENTS ("pubDate", node, ret, 
		item->updated = parse_rfc2822 (text));
	SAVE_TEXT_CONTENTS ("link", node, item->web_url, ret);

	if (xmlStrEqual (xmlTextReaderConstLocalName (node), BAD_CAST ("enclosure"))) {
		SummerDownloadableData *dl = summer_item_data_append_downloadable (item, NULL, NULL, 0);
		int r = 1;
		while (r > 0) {
			r = xmlTextReaderMoveToNextAttribute (node);
			if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node),
					BAD_CAST ("url"))) {
				dl->url = (gchar *)xmlTextReaderValue (node);
			} else if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node),
					BAD_CAST ("type"))) {
				dl->mime = (gchar *)xmlTextReaderValue (node);
			} else if (r > 0 && xmlStrEqual (xmlTextReaderConstLocalName (node),
					BAD_CAST ("length"))) {
				dl->length = atoi ((char *)xmlTextReaderConstValue (node));
			}
		}
		return 2;
	}

	return ret;
}

static gint
handle_node (SummerFeedParser *self, xmlTextReaderPtr node, SummerFeedData *data, gboolean *is_item)
{
	if (xmlTextReaderConstNamespaceUri (node) != NULL)
		return 1;
	
	if (*is_item)
		return handle_item_node (self, node, data);
	else
		return handle_feed_node (self, node, data, is_item);
}
