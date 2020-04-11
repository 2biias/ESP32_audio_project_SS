from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
from urlparse import urlparse


class myHandler(BaseHTTPRequestHandler):
    code = None
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        query = urlparse(self.path).query
        query_components = dict(qc.split("=") for qc in query.split("&"))
        myHandler.code = query_components["code"]
        return


class http():
    def __init__(self, port):
        self.PORT = port
        self.server = HTTPServer(('', self.PORT), myHandler)
        print("serving at port", self.PORT)

    def getinc(self):
        self.server.handle_request()
