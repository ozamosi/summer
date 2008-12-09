#include <libsoup/soup.h>

typedef struct {
	SoupServer *server;
} WebFixture;

static void feed_server (SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query, SoupClientContext *client, gpointer user_data);

static void web_setup (WebFixture *fix, gconstpointer data);
static void web_teardown (WebFixture *fix, gconstpointer data);
