#include <glib.h>
#include <libsummer/summer-atom-parser.h>
#include <libxml/xmlreader.h>

static void
parse ()
{
	SummerFeedParser *parsers[] = {SUMMER_FEED_PARSER (summer_atom_parser_new ())};
	xmlTextReaderPtr reader = xmlNewTextReaderFilename ("atom_feed");
	SummerFeedData *feed_data = summer_feed_parser_parse (parsers, sizeof (parsers) / sizeof (*parsers), reader);
	g_assert_cmpstr (feed_data->title, ==, "Example Feed");
	g_assert_cmpstr (feed_data->description, ==, NULL);
	g_assert_cmpstr (feed_data->id, ==, "urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6");
	g_assert_cmpstr (feed_data->web_url, ==, "http://example.org/");
	GTimeVal time;
	g_time_val_from_iso8601 ("2003-12-13T18:30:02Z", &time);
	g_assert_cmpint (feed_data->updated, ==, time.tv_sec);
	g_assert_cmpstr (feed_data->author, ==, "John Doe");

	g_assert_cmpint (g_list_length (feed_data->items), ==, 1);
	SummerItemData *item_data = (SummerItemData *)feed_data->items->data;
	g_assert_cmpstr (item_data->title, ==, "Atom-Powered Robots Run Amok");
	g_assert_cmpstr (item_data->description, ==, "Some text.");
	g_assert_cmpstr (item_data->id, ==, "urn:uuid:1225c695-cfb8-4ebb-aaaa-80da344efa6a");
	g_assert_cmpstr (item_data->web_url, ==, "http://example.org/2003/12/13/atom03");
	g_time_val_from_iso8601 ("2003-12-13T18:30:02Z", &time);	
	g_assert_cmpint (item_data->updated, ==, time.tv_sec);
	summer_feed_data_free (feed_data);
}

static void
broken_title ()
{
	SummerFeedParser *parsers[] = {SUMMER_FEED_PARSER (summer_atom_parser_new ())};
	xmlTextReaderPtr reader = xmlNewTextReaderFilename ("atom_feed_nolink");
	SummerFeedData *feed_data = summer_feed_parser_parse (parsers, sizeof (parsers) / sizeof (parsers), reader);
	g_assert_cmpstr (feed_data->title, ==, "Example Feed");
	GTimeVal time;
	g_time_val_from_iso8601 ("2003-12-13T18:30:02Z", &time);
	g_assert_cmpint (feed_data->updated, ==, time.tv_sec);
	g_assert (feed_data->web_url == NULL);

	summer_feed_data_free (feed_data);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/atom-feed/parse", parse);
	g_test_add_func ("/atom-feed/broken-title", broken_title);
	
	return g_test_run ();
}
