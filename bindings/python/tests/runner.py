import summer
import unittest

class TestDataTypes (unittest.TestCase):
    def testitem(self):
        i = summer.ItemData ()
        i['title'] = 'hello world'
        self.assertEqual (i['title'], 'hello world') #first
        # Make sure strings aren't freed
        self.assertEqual (i['title'], 'hello world') #second

if __name__ == '__main__':
    unittest.main ()
