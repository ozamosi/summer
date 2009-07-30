/* summer-web-backend.h */

/* This file is part of libsummer.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
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

#ifndef __SUMMER_WEB_BACKEND_H__
#define __SUMMER_WEB_BACKEND_H__

#include <glib-object.h>
#include <libsoup/soup.h>

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
	void (* on_downloaded) (SummerWebBackend *obj, SoupSession *session, SoupMessage *msg);
	gboolean (* on_chunk) (SummerWebBackend *obj, SoupBuffer *chunk);
	void (* on_init) (SummerWebBackend *obj, SoupMessage *msg, GError **error);
	void (* on_error) (SummerWebBackend *obj);
};

GType        summer_web_backend_get_type    (void) G_GNUC_CONST;

void summer_web_backend_fetch (SummerWebBackend *self, GError **error);
void summer_web_backend_fetch_head (SummerWebBackend *self, GError **error);
void summer_web_backend_abort (SummerWebBackend *self);

gchar* summer_web_backend_get_remote_filename (SummerWebBackend *self);

#define SUMMER_WEB_BACKEND_ERROR summer_web_backend_error_quark ()
GQuark summer_web_backend_error_quark (void);

enum SummerWebBackendError {
	SUMMER_WEB_BACKEND_ERROR_INPUT,  /* Data from user (url:s) */
	SUMMER_WEB_BACKEND_ERROR_REMOTE, /* Data from remote server */
	SUMMER_WEB_BACKEND_ERROR_LOCAL   /* Local file errors */
};

G_END_DECLS

#endif /* __SUMMER_WEB_BACKEND_H__ */
