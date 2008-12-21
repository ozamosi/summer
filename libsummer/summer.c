/* summer.c */

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

#include "summer.h"

/*
 * All functions in this file should use gmodule to find available
 * modules to call. Instead, they use a long list of ugly if conditions.
 */

/**
 * SECTION:summer
 * @short_description: Miscellary utility functions
 * @stability: Unstable
 *
 * A set of utility functions for general usage
 */

/**
 * summer_set:
 * @module_name: the name of the module to edit
 * @first_property_name: the name of the first property
 * @...: value for the first property, followed optionally by more name/value
 * pairs, followed by %NULL.
 *
 * Allow the user to configure global, module-specific settings, such
 * as directory to save downloaded files to, cache directory, and similar.
 *
 * Whenever it is possible, you should be able to override this on an instance 
 * basis.
 */

void
summer_set (gchar *module_name, gchar *first_property_name, ...)
{
	va_list var_args;
	if (!g_strcmp0 (module_name, "feed")) {
		va_start (var_args, first_property_name);
//		summer_feed_set (first_property_name, var_args);
		va_end (var_args);
	} else if (!g_strcmp0 (module_name, "download")) {
		va_start (var_args, first_property_name);
		summer_download_set (first_property_name, var_args);
		va_end (var_args);
	} else {
		g_error ("Tried to set properties on %s, which isn't a module", module_name);
	}
}

/**
 * summer_create_download:
 * @mime: the mime type of the download (you're supposed to get this from the
 * feed).
 * @url: the url you want to download.
 *
 * A factory function that returns a %SummerDownload suited for handling the 
 * @mime and the @url requested.
 *
 * Returns: a %SummerDownload if one could be created, otherwise %NULL
 */

SummerDownload*
summer_create_download (gchar *mime, gchar *url) {
	SummerDownload *dl;
	if ((dl = summer_download_web_new (mime, url)))
		return dl;
	return NULL;
}
