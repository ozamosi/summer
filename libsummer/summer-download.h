/* summer-download.h */

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
	void (* start) (SummerDownload *self);
	void (* download_complete) (SummerDownload *obj, gchar *save_path, gpointer user_data);
	void (* download_update) (SummerDownload *obj, guint64 received, guint64 length, gpointer user_data);
	void (* download_started) (SummerDownload *obj, gpointer user_data);
};

GType        summer_download_get_type    (void) G_GNUC_CONST;

void summer_download_set_default (const gchar *tmp_dir, const gchar *save_dir);

void summer_download_start (SummerDownload *obj);

void summer_download_set_save_dir (SummerDownload *self, gchar *save_dir);
gchar* summer_download_get_save_dir (SummerDownload *self);

void summer_download_set_tmp_dir (SummerDownload *self, gchar *tmp_dir);
gchar* summer_download_get_tmp_dir (SummerDownload *self);

G_END_DECLS

#endif /* __SUMMER_DOWNLOAD_H__ */
