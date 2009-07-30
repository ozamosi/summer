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
        summer.download_set_default (tmp_dir = ".", save_dir = ".")
        loop = glib.MainLoop ()
        i = summer.ItemData ()
        i.append_downloadable ('http://blubb')
        dl = summer.create_download (i)
        def error_cb (inner_dl, error):
            loop.quit ()
            self.assertEqual (inner_dl, dl)
            self.assertEqual (error.message, "Download failed: Cannot resolve hostname")
            self.assertEqual (inner_dl.get_property ('url'), 'http://blubb')
        dl.on_error (error_cb)
        dl.start ()
        loop.run ()
    def testonerror_param(self):
        summer.download_set_default (tmp_dir = ".", save_dir = ".")
        loop = glib.MainLoop ()
        i = summer.ItemData ()
        i.append_downloadable ('http://blubb')
        dl = summer.create_download (i)
        def error_cb (inner_dl, error, item):
            loop.quit ()
            self.assertEqual (inner_dl, dl)
            self.assertEqual (error.message, "Download failed: Cannot resolve hostname")
            self.assertEqual (inner_dl.get_property ('url'), 'http://blubb')
            self.assertEqual (item, i)
        dl.on_error (error_cb, i)
        dl.start ()
        loop.run ()

if __name__ == '__main__':
    unittest.main ()
