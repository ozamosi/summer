#include <glib.h>
#include <libsummer/summer-rss2-parser.h>
#include <libxml/xmlreader.h>

static void
property ()
{
	SummerRss2Parser *parser = summer_rss2_parser_new ();
	GValueArray *ns;
	g_object_get (parser, "handled-namespaces", &ns, NULL);
	g_assert_cmpint (0, ==, ns->n_values);
}

static void
parse ()
{
	SummerFeedParser *parser = SUMMER_FEED_PARSER (summer_rss2_parser_new ());
	xmlTextReaderPtr reader = xmlNewTextReaderFilename ("rss2sample.xml");
	SummerFeedData *feed_data = summer_feed_data_new ();
	gboolean item = FALSE;
	GList *items = NULL;
	if (reader != NULL) {
		while (xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderRead (reader);
		}
		int ret = 0;
		while (ret >= 0) {
			ret = summer_feed_parser_handle_feed_node (parser, reader, feed_data, &item);
			if (item) {
				SummerItemData *item_data = summer_item_data_new ();
				int depth = xmlTextReaderDepth (reader);
				do {
					ret = summer_feed_parser_handle_item_node (parser, reader, item_data);
					if (ret == 0) {
						ret = xmlTextReaderRead (reader);
						while (ret > 0 && xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
							ret = xmlTextReaderRead (reader);
						}
					}
				} while (ret >= 0 && xmlTextReaderDepth (reader) > depth);
				items = g_list_append (items, item_data);
			}
			if (ret == 0) {
				ret = xmlTextReaderRead (reader) - 1;
				while (ret >= 0 && xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
					ret = xmlTextReaderRead (reader) - 1;
				}
			}
		}
	}
	g_assert_cmpstr (feed_data->title, ==, "Liftoff News");
	g_assert_cmpstr (feed_data->description, ==, "Liftoff to Space Exploration.");
	g_assert_cmpstr (feed_data->id, ==, NULL);
	g_assert_cmpstr (feed_data->web_url, ==, "http://liftoff.msfc.nasa.gov/");
	GTimeVal time;
	g_time_val_from_iso8601 ("2003-06-10T09:41:01Z", &time);
	g_assert_cmpint (feed_data->updated.tv_sec, ==, time.tv_sec);
	g_assert_cmpint (feed_data->updated.tv_usec, ==, 0);
	g_assert_cmpstr (feed_data->author, ==, "editor@example.com");

	g_assert_cmpint (g_list_length (items), ==, 4);
	SummerItemData *item_data = (SummerItemData *)items->data;
	g_assert_cmpstr (item_data->title, ==, "Star City");
	g_assert_cmpstr (item_data->description, ==, "How do Americans get ready to work with Russians aboard the International Space Station? They take a crash course in culture, language and protocol at Russia's <a href=\"http://howe.iki.rssi.ru/GCTC/gctc_e.htm\">Star City</a>.");
	g_assert_cmpstr (item_data->id, ==, "http://liftoff.msfc.nasa.gov/2003/06/03.html#item573");
	g_assert_cmpstr (item_data->web_url, ==, "http://liftoff.msfc.nasa.gov/news/2003/news-starcity.asp");
	summer_feed_data_free (feed_data);
	summer_item_data_free (item_data);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/rss2-feed/property", property);
	g_test_add_func ("/rss2-feed/parse", parse);
	
	return g_test_run ();
}
