/* summer-xml-backend.h */

/* This file is part of summer_xml_backend.
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

#ifndef __SUMMER_XML_BACKEND_H__
#define __SUMMER_XML_BACKEND_H__

#include <glib-object.h>
/* other include files */

G_BEGIN_DECLS

/* convenience macros */
#define SUMMER_TYPE_XML_BACKEND             (summer_xml_backend_get_type())
#define SUMMER_XML_BACKEND(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_XML_BACKEND,SummerXmlBackend))
#define SUMMER_XML_BACKEND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_XML_BACKEND,SummerXmlBackendClass))
#define SUMMER_IS_XML_BACKEND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_XML_BACKEND))
#define SUMMER_IS_XML_BACKEND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_XML_BACKEND))
#define SUMMER_XML_BACKEND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_XML_BACKEND,SummerXmlBackendClass))

typedef struct _SummerXmlBackend      SummerXmlBackend;
typedef struct _SummerXmlBackendClass SummerXmlBackendClass;

struct _SummerXmlBackend {
	 GObject parent;
	/* insert public members, if any */
};

struct _SummerXmlBackendClass {
	GObjectClass parent_class;
	/* insert signal callback declarations, eg. */
	/* void (* my_event) (SummerXmlBackend* obj); */
};

/* member functions */
GType        summer_xml_backend_get_type    (void) G_GNUC_CONST;

/* typical parameter-less _new function */
/* if this is a kind of GtkWidget, it should probably return at GtkWidget* */
SummerXmlBackend*    summer_xml_backend_new         (void);

/* fill in other public functions, eg.: */
/* 	void       summer_xml_backend_do_something (SummerXmlBackend *self, const gchar* param); */
/* 	gboolean   summer_xml_backend_has_foo      (SummerXmlBackend *self, gint value); */


G_END_DECLS

#endif /* __SUMMER_XML_BACKEND_H__ */

