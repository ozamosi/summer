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
#include "summer-feed-cache.h"
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

static gchar *default_cache_dir;
static gint default_frequency;

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
		if (priv->feed_data->id)
			g_value_set_string (value, priv->feed_data->id);
		else
			g_value_set_string (value, priv->feed_data->web_url);
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
		"The number of seconds between feed refetches. -1 to disable.",
		-1, G_MAXINT, -1,
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
	g_object_set (self, 
		"cache-dir", default_cache_dir, "frequency", default_frequency, NULL);
}

static void
summer_feed_finalize (GObject *obj)
{
	SummerFeedPrivate *priv = SUMMER_FEED_GET_PRIVATE (obj);
	if (priv->url)
		g_free (priv->url);
	if (priv->cache_dir)
		g_free (priv->cache_dir);
	if (priv->feed_data)
		g_object_unref (priv->feed_data);
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
		
		unsigned int i;
		for (i = 0; i < sizeof (parsers) / sizeof (*parsers); i++) {
			g_object_unref (parsers[i]);
		}
	}

	SummerFeedCache *cache = summer_feed_cache_get ();
	summer_feed_cache_filter_old_items (cache, &priv->feed_data->items);
	g_object_unref (cache);

	if (priv->feed_data->items != NULL)
		g_signal_emit_by_name (self, "new-entries");
}

static gboolean
download_timeout (gpointer data) {
	if (!SUMMER_IS_FEED (data)) {
		g_object_unref (data);
		return FALSE;
	}
	SummerFeed *self = SUMMER_FEED (data);
	SummerWebBackend *web = summer_web_backend_new (NULL, self->priv->url);
	g_signal_connect (web, "download-complete", G_CALLBACK (on_downloaded), self);
	summer_web_backend_fetch (web);
	return TRUE;
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
	if (self->priv->frequency > 0) {
		g_object_ref (self);
		g_timeout_add_seconds (self->priv->frequency, 
			(GSourceFunc) download_timeout,
			self);
	}
	download_timeout (self);
}

/**
 * summer_feed_set_default():
 * @cache_dir: the directory to store cache files in, or %NULL to keep current
 * value.
 * @frequency: the number of seconds to wait between checking for updates. %0 to
 * keep current value, %-1 to disable.
 *
 * Set default options for all new SummerFeed objects.
 *
 * These options may be set at any time during the session. You may change these
 * as you wish after downloads have been started, but note that changing these
 * will only affect downloads started after the change. These options can be
 * individually overridden by editing the SummerDownload::cache-dir and 
 * SummerDownload::frequency properties.
 */
void
summer_feed_set_default (const gchar *cache_dir, const gint frequency)
{
	if (cache_dir != NULL) {
		if (default_cache_dir)
			g_free (default_cache_dir);
		default_cache_dir = g_strdup (cache_dir);
		SummerFeedCache *cache = summer_feed_cache_get ();
		gchar *cache_file;
		cache_file = g_build_filename (default_cache_dir, "seen_objects", NULL);
		g_object_set (cache, "cache-file", cache_file, NULL);
		g_object_unref (cache);
	}
	if (frequency != 0)
		default_frequency = frequency;
}

/**
 * summer_feed_get_cache_dir:
 * @self: the SummerFeed object
 *
 * Returns the directory where cache files will be stored.
 *
 * Returns: the directory where cache files will be stored.
 */
gchar *
summer_feed_get_cache_dir (SummerFeed *self)
{
	gchar *cache_dir;
	g_object_get (self, "cache-dir", &cache_dir, NULL);
	return cache_dir;
}

/**
 * summer_feed_set_cache_dir:
 * @self: the SummerFeed object
 * @cache_dir: a directory path
 *
 * Changes the directory where cache files will be stored to @cache_dir
 */
void
summer_feed_set_cache_dir (SummerFeed *self, gchar *cache_dir)
{
	g_object_set (self, "cache-dir", cache_dir, NULL);
}

