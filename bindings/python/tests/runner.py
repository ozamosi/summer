import summer
import unittest
try:
    import glib
except ImportError: #mainloop was recently moved
    import gobject as glib

class TestDataTypes (unittest.TestCase):
    def testitem(self):
        i = summer.ItemData ()
        i['title'] = 'hello world'
        self.assertEqual (i['title'], 'hello world') #first
        # Make sure strings aren't freed
        self.assertEqual (i['title'], 'hello world') #second
    def testonerror_noparam(self):
        loop = glib.MainLoop ()
        i = summer.ItemData ()
        i.append_downloadable ('http://blubb')
        dl = summer.create_download (i)
        def error_cb (inner_dl, error):
            self.assertEqual (inner_dl, dl)
            self.assertEqual (error.message, "Download Failed")
            self.assertEqual (inner_dl.get_property ('url'), 'http://blubb')
            loop.quit ()
        dl.on_error (error_cb)
        dl.start ()
        loop.run ()
    def testonerror_param(self):
        loop = glib.MainLoop ()
        i = summer.ItemData ()
        i.append_downloadable ('http://blubb')
        dl = summer.create_download (i)
        def error_cb (inner_dl, error, item):
            self.assertEqual (inner_dl, dl)
            self.assertEqual (error.message, "Download Failed")
            self.assertEqual (inner_dl.get_property ('url'), 'http://blubb')
            loop.quit ()
        dl.on_error (error_cb, i)
        dl.start ()
        loop.run ()

if __name__ == '__main__':
    unittest.main ()
