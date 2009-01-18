/* summer-download.c */

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

#include "summer-download.h"
#include "summer-marshal.h"
#include "summer-debug.h"
/**
 * SECTION:summer-download
 * @short_description: Base class for downloaders.
 * @stability: Unstable
 * @see_also: %SummerDownloadWeb
 *
 * This section contains the base class for downloaders.
 */

/**
 * SummerDownload:
 *
 * The base class for all downloaders - that is, classes that implement a 
 * certain way to fetch media files from remote sources. You should not create
 * instances of this class.
 *
 * Look at %summer_create_download if you want to create downloader instances.
 */
static void summer_download_class_init (SummerDownloadClass *klass);
static void summer_download_init       (SummerDownload *obj);
static void summer_download_finalize   (GObject *obj);

struct _SummerDownloadPrivate {
	gchar *tmp_dir;
	gchar *save_dir;
	gchar *url;
};
#define SUMMER_DOWNLOAD_GET_PRIVATE(o)   (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                          SUMMER_TYPE_DOWNLOAD, \
										  SummerDownloadPrivate))

static gchar *tmp_dir = NULL;
static gchar *save_dir = NULL;

enum {
	PROP_0,
	PROP_TMP_DIR,
	PROP_SAVE_DIR,
	PROP_URL
};

G_DEFINE_ABSTRACT_TYPE (SummerDownload, summer_download, G_TYPE_OBJECT);

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerDownloadPrivate *priv;
	priv = SUMMER_DOWNLOAD (object)->priv;
	switch (prop_id) {
	case PROP_SAVE_DIR:
		if (priv->save_dir)
			g_free (priv->save_dir);
		priv->save_dir = g_value_dup_string (value);
		break;
	case PROP_TMP_DIR:
		if (priv->tmp_dir)
			g_free (priv->tmp_dir);
		priv->tmp_dir = g_value_dup_string (value);
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
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	SummerDownloadPrivate *priv;
	priv = SUMMER_DOWNLOAD (object)->priv;

	switch (prop_id) {
	case PROP_SAVE_DIR:
		g_value_set_string (value, priv->save_dir);
		break;
	case PROP_TMP_DIR:
		g_value_set_string (value, priv->tmp_dir);
		break;
	case PROP_URL:
		g_value_set_string (value, priv->url);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
summer_download_class_init (SummerDownloadClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_download_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	g_type_class_add_private (gobject_class, sizeof(SummerDownloadPrivate));

	GParamSpec *pspec;
	/**
	 * SummerDownload:save-dir:
	 *
	 * The directory to store completed downloads in. This will be initialized
	 * from the value set by %summer_set on construction.
	 */
	pspec = g_param_spec_string ("save-dir",
		"Save directory",
		"The directory completed download should be stored in",
		"/",
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_SAVE_DIR, pspec);
	
	/**
	 * SummerDownload:tmp-dir:
	 *
	 * The directory to store incomplete downloads in. This will be initialized
	 * from the value set by %summer_set on construction.
	 */
	pspec = g_param_spec_string ("tmp-dir",
			"Temp directory",
			"The directory where incomplete downloads should be stored",
			"/",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_TMP_DIR, pspec);

	pspec = g_param_spec_string ("url",
		"URL",
		"The URL to download",
		NULL,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROP_URL, pspec);

	/**
	 * SummerDownload::download-complete:
	 * @obj: the %SummerDownload object that emitted the signal
	 * @save_path: the file system path where the file was saved. %NULL if there
	 * was an error
	 *
	 * Signal that is emitted when the whole file has been downloaded and moved
	 * to it's final destination.
	 */
	g_signal_new (
			"download-complete",
			SUMMER_TYPE_DOWNLOAD,
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (SummerDownloadClass, download_complete),
			NULL, NULL,
			g_cclosure_marshal_VOID__STRING,
			G_TYPE_NONE,
			1,
			G_TYPE_STRING);
	/**
	 * SummerDownload::download-update:
	 * @obj: the %SummerDownload object that emitted the signal
	 * @received: the number of bytes of the file that has been downloaded
	 * @length: the total number of bytes to be downloaded. %-1 if this is not
	 * known.
	 *
	 * ::download-chunk is emitted every time a new block of the file has been
	 * retrieved.
	 */
	g_signal_new (
			"download-update",
			SUMMER_TYPE_DOWNLOAD,
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (SummerDownloadClass, download_update),
			NULL, NULL,
			summer_marshal_VOID__INT_INT,
			G_TYPE_NONE,
			2,
			G_TYPE_INT, G_TYPE_INT);
}

static void
print_update (SummerDownload *self, gint downloaded, gint length, gpointer user_data)
{
	summer_debug ("%f%% downloaded (%i)", downloaded / (float) length * 100, downloaded);
}

static void
summer_download_init (SummerDownload *self)
{
	self->priv = SUMMER_DOWNLOAD_GET_PRIVATE (self);
	g_object_set (self, "save-dir", save_dir, "tmp-dir", tmp_dir, NULL);
	if (summer_debug (NULL)) {
		g_signal_connect (self, "download-update", G_CALLBACK (print_update), NULL);
	}
}

static void
summer_download_finalize (GObject *self)
{
	SummerDownloadPrivate *priv;
	priv = SUMMER_DOWNLOAD (self)->priv;
	if (priv->save_dir)
		g_free (priv->save_dir);
	if (priv->tmp_dir)
		g_free (priv->tmp_dir);
	if (priv->url)
		g_free (priv->url);
	G_OBJECT_CLASS(summer_download_parent_class)->finalize (self);
}

/**
 * summer_download_set
 * @first_property_name: the first property name.
 * @var_args: the first property value, optionally followed by more property
 * names and values.
 *
 * Not meant to be used directly - see %summer_set.
 */
void
summer_download_set (gchar *first_property_name, va_list var_args)
{
	const gchar *name;
	name = first_property_name;
	while (name) {
		if (!g_strcmp0 (name, "tmp-dir")) {
			if (tmp_dir)
				g_free (tmp_dir);
			tmp_dir = g_strdup (va_arg (var_args, gchar*));
		} else if (!g_strcmp0 (name, "save-dir")) {
			if (save_dir)
				g_free (save_dir);
			save_dir = g_strdup (va_arg (var_args, gchar*));
		}
		name = va_arg (var_args, gchar*);
	}
}

/**
 * summer_download_start:
 * @obj: a %SummerDownload instance
 *
 * Start the file transfer
 */
void
summer_download_start (SummerDownload *obj)
{
	SUMMER_DOWNLOAD_GET_CLASS (obj)->start (obj);
}
