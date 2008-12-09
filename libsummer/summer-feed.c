/* summer-feed.c */

/* This file is part of summer_feed.
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

#include "summer-feed.h"
/* include other impl specific header files */

/* 'private'/'protected' functions */
static void summer_feed_class_init (SummerFeedClass *klass);
static void summer_feed_init       (SummerFeed *obj);
static void summer_feed_finalize   (GObject *obj);
/* list my signals  */
enum {
	/* MY_SIGNAL_1, */
	/* MY_SIGNAL_2, */
	LAST_SIGNAL
};

typedef struct _SummerFeedPrivate SummerFeedPrivate;
struct _SummerFeedPrivate {
	/* my private members go here, eg. */
	/* gboolean frobnicate_mode; */
};
#define SUMMER_FEED_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                         SUMMER_TYPE_FEED, \
                                         SummerFeedPrivate))
/* globals */
static GObjectClass *parent_class = NULL;

/* uncomment the following if you have defined any signals */
/* static guint signals[LAST_SIGNAL] = {0}; */

GType
summer_feed_get_type (void)
{
	static GType my_type = 0;
	if (!my_type) {
		static const GTypeInfo my_info = {
			sizeof(SummerFeedClass),
			NULL,		/* base init */
			NULL,		/* base finalize */
			(GClassInitFunc) summer_feed_class_init,
			NULL,		/* class finalize */
			NULL,		/* class data */
			sizeof(SummerFeed),
			1,		/* n_preallocs */
			(GInstanceInitFunc) summer_feed_init,
			NULL
		};
		my_type = g_type_register_static (G_TYPE_OBJECT,
		                                  "SummerFeed",
		                                  &my_info, 0);
	}
	return my_type;
}

static void
summer_feed_class_init (SummerFeedClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	parent_class            = g_type_class_peek_parent (klass);
	gobject_class->finalize = summer_feed_finalize;

	g_type_class_add_private (gobject_class, sizeof(SummerFeedPrivate));

	/* signal definitions go here, e.g.: */
/* 	signals[MY_SIGNAL_1] = */
/* 		g_signal_new ("my_signal_1",....); */
/* 	signals[MY_SIGNAL_2] = */
/* 		g_signal_new ("my_signal_2",....); */
/* 	etc. */
}

static void
summer_feed_init (SummerFeed *obj)
{
/* uncomment the following if you init any of the private data */
/* 	SummerFeedPrivate *priv = SUMMER_FEED_GET_PRIVATE(obj); */

/* 	initialize this object, eg.: */
/* 	priv->frobnicate_mode = FALSE; */
}

static void
summer_feed_finalize (GObject *obj)
{
/* 	free/unref instance resources here */
	G_OBJECT_CLASS(parent_class)->finalize (obj);
}

SummerFeed*
summer_feed_new (void)
{
	return SUMMER_FEED(g_object_new(SUMMER_TYPE_FEED, NULL));
}

/* insert many other interesting function implementations */
/* such as summer_feed_do_something, or summer_feed_has_foo */

