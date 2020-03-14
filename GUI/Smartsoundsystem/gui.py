from tkinter import *
from tkinter import ttk
import time

from PIL import ImageTk, Image
import os
import requests
from io import BytesIO


class GUI():
    """Main class for gui, will make the tkinter root and keep charge of which page to show"""
    def __init__ (self,spotify):
        self.spotify = spotify
        self.window = Tk()
        self.window.title("Smart Sound System")
        self.window.geometry('800x480')
        #self.window.attributes("-fullscreen", True)
        self.pages = []
        #Create all pages
        self.append_pages()
        self.change_page("Authorize")

    def append_pages(self):
        self.pages.append(spotify_page(self,self.spotify))
        self.pages.append(equalizer_page(self,self.spotify))
        self.pages.append(front_page(self,self.spotify))
        self.pages.append(search_page(self,self.spotify, self.pages[0]))
        self.pages.append(authorize_page(self,self.spotify))


    def change_page(self,page):
        """Hides all current showing pages and shows the target page"""
        for index in self.pages:
            if index.show == True:
                index.hide_page()
            if index.name == page:
                index.show_page()


class page_template():
    """Class responsible for the basic gui element each page will contain, every page inheritance from this page"""
    def __init__(self, GUI, spotify):
        #Colours and button sizes
        self.colour = "dim gray"
        self.colour_text = "black"
        self.colour_button = "white"
        self.slider = "gray24"

        self.spotify = spotify
        self.show = False
        self.gui = GUI
        self.myframe = Frame(self.gui.window, background = self.colour)

    def show_page(self):
        self.show = True
        self.myframe.pack(side="top", fill="both", expand=True)

    def hide_page(self):
        self.hide = False
        self.myframe.pack_forget()

    def mybutton(self, text,x ,y ,command, command_arg = None,height=2, width=10):
        """Custon method for making a button using tkinter"""
        if command_arg == None:
            button = Button(self.myframe, text=text,bg=self.colour_text, fg=self.colour_button, height = height, width = width, highlightthickness = 0,command=lambda: command())
        else:
            button = Button(self.myframe, text=text,bg=self.colour_text, fg=self.colour_button, height = height, width = width, highlightthickness = 0,command=lambda: command(command_arg))

        button.place(x=x,y=y)
        return button

    def mySlider(self,x,y, command ,length = 300, width = 40):
        """Custom method for making a slider"""
        slider = Scale(self.myframe, from_=100,to=0, length = length, width = width, troughcolor = self.slider , background = self.colour_text ,highlightthickness = 0,showvalue = 0)
        slider.set(50)
        slider.bind("<ButtonRelease-1>", command)
        slider.place(x=x,y=y)
        return slider


class authorize_page(page_template):
    """This is the first page the user meets, forcing the user to login before any other actions take place. This page inheritance from page_Template"""
    def __init__(self,GUI,spotify):
        page_template.__init__(self,GUI,spotify)
        self.name = "Authorize"

        self.titel = Label(self.myframe, text="Smart Sound System", font=("Arial Black",35), background = self.colour)
        self.titel.place(x=160, y=45)

        self.button_authorize = self.mybutton("Authorize", 60, 200, self.authorize_pressed, height = 5, width = 80)

    def authorize_pressed(self):
        self.spotify.authorize()
        self.gui.change_page("Frontpage")


class front_page(page_template):
    """This is the page the user is redirected to after login in, this page inherites form page_template"""
    def __init__(self,GUI,spotify):
        page_template.__init__(self,GUI,spotify)
        self.name = "Frontpage"

        self.titel = Label(self.myframe, text="Smart Sound System", font=("Arial Black",35), background = self.colour)
        self.titel.place(x=160, y=45)

        self.button_spotify = self.mybutton("Spotify",190,150, self.gui.change_page, "spotify", height = 3, width = 50)
        self.button_equalizer = self.mybutton("Equalizer", 190, 250, self.gui.change_page, "equalizer", height = 3, width = 50)
        self.button_changeUser = self.mybutton("Change user", 190, 350, self.spotify.authorize, height = 3, width = 50)



class sub_page(page_template):
    """Class responsible for at graphinc interface on subpages, this class inheres from page_template"""
    def __init__(self, GUI, spotify):
        page_template.__init__(self,GUI,spotify)

        self.titel = Label(self.myframe, text="Smart Sound System", font=("Arial Black",35), background = self.colour)
        self.titel.place(x=100,y=5)

        self.button_return_to_home_page = self.mybutton("Return",690,5, self.gui.change_page, "Frontpage", height = 1)


