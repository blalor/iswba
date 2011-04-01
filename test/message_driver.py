#!/usr/bin/env python
# encoding: utf-8
"""
message_driver.py

Created by Brian Lalor on 2011-03-27.
Copyright (c) 2011 __MyCompanyName__. All rights reserved.
"""

import sys
import os
import time

from serial import Serial

MESSAGES = {
    'rt_press'           : (0x50, 0x04, 0x68, 0x3b, 0x02, 0x05),
    'rt_press_long'      : (0x50, 0x04, 0x68, 0x3b, 0x12, 0x15),
    'rt_release'         : (0x50, 0x04, 0x68, 0x3b, 0x22, 0x25),
    'sendend_press'      : (0x50, 0x04, 0x68, 0x3b, 0x80, 0x87),
    'sendend_press_long' : (0x50, 0x04, 0x68, 0x3b, 0x90, 0x97),
    'sendend_release'    : (0x50, 0x04, 0x68, 0x3b, 0xa0, 0xa7),
    'pattern'            : (0x55, 0xaa),
    '55'                 : (0x55,),
    'aa'                 : (0xaa,),
    'zero'               : (0x00,),
    'one'                : (0x01,),
    '2zero'              : (0x00, 0x00),
}


def main():
    port = sys.argv[1]
    msg_keys = sys.argv[2:]
    
    with Serial(port = port, baudrate = 9600, parity='E', timeout=1) as serial:
        serial.flushInput()
        serial.flushOutput()
        
        while serial.read():
            pass
        
        for msg_key in msg_keys:
            print "> " + " ".join('%02X' % (c,) for c in MESSAGES[msg_key])
            serial.write("".join([chr(c) for c in MESSAGES[msg_key]]))
        
        # b = (serial.read(), serial.read())
        # while b[0]:
        #     print '< %02X %02X' % (ord(b[0]), ord(b[1]))
        #     b = (serial.read(), serial.read())
            
        
    


if __name__ == '__main__':
    main()

