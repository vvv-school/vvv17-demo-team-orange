from __future__ import print_function
import os
import sys
sys.path.append("/home/egor/robot-install/lib/python2.7/dist-packages")
import yarp
import numpy as np
import librosa

yarp.Network.init()


p = yarp.BufferedPortSound()
p.open("/audio/in")


if not yarp.NetworkBase_connect("/grabber/audio", p.getName()):
    print("cannot connect to audio port")
    sys.exit(0)

whole_rec = []
SAMPLE_RATE = 48000
RES_WAV_FILE = "/home/egor/test_numpy.wav"

while True:
    try:
        snd = p.read()
        if not snd:
            print("cannot read the port")
        num_samples = snd.getSamples()
        num_channels = snd.getChannels()
        res = np.zeros(shape=(num_channels, num_samples), dtype=float)

        for ch_idx in range(num_channels):
            for sample_id in range( num_samples ):
                sample_value = snd.get( sample_id, 0 )
                sample_value = np.short( sample_value )
                dbl_value = sample_value / 65535.0
                res[ ch_idx, sample_id] = dbl_value
        if len(whole_rec) > 0:
            whole_rec = np.hstack( (whole_rec, res) )
        else:
            whole_rec = res
        print("1 sec is gone:{}".format(whole_rec.shape[1]))

    except KeyboardInterrupt:
        librosa.output.write_wav(RES_WAV_FILE, whole_rec, SAMPLE_RATE)
        print("Written file")
        p.close()
        yarp.Network.fini()
        sys.exit()


