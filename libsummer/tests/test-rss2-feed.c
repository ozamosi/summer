#include <glib.h>
#include <libsummer/summer-rss2-parser.h>
#include <libxml/xmlreader.h>

static void
feedburner ()
{
	SummerFeedParser *parsers[] = {
		SUMMER_FEED_PARSER (summer_rss2_parser_new ()),
		SUMMER_FEED_PARSER (summer_atom_parser_new ())};
	xmlTextReaderPtr reader = xmlNewTextReaderFilename ("../../tests/epicfu");
	SummerFeedData *feed_data = summer_feed_parser_parse (parsers, sizeof (parsers) / sizeof (*parsers), reader);

	g_assert_cmpstr (feed_data->title, ==, "EPIC FU (Quicktime)");
	g_assert_cmpstr (feed_data->description, ==, "art . music . tech");
	g_assert_cmpstr (feed_data->id, ==, NULL);
	g_assert_cmpstr (feed_data->web_url, ==, "http://epicfu.com/");
	GTimeVal time;
	g_time_val_from_iso8601 ("2008-12-02T03:56:43 -0800", &time);
	g_assert_cmpint (feed_data->updated, ==, time.tv_sec);
	g_assert_cmpstr (feed_data->author, ==, NULL);

	g_assert_cmpint (g_list_length (feed_data->items), ==, 20);
	SummerItemData *item_data = (SummerItemData *)feed_data->items->data;
	g_assert_cmpstr (item_data->title, ==, "postcard secrets, killing plastic, emo hate");
	g_assert_cmpstr (item_data->id, ==, "http://epicfu.com/2008/08/postcard-secrets-killing-plast.html");
	g_assert_cmpstr (item_data->web_url, ==, "http://feeds.feedburner.com/~r/epicfu/~3/470132724/postcard-secrets-killing-plast.html");
	summer_feed_data_free (feed_data);
}


static void
parse ()
{
	SummerFeedParser *parsers[] = {SUMMER_FEED_PARSER (summer_rss2_parser_new ())};
	xmlTextReaderPtr reader = xmlNewTextReaderFilename ("rss2sample.xml");
	SummerFeedData *feed_data = summer_feed_parser_parse (parsers, sizeof (parsers) / sizeof (*parsers), reader);
	g_assert_cmpstr (feed_data->title, ==, "Liftoff News");
	g_assert_cmpstr (feed_data->description, ==, "Liftoff to Space Exploration.");
	g_assert_cmpstr (feed_data->id, ==, NULL);
	g_assert_cmpstr (feed_data->web_url, ==, "http://liftoff.msfc.nasa.gov/");
	GTimeVal time;
	g_time_val_from_iso8601 ("2003-06-10T09:41:01Z", &time);
	g_assert_cmpint (feed_data->updated, ==, time.tv_sec);
	g_assert_cmpstr (feed_data->author, ==, "editor@example.com");

	g_assert_cmpint (g_list_length (feed_data->items), ==, 4);
	SummerItemData *item_data = (SummerItemData *)feed_data->items->data;
	g_assert_cmpstr (item_data->title, ==, "Astronauts' Dirty Laundry");
	g_assert_cmpstr (item_data->id, ==, "http://liftoff.msfc.nasa.gov/2003/05/20.html#item570");
	item_data = (SummerItemData *)g_list_last (feed_data->items)->data;
	g_assert_cmpstr (item_data->title, ==, "Star City");
	g_assert_cmpstr (item_data->description, ==, "How do Americans get ready to work with Russians aboard the International Space Station? They take a crash course in culture, language and protocol at Russia's <a href=\"http://howe.iki.rssi.ru/GCTC/gctc_e.htm\">Star City</a>.");
	g_assert_cmpstr (item_data->id, ==, "http://liftoff.msfc.nasa.gov/2003/06/03.html#item573");
	g_assert_cmpstr (item_data->web_url, ==, "http://liftoff.msfc.nasa.gov/news/2003/news-starcity.asp");
	summer_feed_data_free (feed_data);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/rss2-feed/parse-easy", parse);
	g_test_add_func ("/rss2-feed/parse-feedburner", feedburner);
	
	return g_test_run ();
}
