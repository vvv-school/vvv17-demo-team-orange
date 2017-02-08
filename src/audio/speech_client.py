from __future__ import print_function
import os
os.environ["OMP_NUM_THREADS"] = "4"
import sys

import joblib
from librosa.feature import rmse

from prepare_data import get_spectrogram, compose_vector_of_functionals
from rnn_model import get_model

sys.path.append("/home/egor/robot-install/lib/python2.7/dist-packages")
import yarp
import numpy as np
import librosa
import time

yarp.Network.init()


p = yarp.BufferedPortSound()
p.open("/audio/in")

dbg = yarp.BufferedPortBottle()
dbg.open("/audio/out")

claps_port = yarp.BufferedPortBottle()
claps_port.open("/clap/out")

ICUB_PORT = "/icub/audio"
LOCAL_PORT = "/grabber/audio"
PORT = ICUB_PORT#LOCAL_PORT

if not yarp.NetworkBase_connect(PORT, p.getName()):
    print("cannot connect to audio port")
    sys.exit(0)

whole_rec = []
SAMPLE_RATE = 48000
RES_WAV_FILE = "/home/egor/test_numpy.wav"
USE_NN_DETECTOR = False

if USE_NN_DETECTOR:
    model = get_model( num_features= 216 )
    model.load_weights( "./models/rnn_model.h5" )
    train_mean, train_std = joblib.load( "./cache/train_mean_std.dat" )
    print("Loaded data")
else:
    model = joblib.load( "./cache/logistic.dat" )

def logistic_detector( signal, sample_rate, frames_to_classify = 10 ):
    start_time = time.time()
    features = get_spectrogram(signal, sample_rate, window=10, step=10, max_freq=8000)
    input = []
    for i in range(0, features.shape[0], frames_to_classify):
        subset = features[i:i + frames_to_classify]
        if len(subset) < frames_to_classify:
            continue
        input.append(np.expand_dims(compose_vector_of_functionals(subset), axis=0))
    input = np.vstack(input)
    #predictions = model.predict( input )
    prediction_proba = model.predict_proba( input )
    predictions = [1 if p[1] > 0.75 else 0 for p in prediction_proba ]
    total_time = time.time() - start_time
    #print("Prediction took:{}".format(total_time))
    #print(predictions)
    if sum(predictions) > 0:
        print("*** Detected clap by logreg ***")
        return True
    return False


def nn_detector( signal, sample_rate, frames_to_classify = 4 ):
    start_time = time.time()
    features = get_spectrogram( signal, sample_rate, window = 10, step = 10, max_freq = 8000 )
    input = []
    for i in range(0, features.shape[0], frames_to_classify):
        subset = features[i:i + frames_to_classify]
        if len(subset) < frames_to_classify:
            continue
        input.append( np.expand_dims(compose_vector_of_functionals(subset), axis = 0) )
    input = np.vstack( input )
    input = (input - train_mean) / (train_std + 1e-8)
    pred = model.predict( input )
    pred = pred.ravel()
    #print(pred)
    pred = [1 if p > 0.75 else 0 for p in pred]
    total_time = time.time() - start_time
    #print("Prediction took:{}".format( total_time ))
    #print(pred)

    if sum(pred) > 0:
        #print("*** Detected clap ***")
        return True
    return False

def rms_detector( signal, sample_rate, threshold = 6.0 ):
    averaged_signal = np.mean(signal, axis=0)
    S, phase = librosa.magphase(librosa.stft(averaged_signal))

    rms = librosa.feature.rmse(S=S)
    rms = rms.ravel()
    max_rms = rms.max()
    #print("Max RMS:{}".format(max_rms))

    # for rm in rms:
    #     rms_value = float(rm)
    #     bottle = dbg.prepare()
    #     bottle.clear()
    #     bottle.addDouble(rms_value)
    #     dbg.write()
    if max_rms > threshold:
        #print("****@@@@ Clap detected @@@****")
        return True
    return False

try:
    while True:

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

        #simple rmse click
        if USE_NN_DETECTOR:
            clap = nn_detector( res, SAMPLE_RATE )
        else:
            clap_logistic = logistic_detector( res, SAMPLE_RATE, frames_to_classify = 10 )
            clap_by_rms = rms_detector( res, SAMPLE_RATE, threshold = 10.0 )
            clap = clap_by_rms & clap_logistic
            #clap = clap_logistic

        if clap:
            print(">>>>&&&& There was a clap")
            clap_bottle = claps_port.prepare()
            clap_bottle.clear()
            clap_bottle.addInt(1)
            claps_port.write()


        if len(whole_rec) > 0:
            whole_rec = np.hstack( (whole_rec, res) )
        else:
            whole_rec = res
        print("1 sec is gone:{}".format(whole_rec.shape[1]))
        #truncate
        #if whole_rec.shape[1] > SAMPLE_RATE * 2:
        #    whole_rec =

except KeyboardInterrupt:
    librosa.output.write_wav(RES_WAV_FILE, whole_rec, SAMPLE_RATE)
    print("Written file")
except Exception,e:
    print(e)
finally:
    print("grace close")
    p.close()
    yarp.Network.fini()
    dbg.close()
    claps_port.close()
    sys.exit()


