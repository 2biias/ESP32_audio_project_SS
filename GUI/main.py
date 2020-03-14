

import sys

sys.dont_write_bytecode = True


from Smartsoundsystem import spotify
from Smartsoundsystem import http
from Smartsoundsystem import gui


def main():
    #Create http webserver
    myserver = http.http()
    
    #Create spotify class
    myspotify = spotify.spotify(myserver)
    
    #create the gui
    mygui = gui.GUI(myspotify)
    
    
    mygui.window.mainloop()
    
    

if __name__ == '__main__':
    main()




