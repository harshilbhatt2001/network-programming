'''
Created on Fri May 01 23:13:22 2020
@author: Harshil Bhatt
'''


import socket

port = 60000
s = socket.socket()
host = socket.gethostname()
s.bind((host, port))
s.listen()

print("Server Listening...")

while True:
    conn, addr = s.accept()
    print("Got connection from ", addr)
    data = conn.recv(20480)

    filename = 'intro.txt'
    f = open(filename, 'rb')
    l = f.read(20480)
    while l:
        conn.send(l)
        print("Sent ",repr(l))
        l = f.read(20480)
    f.close()

    print("Finished Sending")
    conn.send(b'Thank you for connecting')
    conn.close()
