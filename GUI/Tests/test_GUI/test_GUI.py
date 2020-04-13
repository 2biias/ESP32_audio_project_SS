

import sys

sys.dont_write_bytecode = True

import os
import unittest



sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


from Smartsoundsystem import spotify
from Smartsoundsystem import http
from Smartsoundsystem import gui



class GuiTestCase(unittest.TestCase):
    
    @classmethod
    def setUpClass(cls):
        print("Testing Gui Class")
        cls.myserver = http.http(7000)
        cls.myspotify = spotify.spotify(cls.myserver)
        cls.mygui = gui.GUI(cls.myspotify)
    
        cls.GUI= cls.mygui
        
    @classmethod
    def tearDownClass(Cls):
        print("")        
        
    def test_changepage(self):
        self.check_pages("equalizer")
        self.check_pages("search")
        self.check_pages("Frontpage")
        
    def test_changeToUknown(self):
        self.assertRaises(NameError,self.GUI.change_page,"DenneSideFindesIkke")
        
        
    def check_pages(self, nametest):
        self.GUI.change_page(nametest)
        for index in self.GUI.pages:
            if index.name == nametest:
                self.assertEqual(True, index.show)
            else:
                self.assertEqual(False, index.show)

if __name__ == '__main__':
    unittest.main()