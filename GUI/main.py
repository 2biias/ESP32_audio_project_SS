

import time
import sys

sys.dont_write_bytecode = True


from Smartsoundsystem import spotify
from Smartsoundsystem import http
from Smartsoundsystem import gui




#Create http webserver
myserver = http.http()

#Create spotify class
spotify = spotify.spotify(myserver)

#create the gui
mygui = gui.GUI(spotify)


mygui.window.mainloop()




