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
        self.Authorize_payload = {'client_id': self.myclient_id, 'response_type': 'code', 'redirect_uri': self.myredirect_uri, 'scope': self.myscope, 'mystate': self.mystate}
        self.endpoints()

    def endpoints(self):
        self.Authroize_endpoint = 'https://accounts.spotify.com/authorize'
        self.Token_endpoint = "https://accounts.spotify.com/api/token"
        self.Current_endpoint = "https://api.spotify.com/v1/me/player"
        self.Skip_endpoint = "https://api.spotify.com/v1/me/player/next"
        self.Pause_endpoint = "https://api.spotify.com/v1/me/player/pause"
        self.Play_endpoint = "https://api.spotify.com/v1/me/player/play"
        self.Volume_endpoint = "https://api.spotify.com/v1/me/player/volume"
        self.prev_endpoint = "https://api.spotify.com/v1/me/player/previous"
        self.add_to_que_endpoint = "https://api.spotify.com/v1/me/player/queue"
        self.search_endpoint = "https://api.spotify.com/v1/search"
        self.seek_endpoint = "https://api.spotify.com/v1/me/player/seek"


    def authorize(self):
        response = requests.get(self.Authroize_endpoint, params=self.Authorize_payload, verify=True)
        webpage = subprocess.Popen(["google-chrome", response.url])
        self.http.getinc()
        webpage.kill()
        Token_Body = {'grant_type': 'authorization_code', 'code': http.myHandler.code, 'redirect_uri': self.myredirect_uri,  'client_id': self.myclient_id, 'client_secret': self.myclient_secret}
        response_token = requests.post(url = self.Token_endpoint, data = Token_Body)
        json_response = response_token.json()
        self.access_token = str(json_response['access_token'])
        self.expires = str(json_response['expires_in'])
        self.refresh = str(json_response['refresh_token'])
        self.type = str(json_response['token_type'])
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
        requests.put(url = self.Volume_endpoint, params = myparam, headers = self.Headers)

    def prev_song(self):
        requests.post(url = self.prev_endpoint, headers = self.Headers)

    def add_to_que(self, songUri):
        myparam = { "uri": songUri}
        requests.post(url = self.add_to_que_endpoint,params = myparam, headers = self.Headers)

    def search_for_Song(self, song):
        print(song)
        myparam = { "q": song, "type": "track", "limit": "1"}
        response = requests.get(url = self.search_endpoint,params = myparam, headers = self.Headers)
        jresponse = response.json()

        tracks = jresponse['tracks']
        items = tracks['items']
        data = items[0]

        #find songname
        songname = data['name']
        print(songname)

        #find uri
        uri = data['uri']
        print(uri)

        #find author
        album = data['album']
        artists = album['artists']
        artistdata = artists[0]
        author = artistdata['name']
        print(author)

        #find image
        images = album['images']
        #medium
        medium = images[1]
        imageurl = medium['url']
        print(imageurl)

        return songname, author, uri, imageurl


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
        duration_s = (item['duration_ms'])/1000
        progress_s = (songdata['progress_ms'])/1000
        return [artist, song, albumname, progress_s, duration_s]

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

    def update_time_stamp(self,time):
        myparam = {'position_ms':time*1000}
        requests.put(url = self.seek_endpoint, params = myparam, headers = self.Headers)
