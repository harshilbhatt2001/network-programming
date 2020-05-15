'''
Created on Fri May 01 23:13:22 2020
@author: Harshil Bhatt
'''


import socket
from threading import Thread
from socketserver import ThreadingMixIn


TCP_IP = 'localhost'
TCP_PORT = 9001
BUFFER_SIZE = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

filename = str(input("File to be saved as: "))

with open(filename, 'wb') as f:
    print("File opened")
    while True:
        data = s.recv(BUFFER_SIZE)
        print("data = %s" % data)
        if not data:
            f.close()
            print("File closed")
            break
        f.write(data)

print("Successfully recieved the file")
s.close()
print("Connection Closed")
        