class spotify_page(sub_page):
    """This page is responsible with all communication to the spotfy web API, it inherets from sub_page"""
    def __init__(self, GUI, spotify):
        sub_page.__init__(self,GUI,spotify)
        self.name = "spotify"
        self.playing = True

        self.button_change_page = self.mybutton("Equalizer",690,40, self.gui.change_page, "equalizer", height = 1)
        self.button_change_search = self.mybutton("Search",690,75, self.gui.change_page, "search", height = 1)

        self.button_skip_song = self.mybutton("Skip song",370,130, self.skip_song)
        self.button_pause_play = self.mybutton("Pause/Play",370,255, self.play_pause)
        self.button_prev = self.mybutton("Prev song",370,380, self.prev_song)

        self.volume_slider = self.mySlider(500,130,self.update_volume)
        self.progress_slider = self.mySlider(50,445,self.slider_time,length = 495, width = 15)
        self.progress_slider.config(orient = HORIZONTAL)
        self.progress_slider.config(from_ = 0)

        #Kunster
        self.song = Label(self.myframe, text="Placeholder" , font=("courier", 13), background = self.colour)
        self.song.place(x = 50, y = 70)

        #sang
        self.artist = Label(self.myframe, text="Placeholder" , font=("courier",13), background = self.colour)
        self.artist.place(x = 50, y = 100)

        #billede
        self.panel = Label(self.myframe, highlightthickness = 0)
        self.panel.place(x = 50, y = 130)

        #listbox with # QUESTION:
        self.listbox = Listbox(self.myframe, highlightthickness = 0, borderwidth = 0, width = 25, height = 18, background  = "gray24")
        self.listbox.place(x=570, y = 130)

        #update info every search
        self.myframe.after(1000, self.check_changes)

    def show_page(self):
        """Overwrites page_template show method, so that i can also call update_info()"""
        self.show = True
        self.myframe.pack(side="top", fill="both", expand=True)
        self.update_info()


    def update_volume(self, event):
        print(self.volume_slider.get())
        self.spotify.setvolume(self.volume_slider.get())

    def skip_song(self):
        self.spotify.skipsong()

    def prev_song(self):
        self.spotify.prev_song()

    def change_song(self):
        self.spotify.add_to_que()



    def update_info(self):
        """Update currently playing"""
        try:
            artist,song,albumname,progress_s,duration_s = self.spotify.infocurrent()
            self.progress_slider.config(to = duration_s)
            self.currentsong = song
            self.song.configure(text="Song: " + song)
            self.artist.configure(text="Artist: " + artist)

            #update image
            img_url = self.spotify.get_image()
            response = requests.get(img_url)
            img_data = response.content

            self.img = ImageTk.PhotoImage(Image.open(BytesIO(img_data)))
            self.panel.configure(image=self.img)
        except:
            pass


    def check_changes(self):
        try:
            artist,song,albumname,progress_s,duration_s = self.spotify.infocurrent()
            self.progress_slider.set(progress_s)
            if song == self.currentsong:
                pass
            else:
                self.update_info()
                #if song changes delete newest item in que
                self.listbox.delete(0,0)
        except:
            pass

        finally:
            #check again after 1sec.
            self.myframe.after(1000, self.check_changes)

    def play_pause(self):
        if self.playing == False:
            self.playing = True
            self.spotify.playsong()

        elif self.playing == True:
            self.playing = False
            self.spotify.pausesong()

    def slider_time(self,event):
        pass


class equalizer_page(sub_page):
    """This page is responsibly for all communation with the ESP32, sending equalizer info. This inherets from sub_page"""
    def __init__(self, GUI, spotify):
        sub_page.__init__(self,GUI,spotify)
        self.name = "equalizer"

        self.button_change_page = self.mybutton("Spotify",690,40, self.gui.change_page, "spotify", height = 1)


        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.place(x=625,y=150)

        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.place(x=375,y=150)

        self.volume_slider = Scale(self.myframe, from_=100,to=0, length = 300, width = 40, troughcolor = "gray24" , background = "black",highlightthickness = 0,showvalue = 0)
        self.volume_slider.set(50)
        self.volume_slider.place(x=125,y=150)



class search_page(sub_page):
    """This page communicates with the spotify web API, such that the user can search for songs and add them to the que. It inherets from sub_page"""
    def __init__(self, GUI, spotify, spotifypage):
        sub_page.__init__(self,GUI,spotify)
        self.name = "search"

        self.spotifypage = spotifypage

        self.button_change_page = self.mybutton("Spotify",690,40, self.gui.change_page, "spotify", height = 1)

        self.search_box = Entry(self.myframe)
        self.search_box.place(x=450,y=100)

        self.button_search = self.mybutton("Search",450,140, self.search)

        self.button_add_to_que = self.mybutton("Add to que",450,200, self.add_to_que)


        #Kunster
        self.song = Label(self.myframe, text="Placeholder" , font=("courier", 13), background = self.colour)
        self.song.place(x = 50, y = 90)

        #sang
        self.artist = Label(self.myframe, text="Placeholder" , font=("courier",13), background = self.colour)
        self.artist.place(x = 50, y = 120)

        #billede
        self.panel = Label(self.myframe, highlightthickness = 0)
        self.panel.place(x = 50, y = 150)

    def search(self):
        songname, author, uri, imageurl = self.spotify.search_for_Song(self.search_box.get())

        self.currentsong = songname

        self.uri = uri

        self.song.configure(text="Song: " + songname)
        self.artist.configure(text="Artist: " + author)

        #update image
        response = requests.get(imageurl)
        img_data = response.content

        self.img = ImageTk.PhotoImage(Image.open(BytesIO(img_data)))
        self.panel.configure(image=self.img)

    def add_to_que(self):
        self.spotify.add_to_que(self.uri)
        #add song to listbox que on spotify page
        self.spotifypage.listbox.insert(END, self.currentsong)
