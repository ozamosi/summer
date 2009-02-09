#include <pygobject.h>
 
void summer_register_classes (PyObject *d); 
extern PyMethodDef summer_functions[];
 
DL_EXPORT(void)
initsummer(void)
{
    PyObject *m, *d;
 
    g_type_init ();
	g_thread_init (NULL);

	init_pygobject ();
 
    m = Py_InitModule ("summer", summer_functions);
    d = PyModule_GetDict (m);
 
    summer_register_classes (d);
 
    if (PyErr_Occurred ()) {
        Py_FatalError ("can't initialise module summer");
    }
}
