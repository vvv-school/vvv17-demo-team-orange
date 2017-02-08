from __future__ import print_function
import os
import sys
sys.path.append("/home/egor/robot-install/lib/python2.7/dist-packages")

import yarp
import numpy as np
import librosa

yarp.Network.init()
p = yarp.BufferedPortBottle()
p.open("/bcmAudio/dbg:o")
while True:
    vector = p.read()
    if not vector:
        print("cannot read the port")
    print(vector)