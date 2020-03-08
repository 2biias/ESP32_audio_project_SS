from tkinter import *
import time

from PIL import ImageTk, Image
import os
import requests
from io import BytesIO


class GUI():
    def __init__ (self,spotify):
        self.spotify = spotify
        self.window = Tk()
        self.window.title("Smart Sound System")
        self.window.geometry('800x480')
        #self.window.attributes("-fullscreen", True)
        self.pages = []
        self.pages.append(spotify_page(self,self.spotify))
        self.pages.append(equalizer_page(self,self.spotify))
        self.pages.append(front_page(self,self.spotify))
        self.change_page("Frontpage")

    def change_page(self,page):
        for index in self.pages:
            if index.show == True:
                index.hide_page()
            if index.name == page:
                index.show_page()


class page_template():
    def __init__(self, GUI, spotify):
        #Colours and button sizes
        self.colour = "dim gray"
        self.colour_text = "black"
        self.colour_button = "white"
        self.height = 2
        self.width = 10

        self.spotify = spotify
        self.show = False
        self.gui = GUI
        self.myframe = Frame(self.gui.window, background = self.colour)
        #overskrift

    def show_page(self):
        self.show = True
        self.myframe.pack(side="top", fill="both", expand=True)

    def hide_page(self):
        self.hide = False
        self.myframe.pack_forget()

    def update_info(self):
        #update curently playing
        artist,song,albumname = self.spotify.infocurrent()
        self.song.configure(text="Song: " + song)
        self.artist.configure(text="Artist: " + artist)

        #update image
        img_url = self.spotify.get_image()
        response = requests.get(img_url)
        img_data = response.content
        self.img = ImageTk.PhotoImage(Image.open(BytesIO(img_data)))
        self.panel.configure(image=self.img)

    def play_pause(self):
        if self.playing == False:
            self.playing = True
            self.spotify.playsong()

        elif self.playing == True:
            self.playing = False
            self.spotify.pausesong()


class sub_page(page_template):
    def __init__(self, GUI, spotify):
        page_template.__init__(self,GUI,spotify)

        self.titel = Label(self.myframe, text="Smart Sound System", font=("Arial Black",35), background = self.colour)
        self.titel.place(x=100,y=5)

        self.button_return_to_home_page = Button(self.myframe, text="Return",bg=self.colour_text, fg=self.colour_button, height = 1, width = self.width, highlightthickness = 0, command=lambda:self.gui.change_page("Frontpage"))
        self.button_return_to_home_page.place(x=690,y=5)


class front_page(page_template):
    def __init__(self,GUI,spotify):
        page_template.__init__(self,GUI,spotify)
        self.name = "Frontpage"

        self.titel = Label(self.myframe, text="Smart Sound System", font=("Arial Black",35), background = self.colour)
        self.titel.place(x=160, y=45)

        self.button_spotify = Button(self.myframe, text="Spotify",bg=self.colour_text, fg=self.colour_button, height = 3, width = 50, highlightthickness = 0,command=lambda: self.gui.change_page("spotify"))
        self.button_spotify.place(x=190,y=150)

        self.button_equalizer = Button(self.myframe, text="Equalizer",bg=self.colour_text, fg=self.colour_button, height = 3, width = 50, highlightthickness = 0,command=lambda: self.gui.change_page("equalizer"))
        self.button_equalizer.place(x=190,y=250)

        self.button_authorize = Button(self.myframe, text="Change User",bg=self.colour_text, fg=self.colour_button, height = 3, width = 50,highlightthickness = 0,command=lambda: self.spotify.authorize())
        self.button_authorize.place(x=190,y=350)


class spotify_page(sub_page):
    def __init__(self, GUI, spotify):
        sub_page.__init__(self,GUI,spotify)
        self.name = "spotify"
        self.playing = True

        self.button_change_page = Button(self.myframe, text="Equalizer",bg=self.colour_text, fg=self.colour_button,height = 1, width = self.width , highlightthickness = 0,command=lambda: self.gui.change_page("equalizer"))
        self.button_change_page.place(x=690, y=40)

        self.button_skip_song = Button(self.myframe, text="Skip Song", bg=self.colour_text, fg=self.colour_button, height = self.height, width = self.width,highlightthickness = 0, command=lambda:self.skip_song())
        self.button_skip_song.place(x=430,y=150)

        self.button_pause_play = Button(self.myframe, text="Pause/Play", bg=self.colour_text, fg=self.colour_button, height = self.height, width = self.width,highlightthickness = 0, command=lambda:self.play_pause())
        self.button_pause_play.place(x=430, y= 275)

        self.button_prev = Button(self.myframe, text="Prev song", bg=self.colour_text, fg=self.colour_button, height = self.height, width = self.width,highlightthickness = 0, command=lambda:self.prev_song())
        self.button_prev.place(x=430, y= 400)

        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.bind("<ButtonRelease-1>", self.update_volume)
        self.volume_slider.place(x=625,y=150)

        self.song = Label(self.myframe, text="Placeholder" , font=("courier", 13), background = self.colour)
        self.song.place(x = 50, y = 90)

        self.artist = Label(self.myframe, text="Placeholder" , font=("courier",13), background = self.colour)
        self.artist.place(x = 50, y = 120)

        self.panel = Label(self.myframe, highlightthickness = 0)
        self.panel.place(x = 50, y = 150)

    def update_volume(self, event):
        print(self.volume_slider.get())
        self.spotify.setvolume(self.volume_slider.get())

    def skip_song(self):
        self.spotify.skipsong()
        time.sleep(0.5)
        self.update_info()

    def prev_song(self):
        self.spotify.prev_song()
        time.sleep(0.5)
        self.update_info()

class equalizer_page(sub_page):
    def __init__(self, GUI, spotify):
        sub_page.__init__(self,GUI,spotify)
        self.name = "equalizer"
        self.button_change_page = Button(self.myframe, text="Spotify",bg=self.colour_text, fg=self.colour_button,height = 1, width = self.width , highlightthickness = 0,command=lambda: self.gui.change_page("spotify"))
        self.button_change_page.place(x=690, y=40)

        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.place(x=625,y=150)

        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.place(x=375,y=150)

        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.place(x=125,y=150)
