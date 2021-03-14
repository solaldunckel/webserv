import socket
import http.client

HOST = '127.0.0.1'
PORT = 80

def send_request(method, uri) :
  print("Test " + method + " http://" + HOST + ":" + str(PORT) + uri)
  conn = http.client.HTTPConnection(HOST, PORT)
  conn.request(method, uri)
  response = conn.getresponse()
  return response

def get_tests() :
  response = send_request("GET", "/")
  if response.status != 200:
    print("Expected 200, received " + response.status)
  response = send_request("GET", "/wtf")
  if response.status != 404:
    print("Expected 404, received " + response.status)


send_request("GET", "/")
send_request("GET", "/wtf")

