/* summer-web-backend.h */

/* This file is part of summer_web_backend.
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
/* other include files */

G_BEGIN_DECLS

/* convenience macros */
#define SUMMER_TYPE_WEB_BACKEND             (summer_web_backend_get_type())
#define SUMMER_WEB_BACKEND(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_WEB_BACKEND,SummerWebBackend))
#define SUMMER_WEB_BACKEND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_WEB_BACKEND,SummerWebBackendClass))
#define SUMMER_IS_WEB_BACKEND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_WEB_BACKEND))
#define SUMMER_IS_WEB_BACKEND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_WEB_BACKEND))
#define SUMMER_WEB_BACKEND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_WEB_BACKEND,SummerWebBackendClass))

typedef struct _SummerWebBackend      SummerWebBackend;
typedef struct _SummerWebBackendClass SummerWebBackendClass;

struct _SummerWebBackend {
	 GObject parent;
	/* insert public members, if any */
};

struct _SummerWebBackendClass {
	GObjectClass parent_class;
	/* insert signal callback declarations, eg. */
	/* void (* my_event) (SummerWebBackend* obj); */
};

/* member functions */
GType        summer_web_backend_get_type    (void) G_GNUC_CONST;

/* typical parameter-less _new function */
/* if this is a kind of GtkWidget, it should probably return at GtkWidget* */
SummerWebBackend*    summer_web_backend_new         (void);

/* fill in other public functions, eg.: */
/* 	void       summer_web_backend_do_something (SummerWebBackend *self, const gchar* param); */
/* 	gboolean   summer_web_backend_has_foo      (SummerWebBackend *self, gint value); */


G_END_DECLS

#endif /* __SUMMER_WEB_BACKEND_H__ */

