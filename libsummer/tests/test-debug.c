#include <glib.h>
#include <libsummer/summer-debug.h>

static void
dont_print ()
{
	if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT)) {
		summer_debug ("Test");
		exit (0);
	}
	g_test_trap_assert_passed ();
	g_test_trap_assert_stdout_unmatched ("*Test*");
}

static void
print ()
{
	if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT)) {
		g_setenv ("SUMMER_DEBUG", "1", TRUE);
		summer_debug ("Test");
		exit (0);
		g_unsetenv ("SUMMER_DEBUG");
	}
	g_test_trap_assert_passed ();
	g_test_trap_assert_stdout ("*Test*");
	g_test_trap_assert_stdout ("*Summer*");
}

int main (int argc, char *argv[])
{
	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_add_func ("/debug/dont-print", dont_print);
	g_test_add_func ("/debug/print", print);
	return g_test_run ();
}
