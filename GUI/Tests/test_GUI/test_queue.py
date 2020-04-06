

import sys

sys.dont_write_bytecode = True

import os
import unittest



sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


from Smartsoundsystem import que

from Smartsoundsystem import spotify
from Smartsoundsystem import http
from Smartsoundsystem import gui




class test_queue(unittest.TestCase):
    
    @classmethod
    def setUpClass(cls):
        cls.myserver = http.http()
        cls.myspotify = spotify.spotify(cls.myserver)
        cls.mygui = gui.GUI(cls.myspotify)
    
        cls.queue= cls.mygui.pages[0].que
        
        
    def test_addSongToQueue(self):
        self.assertEqual(0,self.queue.songs_in_queue)
               
        
        self.queue.add_song_to_queue(1, "Arne")
        
        self.assertEqual(1,self.queue.songs_in_queue)
        
        self.queue.remove_song()
        
        
        self.assertEqual(0,self.queue.songs_in_queue)
        
             
        
    def test_songAndAuthor(self):
        
        self.queue.add_song_to_queue("Billie", "Bad guy")
        
      
        self.assertEqual("Billie",self.queue.queue_artist[self.queue.songs_in_queue-1].cget("text"))
        
       
        self.assertEqual("Bad guy",self.queue.queue_title[self.queue.songs_in_queue-1].cget("text"))
        
        self.queue.remove_song()
        
        self.assertEqual(0,self.queue.songs_in_queue)
        
        self.assertEqual(0, len(self.queue.queue_artist))
        self.assertEqual(0, len(self.queue.queue_title))
        
        

        


if __name__ == '__main__':
    unittest.main()