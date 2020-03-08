

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




