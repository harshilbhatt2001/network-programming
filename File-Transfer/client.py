'''
Created on Fri May 01 23:13:22 2020
@author: Harshil Bhatt
'''


import socket

host = socket.gethostname()
port = 60000
s = socket.socket()

s.connect((host, port))
s.send(b'Hello Server!')

with open('received_file', 'wb') as f:
    print("File opened")
    while True:
        print("Receiving data...")
        data = s.recv(20480)
        print("data = %s" % data)
        if not data:
            break
        f.write(data)

f.close()
print("Successfully got the file")
s.close()
print("Connection closed")
