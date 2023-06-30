#!/usr/bin/python3
"""
This script should send the simulated commands to display board's command broker.

Author:  Ivan Jauregui - ivan.jauregui@chamberlain.com
Author:  Rene Delgado  - rene.delgado@chamberlain.com

Python: 3.10.6
"""

import time
import threading
import serial
from inputdata import display_boot_display_rx as inputdata

port = 'COM4' # Define your Windows (COMx) or Linux uart port

print("|------------------------------------------------------------------------------|")
print("|                                                                              |")
print("|                                                                              |")
print("|                        Command Broker Client v.0.2                           |")
print("|                                                                              |")
print("|                                                                              |")
print("|------------------------------------------------------------------------------|")
print("")
input("Press Enter to continue...")
print("")

try:
    ser = serial.Serial(port)
except:
    #logging.error('Error at %s', 'division', exc_info=e)   
    input(port + " is unavailable...")

ser.baudrate = 9600
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
#ser.timeout = None          #block read
ser.timeout = 1            #non-block read
#ser.timeout = 2              #timeout block read
ser.xonxoff = False     #disable software flow control
ser.rtscts = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 2     #timeout for write


# Tx Method
def tx_method():
    packet = bytearray()
    # Loop forever
    while True:
        for command in inputdata:
            for byte in command:                
                # At this point there's a complete command
                packet.append(byte)
            bytes_written = ser.write(packet)
            print(command)
            packet.clear()
            time.sleep(.5)

            

    
# Rx Method
def rx_method():
    ETX = b'\x03'
    STX = b'\x02'
    while True:
        rxbyte = ser.read(1)
        if(rxbyte == STX):
            print("\r\n")
            print("<STX>", end='')
        elif(rxbyte == ETX):
            print("<ETX>")
        else:
            print(rxbyte.decode(), end='')


#Created the Threads
t1 = threading.Thread(target=tx_method)
t2 = threading.Thread(target=rx_method)

#Started the threads
t1.start()
t2.start()

#Joined the threads
t1.join()
t2.join()

print("Command Broker Client Ends...")
