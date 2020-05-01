'''
Created on Fri May 01 08:31:22 2020
@author: Harshil Bhatt
'''

import nmap
import optparse

# defining nmap scan function with arguments
# tgtHost will hold the host value and tgtPort will hold the port value
def nmapScan(tgtHost, tgtPort):
    nmScan = nmap.PortScanner()
    nmScan.scan(tgtHost, tgtPort)
    state = nmScan[tgtHost]['tcp'][int(tgtPort)]['state']
    print("[*] " + tgtHost + "tcp/" + tgtPort + " " + state)

def main():
    parser = optparse.OptionParser('Script Usage:'+'-H <target host> -p <target port>')
    
    parser.add_option('-H', dest='tgtHost', type='string', 
    help='specify target host')

    parser.add_option('-p', dest='tgtPort', type='string', 
    help='specify target port[s] separated by comma')

    (options,args) = parser.parse_args()
    tgtHost = options.tgtHost
    tgtPorts = str(options.tgtPort)
    
    print(tgtPorts)
    
    if (tgtHost == None) | (tgtPorts[0] == None):
        print(parser.usage)
        exit(0)
        
    ports = tgtPorts.strip("'").split(',')
    
    for tgtPort in ports:
        print(tgtHost+ " " +tgtPort)
        nmapScan(tgtHost, tgtPort)

if __name__ == '__main__':
        main()