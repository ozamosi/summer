/* summer-download.c */

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

#include "summer-download.h"
#include "summer-data-types.h"
#include "summer-marshal.h"
#include "summer-enum-types.h"
#include "summer-debug.h"

#include <gio/gio.h>

/**
 * SECTION:summer-download
 * @short_description: Base class for downloaders.
 * @stability: Unstable
 * @see_also: #SummerDownloadWeb
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
 * #SummerDownload instances steal a reference when used - you should thus not
 * free any #SummerDownload-objects.
 *
 * Look at #summer_create_download if you want to create downloader instances.
 */
static void summer_download_class_init (SummerDownloadClass *klass);
static void summer_download_init       (SummerDownload *obj);
static void summer_download_finalize   (GObject *obj);

struct _SummerDownloadPrivate {
	gchar *tmp_dir;
	gchar *save_dir;
	gchar *filename;
	SummerItemData *item;
	SummerDownloadableData *downloadable;
	SummerDownloadState state;
};

#define SUMMER_DOWNLOAD_GET_PRIVATE(o)   (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                          SUMMER_TYPE_DOWNLOAD, \
										  SummerDownloadPrivate))

static gchar *default_tmp_dir = NULL;
static gchar *default_save_dir = NULL;

enum {
	PROP_0,
	PROP_TMP_DIR,
	PROP_SAVE_DIR,
	PROP_URL,
	PROP_FILENAME,
	PROP_ITEM,
	PROP_DOWNLOADABLE,
	PROP_STATE,
	PROP_PAUSED
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
	case PROP_FILENAME:
		if (priv->filename)
			g_free (priv->filename);
		priv->filename = g_value_dup_string (value);
		break;
	case PROP_ITEM:
		if (priv->item)
			g_object_unref (priv->item);
		priv->item = g_value_dup_object (value);
		break;
	case PROP_DOWNLOADABLE:
		if (priv->downloadable)
			g_object_unref (priv->downloadable);
		priv->downloadable = g_value_dup_object (value);
		break;
	case PROP_PAUSED:
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
		g_value_set_string (value, SUMMER_DOWNLOADABLE_DATA (priv->item->downloadables->data)->url);
		break;
	case PROP_FILENAME:
		g_value_set_string (value, priv->filename);
		break;
	case PROP_ITEM:
		g_value_set_object (value, priv->item);
		break;
	case PROP_DOWNLOADABLE:
		g_value_set_object (value, priv->downloadable);
		break;
	case PROP_STATE:
		g_value_set_enum (value, priv->state);
		break;
	case PROP_PAUSED:
		g_value_set_boolean (value, FALSE);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
download_error (SummerDownload *self, GError *error)
{
	g_clear_error (&error);
}

static void
summer_download_class_init (SummerDownloadClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	klass->download_error = download_error;

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
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_URL, pspec);

	pspec = g_param_spec_string ("filename",
		"Filename",
		"The filename of the file that's downloading",
		NULL,
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_FILENAME, pspec);

	pspec = g_param_spec_object ("item",
		"Item",
		"A SummerItemData to download",
		SUMMER_TYPE_ITEM_DATA,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROP_ITEM, pspec);
	
	pspec = g_param_spec_object ("downloadable",
		"Downloadable",
		"The SummerDownloadableData to download",
		SUMMER_TYPE_DOWNLOADABLE_DATA,
		G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROP_DOWNLOADABLE, pspec);

	pspec = g_param_spec_enum ("state",
		"State",
		"If the download is downloading, completed, paused, etc",
		SUMMER_TYPE_DOWNLOAD_STATE,
		SUMMER_DOWNLOAD_STATE_UNKNOWN,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_STATE, pspec);

	pspec = g_param_spec_boolean ("paused",
		"Paused",
		"Whether the download is paused or not",
		FALSE,
		G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROP_PAUSED, pspec);