/**
 * summer_feed_get_frequency:
 * @self: a %SummerFeed object
 *
 * Returns the number of seconds the %SummerFeed object waits before it tries 
 * to recheck the feed.
 *
 * Returns: the frequency.
 */
gint
summer_feed_get_frequency (SummerFeed *self)
{
	gint frequency;
	g_object_get (self, "frequency", &frequency, NULL);
	return frequency;
}

/**
 * summer_feed_set_frequency:
 * @self: a %SummerFeed object
 * @frequency: the new frequency.
 *
 * Sets the number of seconds the %SummerFeed object waits before 
 * it tries to recheck the feed to @frequency.
 */
void
summer_feed_set_frequency (SummerFeed *self, gint frequency)
{
	g_object_set (self, "frequency", frequency, NULL);
}


/**
 * summer_feed_get_url:
 * @self: the SummerFeed object
 *
 * Returns the URL this %SummerFeed uses to fetch it's feeds.
 *
 * Returns: the URL this %SummerFeed uses.
 */
gchar *
summer_feed_get_url (SummerFeed *self)
{
	gchar *url;
	g_object_get (self, "url", &url, NULL);
	return url;
}

/**
 * summer_feed_set_url:
 * @self: the SummerFeed object
 * @url: a URL
 *
 * Changes the URL this %SummerFeed uses to fetched it's feeds to @url.
 */
void
summer_feed_set_url (SummerFeed *self, gchar *url)
{
	g_object_set (self, "url", url, NULL);
}

/**
 * summer_feed_get_title:
 * @self: a %SummerFeedData object.
 *
 * Returns the title of the feed that this object represents.
 *
 * Returns: The title of the feed.
 */
gchar *
summer_feed_get_title (SummerFeed *self)
{
	gchar *title;
	g_object_get (self, "title", &title, NULL);
	return title;
}

/**
 * summer_feed_get_description:
 * @self: a %SummerFeedData object.
 *
 * Returns the description of the feed that this object represents.
 *
 * Returns: The description of the feed.
 */
gchar *
summer_feed_get_description (SummerFeed *self)
{
	gchar *description;
	g_object_get (self, "description", &description, NULL);
	return description;
}

/**
 * summer_feed_get_id:
 * @self: a %SummerFeedData object.
 *
 * Returns a unique identifier for the feed that this object represents.
 *
 * Returns: The title of the feed.
 */
gchar *
summer_feed_get_id (SummerFeed *self)
{
	gchar *id;
	g_object_get (self, "id", &id, NULL);
	return id;
}

/**
 * summer_feed_get_web_url:
 * @self: a %SummerFeedData object.
 *
 * Returns a URL, pointing to a web resource for the feed that this object 
 * represents.
 *
 * Returns: A URL to a web representation of the feed.
 */
gchar *
summer_feed_get_web_url (SummerFeed *self)
{
	gchar *web_url;
	g_object_get (self, "web-url", &web_url, NULL);
	return web_url;
}

/**
 * summer_feed_get_author:
 * @self: a %SummerFeedData object.
 *
 * Returns the author of the feed that this object represents.
 *
 * Returns: The author of the feed.
 */
gchar *
summer_feed_get_author (SummerFeed *self)
{
	gchar *author;
	g_object_get (self, "author", &author, NULL);
	return author;
}

/**
 * summer_feed_get_updated:
 * @self: a %SummerFeedData object.
 *
 * Returns when, in seconds since the epoch, this feed was most recently
 * updated.
 *
 * Returns: The last time this feed was updated.
 */
time_t
summer_feed_get_updated (SummerFeed *self)
{
	glong updated;
	g_object_get (self, "updated", &updated, NULL);
	return updated;
}

/**
 * summer_feed_get_items:
 * @self: a %SummerFeedData object.
 *
 * Returns all items in this feed.
 *
 * Returns: The items in the feed.
 */
GList *
summer_feed_get_items (SummerFeed *self)
{
	return g_list_copy (self->priv->feed_data->items);
}
