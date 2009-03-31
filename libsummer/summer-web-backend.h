/* summer-web-backend.h */

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

#ifndef __SUMMER_WEB_BACKEND_H__
#define __SUMMER_WEB_BACKEND_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define SUMMER_TYPE_WEB_BACKEND             (summer_web_backend_get_type())
#define SUMMER_WEB_BACKEND(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_WEB_BACKEND,SummerWebBackend))
#define SUMMER_WEB_BACKEND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_WEB_BACKEND,SummerWebBackendClass))
#define SUMMER_IS_WEB_BACKEND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_WEB_BACKEND))
#define SUMMER_IS_WEB_BACKEND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_WEB_BACKEND))
#define SUMMER_WEB_BACKEND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_WEB_BACKEND,SummerWebBackendClass))

typedef struct _SummerWebBackend        SummerWebBackend;
typedef struct _SummerWebBackendClass   SummerWebBackendClass;
typedef struct _SummerWebBackendPrivate SummerWebBackendPrivate;

struct _SummerWebBackend {
	GObject parent;
	SummerWebBackendPrivate *priv;
};

struct _SummerWebBackendClass {
	GObjectClass parent_class;
	void (* download_complete) (SummerWebBackend *obj, gchar *save_path, gchar *save_data, gpointer user_data);
	void (* download_chunk) (SummerWebBackend *obj, gint received, gint length, gpointer user_data);
	void (* headers_parsed) (SummerWebBackend *obj, gpointer user_data);
};

GType        summer_web_backend_get_type    (void) G_GNUC_CONST;

SummerWebBackend*    summer_web_backend_new         (const gchar *save_dir, const gchar *url);

void summer_web_backend_fetch (SummerWebBackend *self);
void summer_web_backend_fetch_head (SummerWebBackend *self);
void summer_web_backend_abort (SummerWebBackend *self);

G_END_DECLS

#endif /* __SUMMER_WEB_BACKEND_H__ */
