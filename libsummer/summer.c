#include "summer.h"

void
summer_set (gchar* module_name, gchar* first_property_name, ...)
{
	va_list var_args;
	// This should be replaced by fancy GModule logic
	if (!g_strcmp0 (module_name, "feed-downloader")) {
		va_start (var_args, first_property_name);
		summer_feed_downloader_set (first_property_name, var_args);
		va_end (var_args);
	} else if (!g_strcmp0 (module_name, "downloader")) {
		va_start (var_args, first_property_name);
		summer_downloader_set (first_property_name, var_args);
		va_end (var_args);
	} else {
		g_error ("Tried to set properties on %s, which isn't a module", module_name);
	}
}
