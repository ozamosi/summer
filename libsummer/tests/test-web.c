#include <glib.h>
#include <libsummer/summer-web-backend.h>
#include "server.h"

int main (int argc, char* argv[]) {
	g_test_init (&argc, &argv, NULL);

	g_test_add ("/web-backend/to-ram", WebFixture, 0, web_setup, to_ram, web_teardown);
	//g_test_add ("/web-backend/to-disk", WebFixture, 0, web_setup, to_disk, web_teardown);

	return g_test_run ();
}
