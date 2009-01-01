/* summer-feed.c */

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

#include "summer-feed.h"
#include <string.h>
#include <libxml/xmlreader.h>
#include <gio/gio.h>
#include "summer-feed-parser.h"
#include "summer-atom-parser.h"
#include "summer-rss2-parser.h"
#include "summer-data-types.h"
#include "summer-web-backend.h"
/**
 * SECTION:summer-feed
 * @short_description: Provides a way to download and parse web feeds
 * @stability: Unstable
 * @include: libsummer/summer.h
 *
 * This component provides the class for downloading and parsing
 * web feeds.
 */

/**
 * SummerFeed:
 *
 * A class for parsing feeds.
 */

static void summer_feed_class_init (SummerFeedClass *klass);
static void summer_feed_init       (SummerFeed *obj);
static void summer_feed_finalize   (GObject *obj);

struct _SummerFeedPrivate {
	gchar *url;
	gchar *cache_dir;
	gint frequency;
	SummerFeedData *feed_data;
};
#define SUMMER_FEED_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                         SUMMER_TYPE_FEED, \
                                         SummerFeedPrivate))

static gchar *cache_dir = NULL;
static gint frequency;

enum {
	PROP_0,
	PROP_CACHE_DIR,
	PROP_FREQUENCY,
	PROP_URL,
	PROP_TITLE,
	PROP_DESCRIPTION,
	PROP_ID,
	PROP_WEB_URL,
	PROP_AUTHOR,
	PROP_UPDATED,
	PROP_ITEMS
};

