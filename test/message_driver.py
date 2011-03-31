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
    '2zero'              : (0x00, 0x00),
    'hw'                 : (0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64),
}


def main():
    port = sys.argv[1]
    msg_keys = sys.argv[2:]
    
    with Serial(port = port, baudrate = 9600, parity = 'E', timeout=1) as serial:
        while serial.read():
            pass
        
        for msg_key in msg_keys:
            print ">" + " ".join('%02X' % (c,) for c in MESSAGES[msg_key])
            serial.write("".join([chr(c) for c in MESSAGES[msg_key]]))
            # # time.sleep(0.1)
            # for c in MESSAGES[msg_key]:
            #     print '> %02X' % (c,)
            #     serial.write(chr(c))
            #     serial.flush()
            #     
            #     serial.setParity('N')
            #     print '< %02X' % (ord(serial.read()),)
            
            serial.setParity('N')
            
            b = serial.read()
            while b:
                print '< %02X' % (ord(b),)
                b = serial.read()
            
            serial.setParity('E')
            
            
        
    


if __name__ == '__main__':
    main()

