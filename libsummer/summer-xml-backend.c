/* summer-xml-backend.c */

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

#include "summer-xml-backend.h"
/* include other impl specific header files */

/* 'private'/'protected' functions */
static void summer_xml_backend_class_init (SummerXmlBackendClass *klass);
static void summer_xml_backend_init       (SummerXmlBackend *obj);
static void summer_xml_backend_finalize   (GObject *obj);
/* list my signals  */
enum {
	/* MY_SIGNAL_1, */
	/* MY_SIGNAL_2, */
	LAST_SIGNAL
};

typedef struct _SummerXmlBackendPrivate SummerXmlBackendPrivate;
struct _SummerXmlBackendPrivate {
	/* my private members go here, eg. */
	/* gboolean frobnicate_mode; */
};
#define SUMMER_XML_BACKEND_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                SUMMER_TYPE_XML_BACKEND, \
                                                SummerXmlBackendPrivate))
/* globals */
static GObjectClass *parent_class = NULL;

/* uncomment the following if you have defined any signals */
/* static guint signals[LAST_SIGNAL] = {0}; */

GType
summer_xml_backend_get_type (void)
{
	static GType my_type = 0;
	if (!my_type) {
		static const GTypeInfo my_info = {
			sizeof(SummerXmlBackendClass),
			NULL,		/* base init */
			NULL,		/* base finalize */
			(GClassInitFunc) summer_xml_backend_class_init,
			NULL,		/* class finalize */
			NULL,		/* class data */
			sizeof(SummerXmlBackend),
			1,		/* n_preallocs */
			(GInstanceInitFunc) summer_xml_backend_init,
			NULL
		};
		my_type = g_type_register_static (G_TYPE_OBJECT,
		                                  "SummerXmlBackend",
		                                  &my_info, 0);
	}
	return my_type;
}

static void
summer_xml_backend_class_init (SummerXmlBackendClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	parent_class            = g_type_class_peek_parent (klass);
	gobject_class->finalize = summer_xml_backend_finalize;

	g_type_class_add_private (gobject_class, sizeof(SummerXmlBackendPrivate));

	/* signal definitions go here, e.g.: */
/* 	signals[MY_SIGNAL_1] = */
/* 		g_signal_new ("my_signal_1",....); */
/* 	signals[MY_SIGNAL_2] = */
/* 		g_signal_new ("my_signal_2",....); */
/* 	etc. */
}

static void
summer_xml_backend_init (SummerXmlBackend *obj)
{
/* uncomment the following if you init any of the private data */
/* 	SummerXmlBackendPrivate *priv = SUMMER_XML_BACKEND_GET_PRIVATE(obj); */

/* 	initialize this object, eg.: */
/* 	priv->frobnicate_mode = FALSE; */
}

static void
summer_xml_backend_finalize (GObject *obj)
{
/* 	free/unref instance resources here */
	G_OBJECT_CLASS(parent_class)->finalize (obj);
}

SummerXmlBackend*
summer_xml_backend_new (void)
{
	return SUMMER_XML_BACKEND(g_object_new(SUMMER_TYPE_XML_BACKEND, NULL));
}

/* insert many other interesting function implementations */
/* such as summer_xml_backend_do_something, or summer_xml_backend_has_foo */

