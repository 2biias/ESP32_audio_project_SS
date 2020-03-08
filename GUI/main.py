

import time
import sys

import spotify
import http
import gui



#Create http webserver
myserver = http.http()

#Create spotify class
spotify = spotify.spotify(myserver)

#create the gui
mygui = gui.GUI(spotify)

mygui.window.mainloop()

















# print("Type play for play \nType pause for puase \nType skip for skip \nType info for info\nType volume to set volume")
# while True:
#    Song = raw_input()
#    print(Song)
#
#    if Song == "play":
#        spotify.playsong()
#    elif Song == "pause":
#        spotify.pausesong()
#    elif Song == "skip":
#        spotify.skipsong()
#    elif Song == "volume":
#        volume = (int)(raw_input("Set volume 0-100\n"))
#        if volume >= 0 and volume <= 100:
#            spotify.setvolume(volume)
#    elif Song == "info":
#        artist, song, albumname = spotify.infocurrent()
#        print("Artist: " + artist)
#        print("Song name: " + song)
#        if song == albumname:
#            print("This is a single")
#        else:
#            print("Album name: " + albumname)
#    else:
#        print("Invalid input")