	/**
	 * SummerDownload::download-complete:
	 * @obj: the #SummerDownload object that emitted the signal
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
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE,
			0);

	/**
	 * SummerDownload::download-done:
	 * @obj: the #SummerDownload object that emitted the signal
	 *
	 * Signal that is emitted when summer is done with the file. In the HTTP
	 * case, this is the same as #SummerDownload::download-complete. In the
	 * bittorrent case, #SummerDownload::download-complete is emitted prior to
	 * seeding, while #SummerDownload::download-done is emitted after.
	 */
	g_signal_new (
			"download-done",
			SUMMER_TYPE_DOWNLOAD,
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (SummerDownloadClass, download_done),
			NULL, NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE,
			0);

	/**
	 * SummerDownload::download-update:
	 * @obj: the #SummerDownload object that emitted the signal
	 * @received: the number of bytes of the file that has been downloaded
	 * @length: the total number of bytes to be downloaded. %-1 if this is not
	 * known.
	 *
	 * ::download-update is emitted if the backend feels it has anything new
	 * to report. This will usually be approximately once per second, but if
	 * there is nothing to report, the backend may choose not to emit this
	 * signal.
	 */
	g_signal_new (
			"download-update",
			SUMMER_TYPE_DOWNLOAD,
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (SummerDownloadClass, download_update),
			NULL, NULL,
			summer_marshal_VOID__UINT64_UINT64,
			G_TYPE_NONE,
			2,
			G_TYPE_UINT64, G_TYPE_UINT64);
	
	/**
	 * SummerDownload::download-started:
	 * @obj: the #SummerDownload object that emitted the signal
	 *
	 * Some information, such as filesize or filename, may not be known by the
	 * downloading backend initially. When this kind of information has been
	 * collected, ::download-update is emitted.
	 */
	g_signal_new (
			"download-started",
			SUMMER_TYPE_DOWNLOAD,
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (SummerDownloadClass, download_started),
			NULL, NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE,
			0);
	
	/**
	 * SummerDownload::download-error:
	 * @obj: the #SummerDownload object that emitted the signal
	 * @error: a %GError
	 *
	 * If a download fails, this signal will be emitted, sending a %GError
	 * parameter explaining the error. If this is emitted,
	 * #SummerDownload::download-complete will not be emitted.
	 */
	g_signal_new (
			"download-error",
			SUMMER_TYPE_DOWNLOAD,
			G_SIGNAL_RUN_CLEANUP,
			G_STRUCT_OFFSET (SummerDownloadClass, download_error),
			NULL, NULL,
			g_cclosure_marshal_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
}

static void
print_update (SummerDownload *self, guint64 downloaded, guint64 length, gpointer user_data)
{
	gchar *name;
	g_object_get (self, "filename", &name, NULL);
	summer_debug ("%s: %f%% downloaded (%" G_GUINT64_FORMAT " of %" G_GUINT64_FORMAT ")", name, downloaded / (float) length * 100, downloaded, length);
	g_free (name);
}

static void
set_started (SummerDownload *self, ...)
{
	g_return_if_fail (self->priv->state <= SUMMER_DOWNLOAD_STATE_DOWNLOADING);

	self->priv->state = SUMMER_DOWNLOAD_STATE_DOWNLOADING;
}

static void
set_complete (SummerDownload *self, ...)
{
	g_return_if_fail (self->priv->state <= SUMMER_DOWNLOAD_STATE_SEEDING);

	self->priv->state = SUMMER_DOWNLOAD_STATE_SEEDING;
}

static void
set_done (SummerDownload *self, ...)
{
	g_return_if_fail (self->priv->state <= SUMMER_DOWNLOAD_STATE_DONE);

	self->priv->state = SUMMER_DOWNLOAD_STATE_DONE;
}

static void
set_failed (SummerDownload *self, ...)
{
	self->priv->state = SUMMER_DOWNLOAD_STATE_FAILED;
}
static void
summer_download_init (SummerDownload *self)
{
	self->priv = SUMMER_DOWNLOAD_GET_PRIVATE (self);
	g_object_set (self, "save-dir", default_save_dir, "tmp-dir", default_tmp_dir, NULL);
	g_signal_connect (self, "download-started", G_CALLBACK (set_started), NULL);
	g_signal_connect (self, "download-complete", G_CALLBACK (set_complete), NULL);
	g_signal_connect (self, "download-done", G_CALLBACK (set_done), NULL);
	g_signal_connect (self, "download-error", G_CALLBACK (set_failed), NULL);
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
	if (priv->filename)
		g_free (priv->filename);
	if (priv->item)
		g_object_unref (G_OBJECT (priv->item));
	G_OBJECT_CLASS(summer_download_parent_class)->finalize (self);
}

/**
 * summer_download_set_default:
 * @tmp_dir: the directory to save temporary files in, or %NULL to keep current
 * value.
 * @save_dir: the directory to save completed downloads in, or %NULL to keep
 * current value.
 *
 * Set default options for all new SummerDownload objects.
 *
 * These options may be set at any time during the session. You may change these
 * as you wish after downloads have been started, but note that changing these
 * will only affect downloads started after the change. These options can be
 * individually overridden by editing the SummerDownload::tmp-dir and 
 * SummerDownload::save-dir properties.
 */
void
summer_download_set_default (const gchar *tmp_dir, const gchar *save_dir)
{
	if (tmp_dir != NULL) {
		if (default_tmp_dir)
			g_free (default_tmp_dir);
		default_tmp_dir = g_strdup (tmp_dir);
	}
	if (save_dir != NULL) {
		if (default_save_dir)
			g_free (default_save_dir);
		default_save_dir = g_strdup (save_dir);
	}
}

/**
 * summer_download_start:
 * @obj: a #SummerDownload instance
 * @error: a #GError, or %NULL
 *
 * Start the file transfer
 */
void
summer_download_start (SummerDownload *obj, GError **error)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD (obj));
	SUMMER_DOWNLOAD_GET_CLASS (obj)->start (obj, error);
}