G_DEFINE_TYPE (SummerFeed, summer_feed, G_TYPE_OBJECT);

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerFeedPrivate *priv;
	priv = SUMMER_FEED (object)->priv;

	switch (prop_id) {
	case PROP_CACHE_DIR:
		if (priv->cache_dir)
			g_free (priv->cache_dir);
		priv->cache_dir = g_value_dup_string (value);
		break;
	case PROP_FREQUENCY:
		priv->frequency = g_value_get_int (value);
		break;
	case PROP_URL:
		if (priv->url)
			g_free (priv->url);
		priv->url = g_value_dup_string (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
set_glist (GValue *value, GList *list)
{
	GValueArray *array = g_value_array_new (g_list_length (list));
	for (; list; list = list->next) {
		GValue tmp = {0};
		g_value_init (&tmp, G_TYPE_POINTER);
		g_value_set_pointer (&tmp, list->data);
		array = g_value_array_append (array, &tmp);
	}

	g_value_set_boxed (value, array);
	g_value_array_free (array);
}

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	SummerFeed *self = SUMMER_FEED (object);
	SummerFeedPrivate *priv;
	priv = self->priv;

	switch (prop_id) {
	case PROP_CACHE_DIR:
		g_value_set_string (value, priv->cache_dir);
		break;
	case PROP_FREQUENCY:
		g_value_set_int (value, priv->frequency);
		break;
	case PROP_URL:
		g_value_set_string (value, priv->url);
		break;
	case PROP_TITLE:
		g_value_set_string (value, priv->feed_data->title);
		break;
	case PROP_DESCRIPTION:
		g_value_set_string (value, priv->feed_data->description);
		break;
	case PROP_ID:
		g_value_set_string (value, priv->feed_data->id);
		break;
	case PROP_WEB_URL:
		g_value_set_string (value, priv->feed_data->web_url);
		break;
	case PROP_AUTHOR:
		g_value_set_string (value, priv->feed_data->author);
		break;
	case PROP_UPDATED:
		g_value_set_long (value, priv->feed_data->updated);
		break;
	case PROP_ITEMS:
		set_glist (value, priv->feed_data->items);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
summer_feed_class_init (SummerFeedClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_feed_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	g_type_class_add_private (gobject_class, sizeof(SummerFeedPrivate));

	GParamSpec *pspec;
	pspec = g_param_spec_string ("cache-dir",
		"Cache directory",
		"The directory to store cache files in. NULL to not cache.",
		NULL,
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_CACHE_DIR, pspec);

	pspec = g_param_spec_int ("frequency",
		"Frequency",
		"The number of seconds between feed refetches. 0 to disable.",
		0, 65535, 0,
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_FREQUENCY, pspec);

	pspec = g_param_spec_string ("url",
		"URL",
		"The URL of the feed",
		NULL,
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_URL, pspec);

	pspec = g_param_spec_string ("title",
		"Title",
		"The title of the feed",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_TITLE, pspec);

	pspec = g_param_spec_string ("description",
		"Description",
		"A more verbose description of feed than the title",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_DESCRIPTION, pspec);

	pspec = g_param_spec_string ("id",
		"ID",
		"A text string that uniquely identifies the feed",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_ID, pspec);

	pspec = g_param_spec_string ("web-url",
		"Web URL",
		"A URL where the same content is available as a web page",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_WEB_URL, pspec);

	pspec = g_param_spec_string ("author",
		"Author",
		"The author of the feed",
		NULL,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_AUTHOR, pspec);

	pspec = g_param_spec_long ("updated",
		"Updated",
		"The time when the feed was last updated",
		0,G_MAXLONG,0,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_UPDATED, pspec);

	pspec = g_param_spec_pointer ("item",
		"Item",
		"An item (a post) in the feed",
		G_PARAM_READABLE);
	pspec = g_param_spec_value_array ("items",
		"Items",
		"A list of the items (posts) in the feed",
		pspec,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_ITEMS, pspec);

	/**
	 * SummerFeed::new-entries:
	 *
	 * Signal that is emitted whenever new entries are downloaded
	 */
	g_signal_new (
		"new-entries",
		SUMMER_TYPE_FEED,
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (SummerFeedClass, new_entries),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE,
		0);
}

static void
summer_feed_init (SummerFeed *self)
{
	self->priv = SUMMER_FEED_GET_PRIVATE (self);
	g_object_set (self, "cache-dir", cache_dir, "frequency", frequency, NULL);
}

static void
summer_feed_finalize (GObject *obj)
{
	G_OBJECT_CLASS(summer_feed_parent_class)->finalize (obj);
}

/**
 * summer_feed_new():
 *
 * Creates a new %SummerFeed object.
 *
 * Returns: The newly created %SummerFeed object.
 */
SummerFeed*
summer_feed_new ()
{
	return SUMMER_FEED(g_object_new(SUMMER_TYPE_FEED, NULL));
}

static void
on_downloaded (SummerWebBackend *web, gchar *save_path, gchar *save_data, gpointer user_data)
{
	SummerFeed *self = SUMMER_FEED (user_data);
	SummerFeedPrivate *priv = self->priv;
	if (save_data) {
		SummerFeedParser *parsers[] = {
			SUMMER_FEED_PARSER (summer_atom_parser_new ()),
			SUMMER_FEED_PARSER (summer_rss2_parser_new ())};
		xmlParserInputBufferPtr buffer = xmlParserInputBufferCreateMem (save_data, strlen (save_data), 0);
		xmlTextReaderPtr reader = xmlNewTextReader (buffer, priv->url);
		priv->feed_data = summer_feed_parser_parse (parsers, sizeof (parsers) / sizeof (*parsers), reader);
	}
	g_signal_emit_by_name (self, "new-entries");
}

/**
 * summer_feed_start ():
 * @self: A %SummerFeed instance.
 * @url: The URL to download.
 *
 * Download the feed, parse it, and redo it every %SummerFeed::frequency 
 * seconds.
 */
void
summer_feed_start (SummerFeed *self, gchar *url) {
	g_object_set (self, "url", url, NULL);
	SummerWebBackend *web = summer_web_backend_new (NULL, url);
	g_signal_connect (web, "download-complete", G_CALLBACK (on_downloaded), self);
	summer_web_backend_fetch (web);
}

/**
 * summer_feed_set ():
 * @first_property_name: the first property name.
 * @var_args: the first property value, optionally followed by more property
 * names and values.
 *
 * Not meant to be used directly - see %summer_set.
 */
void
summer_feed_set (gchar *first_property_name, va_list var_args)
{
	const gchar *name;
	name = first_property_name;
	while (name) {
		if (!g_strcmp0 (name, "cache-dir")) {
			if (cache_dir)
				g_free (cache_dir);
			cache_dir = g_strdup (va_arg (var_args, gchar*));
		} else if (!g_strcmp0 (name, "frequency")) {
			frequency = va_arg (var_args, gint);
		} else {
			g_error ("Invalid property for feed: \"%s\"", name);
		}
		name = va_arg (var_args, gchar*);
	}
}
