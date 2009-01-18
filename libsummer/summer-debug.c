/* summer-debug.c */

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

#include "summer-debug.h"

/**
 * summer_debug:
 * @string: a debug string. This string may contain %printf() style formating.
 * @...: the parameters to insert into the debug string.
 *
 * This function let's the library conditionally print debug messages.
 * These will only be printed if the environment variable %SUMMER_DEBUG is set.
 *
 * Returns: %TRUE if debugging is activated, otherwise %FALSE
 */
gboolean
summer_debug (gchar *string, ...) {
	if (g_getenv ("SUMMER_DEBUG") != NULL) {
		if (string != NULL) {
			va_list args;
			va_start (args, string);
			g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, string, args);
			va_end (args);
		}
		return TRUE;
	}
	return FALSE;
}
