#ifndef __SERVER_H__
#define __SERVER_H__

#include <libsoup/soup.h>

guint PORT;

typedef struct {
	SoupServer *server;
} WebFixture;

void feed_server (SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query, SoupClientContext *client, gpointer user_data);

void web_setup (WebFixture *fix, gconstpointer data);
void web_teardown (WebFixture *fix, gconstpointer data);

#endif /* __SERVER_H__ */
