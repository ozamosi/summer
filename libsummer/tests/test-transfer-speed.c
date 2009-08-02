#include <glib.h>
#include <glib-object.h>
#include <libsummer/summer-transfer-speed.h>

static void
basic ()
{
	SummerTransferSpeed *speed = summer_transfer_speed_new ();
	g_assert (speed != NULL);
	g_assert_cmpfloat (0.0, ==, summer_transfer_speed_get (speed));
	summer_transfer_speed_add_datapoint (speed, 10);
	g_assert_cmpfloat (10.0, ==, summer_transfer_speed_get (speed));
	summer_transfer_speed_add_datapoint (speed, 30);
	summer_transfer_speed_add_datapoint (speed, 30);
	summer_transfer_speed_add_datapoint (speed, 30);
	summer_transfer_speed_add_datapoint (speed, 30);
	summer_transfer_speed_add_datapoint (speed, 30);
	g_assert_cmpfloat (0.0, ==, summer_transfer_speed_get (speed));
	/* Make sure the last datapoint is not the oldest datapoint */
	summer_transfer_speed_add_datapoint (speed, 1000);
	summer_transfer_speed_add_datapoint (speed, 1000);
	summer_transfer_speed_add_datapoint (speed, 1000);
	g_usleep (0.5 * G_USEC_PER_SEC);
	summer_transfer_speed_add_datapoint (speed, 5030);
	/* It's slightly more than 0.5 seconds between calls, so speed should be
	 * slightly less than if exact. */
	g_assert_cmpfloat (10000.0, >=, summer_transfer_speed_get (speed));
	g_assert_cmpfloat (9000.0, <=, summer_transfer_speed_get (speed));
	summer_transfer_speed_destroy (&speed);
	g_assert (speed == NULL);
}

int main (int argc, char *argv[]) {
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/transfer-speed/basic", basic);

	return g_test_run ();
}
