import sys

sys.dont_write_bytecode = True

import os
import unittest
import requests
import threading



sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


from Smartsoundsystem import http



class GuiTestCase(unittest.TestCase):
    
    @classmethod
    def setUpClass(cls):
        print("Testing http class")
        cls.myserver = http.http(5000)
        
    @classmethod
    def tearDownClass(Cls):
        print("")
   
        
    def test_inc(self):
        inc = threading.Thread(target=self.threadinc)
        get = threading.Thread(target=self.threadget)
        inc.start()
        get.start()
        inc.join()
        get.join()
        self.assertEqual(200, self.response.status_code)
        self.assertEqual("123214xdcd24314",http.myHandler.code)
        
        
    def threadinc(self):
        self.myserver.getinc()
        
    def threadget(self):
        self.response = requests.get("http://127.0.0.1:5000", params={"code": "123214xdcd24314"})


if __name__ == '__main__':
    unittest.main()