/**
 * summer_download_abort:
 * @obj: a #SummerDownload instance
 *
 * Aborts the file transfer.
 */
void
summer_download_abort (SummerDownload *obj)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD (obj));
	SUMMER_DOWNLOAD_GET_CLASS (obj)->abort (obj);
}

/**
 * summer_download_set_save_dir:
 * @self: a #SummerDownload instance
 * @save_dir: a directory path
 *
 * Changes the directory to store completed downloads in.
 */
void
summer_download_set_save_dir (SummerDownload *self, gchar *save_dir)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD (self));
	g_object_set (self, "save-dir", save_dir, NULL);
}

/**
 * summer_download_get_save_dir:
 * @self: a #SummerDownload instance
 *
 * Returns the directory where completed downloads are stored. You must free the
 * returned string yourself.
 *
 * Returns: the directory where completed downloads are stored
 */
gchar*
summer_download_get_save_dir (SummerDownload *self)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), NULL);
	gchar *save_dir;
	g_object_get (self, "save-dir", &save_dir, NULL);
	return save_dir;
}

/**
 * summer_download_set_tmp_dir:
 * @self: a #SummerDownload instance
 * @tmp_dir: a directory path
 *
 * Changes the directory to store temporary files (for instance incomplete 
 * downloads) in.
 */
void
summer_download_set_tmp_dir (SummerDownload *self, gchar *tmp_dir)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD (self));
	g_object_set (self, "tmp-dir", tmp_dir, NULL);
}

/**
 * summer_download_get_tmp_dir:
 * @self: a #SummerDownload instance
 *
 * Returns the directory where incomplete downloads and other temporary files
 * are stored. You must free the returned string yourself.
 *
 * Returns: the directory where temporary files are stored
 */
gchar*
summer_download_get_tmp_dir (SummerDownload *self)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), NULL);
	gchar *tmp_dir;
	g_object_get (self, "tmp-dir", &tmp_dir, NULL);
	return tmp_dir;
}

/**
 * summer_download_set_filename:
 * @self: a #SummerDownload instance
 * @filename: the filename to set. May not be %NULL.
 *
 * Sets the desired filename. Has no effect after the download is complete.
 */
void
summer_download_set_filename (SummerDownload *self, gchar *filename)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD (self));
	g_return_if_fail (filename == NULL);
	if (self->priv->state >= SUMMER_DOWNLOAD_STATE_SEEDING)
		return;
	g_object_set (self, "filename", filename, NULL);
}

