/* summer-feed-cache-default.c */

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

#include "summer-feed-cache-default.h"
#include "summer-feed-cache.h"
#include "summer-data-types.h"
#include <string.h>
#include <glib.h>
#include <gio/gio.h>

/**
 * SECTION:summer-feed-cache-default
 * @short_description: Helps you keep track of what files you've downloaded
 * @stability: Unstable
 * @include: libsummer/summer-feed-cache-default.h
 *
 * This component is used to keep track of what files are old, and what files
 * are new. It has no concept of per-subscription uniqueness.
 */

/**
 * SummerFeedCacheDefault:
 *
 * A singleton class used to filter previously seen files.
 */

static void summer_feed_cache_default_class_init (SummerFeedCacheDefaultClass *klass);
static void summer_feed_cache_default_init       (SummerFeedCacheDefault *obj);
static void summer_feed_cache_default_finalize   (GObject *obj);
static void summer_feed_cache_interface_init     (SummerFeedCacheInterface *iface);
static GList* parse_cache_file (GFile *file);

struct _SummerFeedCacheDefaultPrivate {
	GList *downloading_objects;
	GList *downloaded_objects;
	GFile *cache_file;
	gchar *cache_path;
};
#define SUMMER_FEED_CACHE_DEFAULT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                          SUMMER_TYPE_FEED_CACHE_DEFAULT, \
										  SummerFeedCacheDefaultPrivate))

enum {
	PROP_0,
	PROP_CACHE_FILE
};

