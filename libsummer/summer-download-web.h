/* summer-download-web.h */

/* This file is part of summer_download_web.
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

#ifndef __SUMMER_DOWNLOAD_WEB_H__
#define __SUMMER_DOWNLOAD_WEB_H__

#include <glib-object.h>
/* other include files */

G_BEGIN_DECLS

/* convenience macros */
#define SUMMER_TYPE_DOWNLOAD_WEB             (summer_download_web_get_type())
#define SUMMER_DOWNLOAD_WEB(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_DOWNLOAD_WEB,SummerDownloadWeb))
#define SUMMER_DOWNLOAD_WEB_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_DOWNLOAD_WEB,SummerDownloadWebClass))
#define SUMMER_IS_DOWNLOAD_WEB(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_DOWNLOAD_WEB))
#define SUMMER_IS_DOWNLOAD_WEB_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_DOWNLOAD_WEB))
#define SUMMER_DOWNLOAD_WEB_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_DOWNLOAD_WEB,SummerDownloadWebClass))

typedef struct _SummerDownloadWeb      SummerDownloadWeb;
typedef struct _SummerDownloadWebClass SummerDownloadWebClass;

struct _SummerDownloadWeb {
	 GObject parent;
	/* insert public members, if any */
};

struct _SummerDownloadWebClass {
	GObjectClass parent_class;
	/* insert signal callback declarations, eg. */
	/* void (* my_event) (SummerDownloadWeb* obj); */
};

/* member functions */
GType        summer_download_web_get_type    (void) G_GNUC_CONST;

/* typical parameter-less _new function */
/* if this is a kind of GtkWidget, it should probably return at GtkWidget* */
SummerDownloadWeb*    summer_download_web_new         (void);

/* fill in other public functions, eg.: */
/* 	void       summer_download_web_do_something (SummerDownloadWeb *self, const gchar* param); */
/* 	gboolean   summer_download_web_has_foo      (SummerDownloadWeb *self, gint value); */


G_END_DECLS

#endif /* __SUMMER_DOWNLOAD_WEB_H__ */

