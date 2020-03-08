import subprocess
import requests
import json
import webbrowser
import time
import socketserver

import http



class spotify():
    def __init__(self, http):
        self.auth = False
        self.http = http
        self.myclient_id = '9949d61e931a4e38be31a1f717f5e726'
        self.myclient_secret = '3a4540f32774457a9b40e5834df1d567'
        self.myredirect_uri = 'http://127.0.0.1:8000'
        self.myscope = 'user-modify-playback-state%20user-read-playback-state%20user-read-playback-state%20user-top-read'
        self.mystate = 'mysystem'
        self.Authroize_endpoint = 'https://accounts.spotify.com/authorize'
        self.Authorize_payload = {'client_id': self.myclient_id, 'response_type': 'code', 'redirect_uri': self.myredirect_uri, 'scope': self.myscope, 'mystate': self.mystate}
        self.Token_endpoint = "https://accounts.spotify.com/api/token"
        self.Current_endpoint = "https://api.spotify.com/v1/me/player"
        self.Skip_endpoint = "https://api.spotify.com/v1/me/player/next"
        self.Pause_endpoint = "https://api.spotify.com/v1/me/player/pause"
        self.Play_endpoint = "https://api.spotify.com/v1/me/player/play"
        self.Volume_endpoint = "https://api.spotify.com/v1/me/player/volume"
        self.prev_endpoint = "https://api.spotify.com/v1/me/player/previous"

    def authorize(self):
        self.response = requests.get(self.Authroize_endpoint, params=self.Authorize_payload, verify=True)
        self.webpage = subprocess.Popen(["google-chrome", self.response.url])
        self.http.getinc()
        self.webpage.kill()
        self.Token_Body = {'grant_type': 'authorization_code', 'code': http.myHandler.code, 'redirect_uri': self.myredirect_uri,  'client_id': self.myclient_id, 'client_secret': self.myclient_secret}
        self.response_token = requests.post(url = self.Token_endpoint, data = self.Token_Body)
        self.json_response = self.response_token.json()
        self.access_token = str(self.json_response['access_token'])
        self.expires = str(self.json_response['expires_in'])
        self.refresh = str(self.json_response['refresh_token'])
        self.type = str(self.json_response['token_type'])
        self.access = self.type +' '+self.access_token
        self.Headers = {'Authorization': self.access}
        self.auth = True

    def skipsong(self):
        requests.post(url = self.Skip_endpoint, headers = self.Headers)

    def pausesong(self):
        requests.put(url = self.Pause_endpoint, headers = self.Headers)

    def playsong(self):
        requests.put(url = self.Play_endpoint, headers = self.Headers)

    def setvolume(self,volume):
        print("Volume in func: " + str(volume))
        myparam = {'volume_percent':volume}
        r = requests.put(url = self.Volume_endpoint, params = myparam, headers = self.Headers)

    def prev_song(self):
        requests.post(url = self.prev_endpoint, headers = self.Headers)


    def infocurrent(self):
        currently_playing = requests.get(self.Current_endpoint, headers = self.Headers)
        songdata = currently_playing.json()
        item = songdata['item']
        album = item['album']
        albumname = album['name']
        artists = album['artists']
        song = item['name']
        artist_string  = artists[0]
        artist = artist_string['name']
        return [artist, song, albumname]

    def get_image(self):
        currently_playing = requests.get(self.Current_endpoint, headers = self.Headers)
        songdata = currently_playing.json()
        item = songdata['item']
        album = item['album']
        images = album['images']
        smallimg = images[1]
        image = smallimg['url']
        print(image)
        return image
