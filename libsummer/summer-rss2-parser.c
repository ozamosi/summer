/* summer-rss2-parser.c */

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

#include "summer-rss2-parser.h"
/* include other impl specific header files */

/* 'private'/'protected' functions */
static void summer_rss2_parser_class_init (SummerRss2ParserClass *klass);
static void summer_rss2_parser_init       (SummerRss2Parser *obj);
static void summer_rss2_parser_finalize   (GObject *obj);
/* list my signals  */
enum {
	/* MY_SIGNAL_1, */
	/* MY_SIGNAL_2, */
	LAST_SIGNAL
};

typedef struct _SummerRss2ParserPrivate SummerRss2ParserPrivate;
struct _SummerRss2ParserPrivate {
	/* my private members go here, eg. */
	/* gboolean frobnicate_mode; */
};
#define SUMMER_RSS2_PARSER_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                               SUMMER_TYPE_RSS2_PARSER, \
                                               SummerRss2ParserPrivate))
/* globals */
/* uncomment the following if you have defined any signals */
/* static guint signals[LAST_SIGNAL] = {0}; */

G_DEFINE_TYPE (SummerRss2Parser, summer_rss2_parser, G_TYPE_OBJECT);

static void
summer_rss2_parser_class_init (SummerRss2ParserClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_rss2_parser_finalize;

	g_type_class_add_private (gobject_class, sizeof(SummerRss2ParserPrivate));

	/* signal definitions go here, e.g.: */
/* 	signals[MY_SIGNAL_1] = */
/* 		g_signal_new ("my_signal_1",....); */
/* 	signals[MY_SIGNAL_2] = */
/* 		g_signal_new ("my_signal_2",....); */
/* 	etc. */
}

static void
summer_rss2_parser_init (SummerRss2Parser *obj)
{
/* uncomment the following if you init any of the private data */
/* 	SummerRss2ParserPrivate *priv = SUMMER_RSS2_PARSER_GET_PRIVATE(obj); */

/* 	initialize this object, eg.: */
/* 	priv->frobnicate_mode = FALSE; */
}

static void
summer_rss2_parser_finalize (GObject *obj)
{
/* 	free/unref instance resources here */
	G_OBJECT_CLASS(summer_rss2_parser_parent_class)->finalize (obj);
}

SummerRss2Parser*
summer_rss2_parser_new (void)
{
	return SUMMER_RSS2_PARSER(g_object_new(SUMMER_TYPE_RSS2_PARSER, NULL));
}

/* insert many other interesting function implementations */
/* such as summer_rss2_parser_do_something, or summer_rss2_parser_has_foo */

