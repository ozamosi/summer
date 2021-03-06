/* summer-download.h */

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

#ifndef __SUMMER_DOWNLOAD_H__
#define __SUMMER_DOWNLOAD_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define SUMMER_TYPE_DOWNLOAD             (summer_download_get_type())
#define SUMMER_DOWNLOAD(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_DOWNLOAD,SummerDownload))
#define SUMMER_DOWNLOAD_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_DOWNLOAD,SummerDownloadClass))
#define SUMMER_IS_DOWNLOAD(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_DOWNLOAD))
#define SUMMER_IS_DOWNLOAD_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_DOWNLOAD))
#define SUMMER_DOWNLOAD_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_DOWNLOAD,SummerDownloadClass))

typedef struct _SummerDownload        SummerDownload;
typedef struct _SummerDownloadClass   SummerDownloadClass;
typedef struct _SummerDownloadPrivate SummerDownloadPrivate;

struct _SummerDownload {
	GObject parent;
	SummerDownloadPrivate *priv;
};

struct _SummerDownloadClass {
	GObjectClass parent_class;
	void (* start) (SummerDownload *self, GError **error);
	void (* abort) (SummerDownload *self);
	void (* download_complete) (SummerDownload *obj);
	void (* download_done) (SummerDownload *obj);
	void (* download_update) (SummerDownload *obj, guint64 received, guint64 length);
	void (* download_started) (SummerDownload *obj);
	void (* download_error) (SummerDownload *obj, GError *error);
};

GType        summer_download_get_type    (void) G_GNUC_CONST;

typedef enum {
	SUMMER_DOWNLOAD_STATE_UNKNOWN = 10,
	SUMMER_DOWNLOAD_STATE_DOWNLOADING = 20,
	SUMMER_DOWNLOAD_STATE_FAILED = 30,
	SUMMER_DOWNLOAD_STATE_SEEDING = 40,
	SUMMER_DOWNLOAD_STATE_DONE = 50,
} SummerDownloadState;

void summer_download_set_default (const gchar *tmp_dir, const gchar *save_dir);

void summer_download_start (SummerDownload *obj, GError **error);
void summer_download_abort (SummerDownload *obj);

void summer_download_set_save_dir (SummerDownload *self, gchar *save_dir);
gchar* summer_download_get_save_dir (SummerDownload *self);

void summer_download_set_tmp_dir (SummerDownload *self, gchar *tmp_dir);
gchar* summer_download_get_tmp_dir (SummerDownload *self);

void summer_download_set_filename (SummerDownload *self, gchar *filename);
gchar* summer_download_get_filename (SummerDownload *self);

gchar* summer_download_get_save_path (SummerDownload *self);

gboolean summer_download_delete (SummerDownload *self, GError **error);

SummerDownloadState summer_download_get_state (SummerDownload *self);

gboolean summer_download_is_paused (SummerDownload *self);
void summer_download_set_paused (SummerDownload *self, gboolean pause);

gfloat summer_download_get_transfer_speed (SummerDownload *self);

#define SUMMER_DOWNLOAD_ERROR summer_download_error_quark ()
GQuark summer_download_error_quark (void);

enum SummerDownloadError {
	SUMMER_DOWNLOAD_ERROR_OUTPUT,
	SUMMER_DOWNLOAD_ERROR_INPUT
};

G_END_DECLS

#endif /* __SUMMER_DOWNLOAD_H__ */
