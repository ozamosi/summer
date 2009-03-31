#include <glib.h>
#include <libsummer/summer.h>
#include <libsummer/summer-data-types.h>
#include <libsummer/summer-feed-cache.h>

static GList*
append_uri (GList *list, gchar *uri) {
	SummerFeedData *feed = summer_feed_data_new ();
	SummerItemData *item = summer_feed_data_append_item (feed);
	item->id = g_strdup (uri);
	list = g_list_append (list, item);
	return list;
}

static void
basic ()
{
	gchar *path = g_build_filename (g_get_tmp_dir (), "feedcache", NULL);
	SummerFeedCache *cache = summer_feed_cache_get ();
	g_object_set (cache, "cache-file", path, NULL);
	GList *list = NULL;

	list = append_uri (list, "urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6");
	summer_feed_cache_filter_old_items (cache, &list);
	g_assert_cmpint (g_list_length (list), ==, 1);
	
	summer_feed_cache_filter_old_items (cache, &list);
	g_assert_cmpint (g_list_length (list), ==, 0);
	
	list = append_uri (list, "urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6");
	list = append_uri (list, "urn:uuid:1225c695-cfb8-4ebb-aaaa-80da344efa6a");
	summer_feed_cache_filter_old_items (cache, &list);
	g_assert_cmpint (g_list_length (list), ==, 1);
	
	list = append_uri (list, "urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6");
	list = append_uri (list, "urn:uuid:1225c695-cfb8-4ebb-aaaa-80da344efa6a");
	list = append_uri (list, "http://epicfu.com/2008/12/how-to-make-a-kick-ass-web-sho.html");
	list = append_uri (list, "http://epicfu.com/2008/12/get-ready-for-the-end-of-the-w.html");
	summer_feed_cache_filter_old_items (cache, &list);
	g_assert_cmpint (g_list_length (list), ==, 2);

	g_object_unref (cache);
	g_remove (path);
	g_free (path);
}

int
main (int argc, char *argv[])
{
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/feed-cache/basic", basic);
	return g_test_run ();
}
