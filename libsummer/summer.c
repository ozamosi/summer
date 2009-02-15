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
#include "summer-debug.h"
#include "summer-download-torrent.h"
#include "summer-download-youtube.h"
#include "summer-download-web.h"
#include "summer-feed-cache.h"

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
 * summer_create_download:
 * @item: a %SummerItemData, containing information about the download.
 *
 * A factory function that returns a %SummerDownload suited for handling the 
 * download specified in @item.
 *
 * Returns: a %SummerDownload if one could be created, otherwise %NULL
 */

SummerDownload*
summer_create_download (SummerItemData *item)
{
	SummerDownload *dl;
	if ((dl = summer_download_torrent_new (item)))
		summer_debug ("Starting torrent download of '%s'", item->title);
	else if ((dl = summer_download_web_new (item)))
		summer_debug ("Starting web download of '%s'", item->title);

	return dl;
}

/**
 * summer_shutdown:
 *
 * Shuts down active connections and frees resources.
 */

void
summer_shutdown ()
{
	summer_download_torrent_shutdown ();
}