/**
 * summer_download_get_filename:
 * @self: a #SummerDownload instance
 *
 * Returns the filename that is/will be the name of the completed file.
 *
 * Returns: the name of the completed file
 */
gchar*
summer_download_get_filename (SummerDownload *self)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), NULL);
	gchar *filename;
	g_object_get (self, "filename", &filename, NULL);
	return filename;
}

/**
 * summer_download_get_save_path:
 * @self: a #SummerDownload instance
 *
 * Returns the full path to where the download is stored. You should not attempt
 * to call this function unless #SummerDownload::download-started has been
 * emitted, or the information you receive might be bogus.
 *
 * Returns: The path to the completed download
 */
gchar*
summer_download_get_save_path (SummerDownload *self)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), NULL);
	gchar *filename, *save_dir, *path;
	filename = summer_download_get_filename (self);
	save_dir = summer_download_get_save_dir (self);
	path = g_build_filename (save_dir, filename, NULL);
	g_free (filename);
	g_free (save_dir);
	return path;
}

/**
 * summer_download_delete:
 * @self: a #SummerDownload object
 * @error: a #GError pointer
 *
 * Deletes the downloaded file.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 */
gboolean
summer_download_delete (SummerDownload *self, GError **error)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), FALSE);
	gchar *filename, *dir, *path;
	filename = summer_download_get_filename (self);
	if (self->priv->state >= SUMMER_DOWNLOAD_STATE_SEEDING)
		dir = summer_download_get_save_dir (self);
	else
		dir = summer_download_get_tmp_dir (self);
	path = g_build_filename (dir, filename, NULL);
	g_free (filename);
	g_free (dir);
	GFile *tmpfile = g_file_new_for_path (path);
	GError *tmp_error = NULL;
	g_file_delete (tmpfile, NULL, &tmp_error);
	g_free (path);
	g_object_unref (tmpfile);
	if (tmp_error != NULL) {
		g_propagate_error (error, tmp_error);
	}
	return tmp_error == NULL;
}

/**
 * SummerDownloadState:
 * @SUMMER_DOWNLOAD_STATE_UNKNOWN: This is the default value, and usually means
 * the download hasn't started yet
 * @SUMMER_DOWNLOAD_STATE_DOWNLOADING: The download is being downloaded
 * @SUMMER_DOWNLOAD_STATE_FAILED: The download could not be completed
 * @SUMMER_DOWNLOAD_STATE_SEEDING: The download has been downloaded, and is now
 * seeding. This only applies to bittorrent downloads
 * @SUMMER_DOWNLOAD_STATE_DONE: Summer will no longer do anything to the
 * download
 *
 * An enum of different download states. Not all downloads trigger all states,
 * however, downloads should normally always move from lower value states to
 * higher value states.
 */

/**
 * summer_download_get_state:
 * @self: a #SummerDownload object
 *
 * Retrieves the state of the download.
 * Returns: the current state mask
 */
SummerDownloadState
summer_download_get_state (SummerDownload *self)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), 0);

	SummerDownloadState state;
	g_object_get (self, "state", &state, NULL);
	return state;
}

/**
 * summer_download_is_paused:
 * @self: a #SummerDownload object
 *
 * Returns whether the download is paused or not.
 * Returns: %TRUE if the download is paused, %FALSE otherwise.
 */
gboolean
summer_download_is_paused (SummerDownload *self)
{
	g_return_val_if_fail (SUMMER_IS_DOWNLOAD (self), FALSE);

	gboolean paused;
	g_object_get (self, "paused", &paused, NULL);
	return paused;
}

/**
 * summer_download_set_paused:
 * @self: a #SummerDownload object
 * @pause: whether to pause or resume the download
 *
 * Pauses or resumes the download.
 */
void
summer_download_set_paused (SummerDownload *self, gboolean pause)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD (self));

	g_object_set (self, "paused", pause, NULL);
}


GQuark
summer_download_error_quark (void)
{
	return g_quark_from_static_string ("summer-download-error-quark");
}
