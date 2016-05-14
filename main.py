#!/usr/bin/env python2

import os
import sys
import time
import struct
import serial
import alsaaudio

import numpy as np

from gnuradio import gr
from trx import trx

cutoff = [
    {0:[-613, -587], 1:[-625, -575], 2:[-650, -550], 3:[-725, -475], 4:[-800, -400], 5:[-850, -350], 6:[-900, -300], 7:[-975, -225], 8:[-1000, -200], 9:[-1100, -100]},
    {0:[587, 613], 1:[575, 625], 2:[550, 650], 3:[475, 725], 4:[400, 800], 5:[350, 850], 6:[300, 900], 7:[225, 975], 8:[200, 1000], 9:[100, 1100]},
    {0:[-1150, -150], 1:[-1950, -150], 2:[-2250, -150], 3:[-2550, -150], 4:[-2850, -150], 5:[-3050, -150], 6:[-3450, -150], 7:[-3950, -150], 8:[-4550, -150], 9:[-5150, -150]},
    {0:[150, 1150], 1:[150, 1950], 2:[150, 2250], 3:[150, 2550], 4:[150, 2850], 5:[150, 3050], 6:[150, 3450], 7:[150, 3950], 8:[150, 4550], 9:[150, 5150]},
    {0:[-2500, 2500], 1:[-3000, 3000], 2:[-3500, 3500], 3:[-4000, 4000], 4:[-4500, 4500], 5:[-5000, 5000], 6:[-6000, 6000], 7:[-8000, 8000], 8:[-9000, 9000], 9:[-10000, 10000]}
]

playback = alsaaudio.Mixer('Speaker')
playback.setvolume(50)

recording = alsaaudio.Mixer('Mic')
recording.setvolume(50)

tb = trx()
tb.start()
ser = serial.Serial('/dev/ttyPS1', 115200)
while 1:
    time.sleep(0.05)
    # update RX meter
    data = int(np.minimum(1400.0, -100.0 * np.log10(tb.rx_meter.level())))
    ser.write(struct.pack('<BIB', 1, data, 0))
    while ser.inWaiting() >= 6:
        buffer = ser.read(6)
        code, data, crc8 = struct.unpack('<BIB', buffer[0:6])
        if code == 1:
            tb.set_rx_freq(data)
        elif code == 2:
            tb.set_tx_freq(data)
        elif code == 3:
            playback.setvolume(data * 10)
        elif code == 4:
            recording.setvolume(data * 10)
        elif code == 5:
            tb.set_cwl_cutoff(cutoff[0][data])
            tb.set_cwu_cutoff(cutoff[1][data])
            tb.set_lsb_cutoff(cutoff[2][data])
            tb.set_usb_cutoff(cutoff[3][data])
            tb.set_am_cutoff(cutoff[4][data])
        elif code == 6:
            mode = data
            if mode == 0:
                tb.set_tx_corr(-600)
            elif mode == 1:
                tb.set_tx_corr(600)
            else:
                tb.set_tx_corr(0)
            tb.stop()
            tb.wait()
            tb.set_rx_mode(mode)
            tb.set_tx_mode(mode)
            tb.start()
        elif code == 7:
            tb.set_ptt(data)
