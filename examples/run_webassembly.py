#! /usr/bin/python3
from http.server import HTTPServer, SimpleHTTPRequestHandler
import sys
import webbrowser

PORT = 8888

class CustomHTTPRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("cross-origin-embedder-policy", "require-corp")
        self.send_header("cross-origin-opener-policy", "same-origin")
        SimpleHTTPRequestHandler.end_headers(self)

with HTTPServer(("", PORT), CustomHTTPRequestHandler) as httpd:
    try:
        webbrowser.open("http://localhost:%s" % PORT)
        print("serving at port", PORT)
        httpd.serve_forever()
    finally:
        sys.exit(0)
