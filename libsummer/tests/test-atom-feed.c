#include <glib.h>
#include <libsummer/summer-atom-parser.h>
#include <libxml/xmlreader.h>

static void
property ()
{
	SummerAtomParser *parser = summer_atom_parser_new ();
	GValueArray *ns;
	g_object_get (parser, "handled-namespaces", &ns, NULL);
	g_assert_cmpint (2, ==, ns->n_values);
	g_assert_cmpstr (g_value_get_string (g_value_array_get_nth (ns, 0)), ==, "http://www.w3.org/2005/Atom");
	g_assert_cmpstr (g_value_get_string (g_value_array_get_nth (ns, 1)), ==, "http://purl.org/atom/ns#");
}

static void
parse ()
{
	SummerFeedParser *parser = SUMMER_FEED_PARSER (summer_atom_parser_new ());
	xmlTextReaderPtr reader = xmlNewTextReaderFilename ("atom_feed");
	SummerFeedData *feed_data = summer_feed_data_new ();
	gboolean item = FALSE;
	GList *items = NULL;
	if (reader != NULL) {
		while (xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderRead (reader);
		}
		int ret;
		while (ret >= 0) {
			ret = summer_feed_parser_handle_feed_node (parser, reader, feed_data, &item);
			if (item) {
				SummerItemData *item_data = summer_item_data_new ();
				int depth = xmlTextReaderDepth (reader) -1;
				do {
					ret = summer_feed_parser_handle_item_node (parser, reader, item_data);
					if (ret == 0) {
						ret =xmlTextReaderRead (reader);
						while (ret > 0 && xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
							ret =xmlTextReaderRead (reader);
						}
					}
				} while (ret >= 0 && xmlTextReaderDepth (reader) > depth);
				items = g_list_append (items, item_data);
			}
			if (ret == 0) {
				ret = xmlTextReaderRead (reader);
				while (ret > 0 && xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
					ret = xmlTextReaderRead (reader);
				}
			}
		}
	}
	g_assert_cmpstr (feed_data->title, ==, "Example Feed");
	g_assert_cmpstr (feed_data->description, ==, NULL);
	g_assert_cmpstr (feed_data->id, ==, "urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6");
	g_assert_cmpstr (feed_data->web_url, ==, "http://example.org/");
	GTimeVal time;
	g_time_val_from_iso8601 ("2003-12-13T18:30:02Z", &time);
	g_assert_cmpint (feed_data->updated.tv_sec, ==, time.tv_sec);
	g_assert_cmpint (feed_data->updated.tv_usec, ==, time.tv_usec);
	g_assert_cmpstr (feed_data->author, ==, "John Doe");

	g_assert_cmpint (g_list_length (items), ==, 1);
	SummerItemData *item_data = (SummerItemData *)items->data;
	g_assert_cmpstr (item_data->title, ==, "Atom-Powered Robots Run Amok");
	g_assert_cmpstr (item_data->description, ==, "Some text.");
	g_assert_cmpstr (item_data->id, ==, "urn:uuid:1225c695-cfb8-4ebb-aaaa-80da344efa6a");
	g_assert_cmpstr (item_data->web_url, ==, "http://example.org/2003/12/13/atom03");
	g_time_val_from_iso8601 ("2003-12-13T18:30:02Z", &time);	
	g_assert_cmpint (item_data->updated.tv_sec, ==, time.tv_sec);
	g_assert_cmpint (item_data->updated.tv_usec, ==, time.tv_usec);
	summer_feed_data_free (feed_data);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/atom-feed/property", property);
	g_test_add_func ("/atom-feed/parse", parse);
	
	return g_test_run ();
}
