/* summer-web-backend-ram.c */

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

#include "summer-web-backend-ram.h"

G_DEFINE_TYPE (SummerWebBackendRam, summer_web_backend_ram, SUMMER_TYPE_WEB_BACKEND)

/**
 * SECTION:summer-web-backend-ram
 * @short_description: Downloads files to ram
 * @stability: Private
 * @include: libsummer/summer-web-backend-ram.h
 *
 * This component is only meant to be used by the downloaders. It contains
 * #SummerWebBackendRam which downloads files to ram.
 */

/**
 * SummerWebBackendRam:
 *
 * An class that downloads HTTP resources to RAM. Should only be used for small,
 * temporary files.
 */

static void
summer_web_backend_ram_finalize (GObject *object)
{
	G_OBJECT_CLASS (summer_web_backend_ram_parent_class)->finalize (object);
}

static void
on_downloaded (SummerWebBackend *self, SoupSession *session, SoupMessage *msg)
{
	g_return_if_fail (SUMMER_IS_WEB_BACKEND_RAM (self));
	g_return_if_fail (SOUP_IS_SESSION (session));
	g_return_if_fail (SOUP_IS_MESSAGE (msg));
	g_return_if_fail (msg->response_body);
	g_return_if_fail (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code));

	g_signal_emit_by_name (self,
		"download-complete",
		NULL,
		msg->response_body->data,
		NULL);
}

static void
summer_web_backend_ram_class_init (SummerWebBackendRamClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = summer_web_backend_ram_finalize;

	SummerWebBackendClass *wb_class = SUMMER_WEB_BACKEND_CLASS (klass);
	wb_class->on_downloaded = on_downloaded;
}

static void
summer_web_backend_ram_init (SummerWebBackendRam *self)
{
}

/**
 * summer_web_backend_ram_new:
 * @url: the url to download from.
 *
 * Creates a new #SummerWebBackend that will save it's downloaded content in RAM.
 *
 * Returns: a newly created #SummerWebBackend object
 */
SummerWebBackend *
summer_web_backend_ram_new (const gchar *url)
{
	return g_object_new (SUMMER_TYPE_WEB_BACKEND_RAM, "url", url, NULL);
}
