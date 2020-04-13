from tkinter import *


class queue():
    def __init__(self,spotify_page):
        self.spotifypage = spotify_page
        self.queue_artist = []
        self.queue_title = []
        self.songs_in_queue = 0

    def add_song_to_queue(self,artist,title):
        if self.songs_in_queue > 10:
            raise IndexError("Queue Full")
        if not type(artist) == str and not type(artist) == str:
            raise TypeError("Type not string")
          
            
        self.songs_in_queue += 1
        self.queue_artist.append(Label(self.spotifypage.myframe, text= artist , font=("courier", 8), background = "dim gray"))
        self.queue_title.append(Label(self.spotifypage.myframe, text=title , font=("courier", 8), background = "dim gray"))

        self.queue_artist[self.songs_in_queue-1].place(x=580, y= 155 + (self.songs_in_queue -1 )*25)
        self.queue_title[self.songs_in_queue-1].place(x=680, y= 155 + (self.songs_in_queue -1 )*25)

    def remove_song(self):
        if self.songs_in_queue == 0:
            raise IndexError("No songs in que")
        
        label_artist = self.queue_artist.pop(0)
        label_title = self.queue_title.pop(0)

        self.last_artist = label_artist.cget("text")
        self.last_title = label_title.cget("text")

        label_artist.destroy()
        label_title.destroy()

        for index, item, in enumerate(self.queue_artist):
            item.place(x = 580, y = 155 + index * 25 )

        for index, item in enumerate(self.queue_title):
            item.place(x = 680, y = 155 + index * 25 )

        self.songs_in_queue -= 1
        

        
