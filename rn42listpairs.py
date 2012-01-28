#!/usr/bin/python
import sys
import serial
from serial.tools import list_ports


# consider using list_ports.grep() instead
def find_usb_port():
    ports = list_ports.comports()
    for port in ports:
        if port[0].count('usbserial') == 1:
            return port[0]
    return None

def main(*args):
    usb_port = find_usb_port()
    if not usb_port:
        print 'USB port not found, exiting'
        return 1

    print 'Found USB port: %s' % usb_port
    print 'Connecting to it with serial...'

    ser = serial.Serial(usb_port, 115200, timeout = 2)

    print 'Connected.'

    ser.write("$$$")
    print ser.readline()
    ser.write("D")
    print ser.readline()
    ser.close()

    return 0 # exit errorlessly

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
