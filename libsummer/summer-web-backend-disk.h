/* summer-web-backend-disk.h */

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

#ifndef _SUMMER_WEB_BACKEND_DISK_H
#define _SUMMER_WEB_BACKEND_DISK_H

#include <glib-object.h>
#include "summer-web-backend.h"
G_BEGIN_DECLS

#define SUMMER_TYPE_WEB_BACKEND_DISK summer_web_backend_disk_get_type()

#define SUMMER_WEB_BACKEND_DISK(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), \
	SUMMER_TYPE_WEB_BACKEND_DISK, SummerWebBackendDisk))

#define SUMMER_WEB_BACKEND_DISK_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), \
	SUMMER_TYPE_WEB_BACKEND_DISK, SummerWebBackendDiskClass))

#define SUMMER_IS_WEB_BACKEND_DISK(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
	SUMMER_TYPE_WEB_BACKEND_DISK))

#define SUMMER_IS_WEB_BACKEND_DISK_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), \
	SUMMER_TYPE_WEB_BACKEND_DISK))

#define SUMMER_WEB_BACKEND_DISK_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), \
	SUMMER_TYPE_WEB_BACKEND_DISK, SummerWebBackendDiskClass))

typedef struct _SummerWebBackendDisk SummerWebBackendDisk;
typedef struct _SummerWebBackendDiskClass SummerWebBackendDiskClass;
typedef struct _SummerWebBackendDiskPrivate SummerWebBackendDiskPrivate;

struct _SummerWebBackendDisk
{
	SummerWebBackend parent;

	SummerWebBackendDiskPrivate *priv;
};

struct _SummerWebBackendDiskClass
{
	SummerWebBackendClass parent_class;
};

GType summer_web_backend_disk_get_type (void);

SummerWebBackend *summer_web_backend_disk_new (const gchar *url, const gchar *save_dir);

G_END_DECLS

#endif /* _SUMMER_WEB_BACKEND_DISK_H */
