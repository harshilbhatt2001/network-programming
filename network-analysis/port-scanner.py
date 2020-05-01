'''
Created on Fri May 01 08:31:22 2020
@author: Harshil Bhatt
'''

import nmap

# Initialise port scanner
nmScan = nmap.PortScanner()

# scan localhost on ports 21 to 443
nmScan.scan('127.0.0.1', '21-443')

for host in nmScan.all_hosts():
    print("Host: %s %s" % (host, (nmScan[host].hostname())))
    print("State: %s" % nmScan[host].state())
    for proto in nmScan[host].all_protocols():
        print("--------")
        print('Protocol : %s' % proto)
 
        lport = nmScan[host][proto].keys()
        sorted(lport)
        for port in lport:
           print ('port : %s\tstate : %s' % (port, nmScan[host][proto][port]['state']))