G_DEFINE_TYPE_WITH_CODE (
	SummerFeedCacheDefault,
	summer_feed_cache_default,
	G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (
		SUMMER_TYPE_FEED_CACHE,
		summer_feed_cache_interface_init));

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerFeedCacheDefaultPrivate *priv = SUMMER_FEED_CACHE_DEFAULT (object)->priv;

	switch (prop_id) {
	case PROP_CACHE_FILE:
		if (priv->cache_path)
			g_free (priv->cache_path);
		priv->cache_path = g_value_dup_string (value);
		if (G_IS_OBJECT (priv->cache_file))
			g_object_unref (priv->cache_file);
		priv->cache_file = g_file_new_for_path (priv->cache_path);
		if (priv->downloaded_objects) {
			GList *list;
			for (list = priv->downloaded_objects;
					list != NULL;
					list = list->next) {
				g_free (list->data);
			}
			g_list_free (priv->downloaded_objects);
		}
		priv->downloaded_objects = parse_cache_file (priv->cache_file);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	SummerFeedCacheDefaultPrivate *priv = SUMMER_FEED_CACHE_DEFAULT (object)->priv;

	switch (prop_id) {
	case PROP_CACHE_FILE:
		g_value_set_string (value, priv->cache_path);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
summer_feed_cache_default_class_init (SummerFeedCacheDefaultClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;
	gobject_class->finalize = summer_feed_cache_default_finalize;

	g_type_class_add_private (gobject_class, sizeof(SummerFeedCacheDefaultPrivate));

	GParamSpec *pspec;

	pspec = g_param_spec_string ("cache-file",
		"Cache File",
		"The file to save the cache data in",
		NULL,
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_CACHE_FILE, pspec);
}

static GList*
parse_cache_file (GFile *file)
{
	if (!g_file_query_exists (file, NULL))
		return NULL;

	GList *result = NULL;
	GFileInputStream *stream = g_file_read (file, NULL, NULL);
	if (!G_IS_INPUT_STREAM (stream)) {
		return NULL;
	}
	gsize length = 1;

	GString *doc = g_string_new (NULL);
	gchar buffer[2048];
	while (length > 0) {
		g_input_stream_read_all (G_INPUT_STREAM (stream), buffer, 2048, &length, NULL, NULL);
		g_string_append_len (doc, buffer, length);
	}
	g_input_stream_close (G_INPUT_STREAM (stream), NULL, NULL);
	gchar **lines = g_strsplit (doc->str, "\n", 0);
	g_string_free (doc, TRUE);

	gchar **line;
	for (line = lines; *line != NULL; line++)
	{
		gboolean exists = FALSE;
		GList *node;
		for (node = result; node != NULL; node = node->next) {
			if (!g_strcmp0 ((gchar *)node->data, *line)) {
				exists = TRUE;
				break;
			}
		}
		if (!exists) {
			result = g_list_prepend (result, g_strdup (*line));
		}
	}
	result = g_list_reverse (result);
	g_strfreev (lines);
	return result;
}

static void
summer_feed_cache_default_init (SummerFeedCacheDefault *self)
{
	self->priv = SUMMER_FEED_CACHE_DEFAULT_GET_PRIVATE (self);
}

static void
summer_feed_cache_default_finalize (GObject *obj)
{
	SummerFeedCacheDefaultPrivate *priv = SUMMER_FEED_CACHE_DEFAULT (obj)->priv;
	g_object_unref (priv->cache_file);
	GList *list;
	for (list = priv->downloaded_objects; list != NULL; list = list->next) {
		g_free (list->data);
	}
	g_list_free (priv->downloaded_objects);

	for (list = priv->downloading_objects; list != NULL; list = list->next) {
		g_free (list->data);
	}
	g_list_free (priv->downloading_objects);

	g_free (priv->cache_path);
	G_OBJECT_CLASS(summer_feed_cache_default_parent_class)->finalize (obj);
}

static void
write_cache (SummerFeedCacheDefault *self) {
	if (self->priv->cache_path == NULL)
		return;
	GError *error = NULL;
	GFile *directory = g_file_get_parent (self->priv->cache_file);
	if (!g_file_query_exists (directory, NULL)) {
		if (!g_file_make_directory_with_parents (directory, NULL, &error)) {
			g_warning ("Couldn't create cache dir: %s", error->message);
			g_clear_error (&error);
			return;
		}
	}
	GFileOutputStream *stream = g_file_replace (
		self->priv->cache_file,
		NULL,
		FALSE,
		G_FILE_CREATE_NONE,
		NULL,
		&error);
	if (error != NULL) {
		g_warning ("Error writing cache file: %s", error->message);
		g_clear_error (&error);
		return;
	}
	GList *node;
	for (node = self->priv->downloaded_objects; node != NULL; node = node->next) {
		if (node->data == NULL)
			continue;
		g_output_stream_write (
			G_OUTPUT_STREAM (stream),
			node->data,
			strlen (node->data),
			NULL,
			NULL);
		g_output_stream_write (
			G_OUTPUT_STREAM (stream),
			"\n",
			1,
			NULL,
			NULL);
	}
	g_output_stream_close (G_OUTPUT_STREAM (stream), NULL, NULL);
}

static GList*
list_find_string (GList *list, const gchar *string)
{
	GList *l;
	for (l = list; l != NULL; l = l->next) {
		if (!g_strcmp0 ((gchar *)l->data, string)) {
			return l;
		}
	}
	return NULL;
}

static void
filter_items (SummerFeedCache *obj, GList **items)
{
	g_return_if_fail (SUMMER_IS_FEED_CACHE_DEFAULT (obj));
	SummerFeedCacheDefault *self = SUMMER_FEED_CACHE_DEFAULT (obj);

	GList *cur_item, *cur_cache, *new_items = NULL;
	for (cur_item = *items; cur_item != NULL; cur_item = cur_item->next) {
		gchar *id = summer_item_data_get_id (SUMMER_ITEM_DATA (cur_item->data));
		cur_cache = list_find_string (self->priv->downloaded_objects, id);
		if (!cur_cache)
			cur_cache = list_find_string (self->priv->downloading_objects, id);

		if (cur_cache) {
			g_object_unref (G_OBJECT (cur_item->data));
		} else {
			new_items = g_list_prepend (new_items, cur_item->data);
			self->priv->downloading_objects = g_list_prepend (
				self->priv->downloading_objects, g_strdup (id));
		}
	}
	g_list_free (*items);
	*items = g_list_reverse (new_items);
	write_cache (self);
}

static void
add_new_item (SummerFeedCache *obj, SummerItemData *item)
{
	g_return_if_fail (SUMMER_IS_FEED_CACHE_DEFAULT (obj));
	SummerFeedCacheDefault *self = SUMMER_FEED_CACHE_DEFAULT (obj);

	gchar *id = summer_item_data_get_id (item);
	if (!id)
		return;
	GList *cache_node = list_find_string (self->priv->downloading_objects, id);
	if (!cache_node)
		return;
	self->priv->downloaded_objects = g_list_prepend (
		self->priv->downloaded_objects, cache_node->data);
	self->priv->downloading_objects = g_list_delete_link (
		self->priv->downloading_objects, cache_node);
	write_cache (self);
}

static void
summer_feed_cache_interface_init (SummerFeedCacheInterface *iface)
{
	iface->filter_items = filter_items;
	iface->add_new_item = add_new_item;
}
