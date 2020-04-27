
import sys

sys.dont_write_bytecode = True

import os

from mock import Mock, patch

import unittest
import json



sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


from Smartsoundsystem import spotify
from Smartsoundsystem import http
from Smartsoundsystem import gui



class SpotifyTestCase(unittest.TestCase):
    
    @classmethod
    def setUpClass(cls):
        print("Testing Spotify Class")
        cls.myserver = http.http(4000)
        
        cls.myspotify = spotify.spotify(cls.myserver)
        cls.myspotify.Headers = "NaN"
           
    
    @classmethod
    def tearDownClass(Cls):
        print("")
        
    @patch('Smartsoundsystem.spotify.requests.post')
    def test_skipsong(self,mock_post):
        mock_post.return_value.ok = True
        
        response = self.myspotify.skipsong()
        
      
        self.assertEqual(True,response.ok)
        
        mock_post.return_value.ok = False
        
        self.assertRaises(RuntimeError ,self.myspotify.skipsong)
        
        self.assertEqual(2,mock_post.call_count) 
        
    @patch('Smartsoundsystem.spotify.requests.get')
    def test_infocurrent(self,mock_get):
        with open('Tests/test_spotify/current_response.json') as json_file:
            myfile = json.load(json_file)
            mock_get.return_value.json.return_value = myfile
        
        response = self.myspotify.infocurrent()
        
        
        self.assertEqual('Shaka Loveless', response[0])
        self.assertEqual('Shaka Loveless', response[2])
        self.assertEqual(2, response[3])
        self.assertEqual(195, response[4])
        
    @patch('Smartsoundsystem.spotify.requests.get')
    def test_isuserplaying(self,mock_get):
        mock_get.return_value.status_code = 200
        
        self.assertEqual(self.myspotify.isUserPlaying(),True)
        
        mock_get.return_value.status_code = 401

        self.assertEqual(self.myspotify.isUserPlaying(),False)

        
        
        
if __name__ == '__main__':
    unittest.main()