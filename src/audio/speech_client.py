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

DEBUG = True
ICUB_PORT = "/icub/audio"
RPC_CLAP_NAME = "/master/rpc:i"
RPC_LOCAL_CLIENT_NAME = "/clapDuo/master/rpc:o"
rpc_port =  yarp.RpcClient()


if not rpc_port.open(RPC_CLAP_NAME) and not DEBUG:
    print("Could not connect to RPC client")
    sys.exit(-1)
yarp.NetworkBase_connect(rpc_port.getName(), RPC_LOCAL_CLIENT_NAME, "tcp")


LOCAL_PORT = "/grabber/audio"
PORT = ICUB_PORT#LOCAL_PORT

if not yarp.NetworkBase_connect(PORT, p.getName()):
    print("cannot connect to audio port")
    sys.exit(0)

whole_rec = []
SAMPLE_RATE = 48000
RES_WAV_FILE = "/home/egor/test_numpy.wav"
USE_NN_DETECTOR = True

if USE_NN_DETECTOR:
    print("Use nn model")
    model = get_model( num_features= 248 )
    model.load_weights( "./models/rnn_model.h5" )
    train_mean, train_std = joblib.load( "./cache/train_mean_std.dat" )
else:
    print("use logreg model")
    model = joblib.load( "./cache/logistic.dat" )

def logistic_detector( signal, sample_rate, frames_to_classify = 15 ):
    start_time = time.time()
    features = get_spectrogram(signal, sample_rate, window=15, step=15,min_freq = 700, max_freq=8000)
    input = []
    for i in range(0, features.shape[0], frames_to_classify):
        subset = features[i:i + frames_to_classify]
        if len(subset) < frames_to_classify:
            continue
        input.append(np.expand_dims(compose_vector_of_functionals(subset), axis=0))
    input = np.vstack(input)

    #input = (input - train_mean) / (train_std + 1e-8)

    #predictions = model.predict( input )
    prediction_proba = model.predict_proba( input )
    print(prediction_proba)
    predictions = [1 if p[1] > 0.75 else 0 for p in prediction_proba ]
    total_time = time.time() - start_time
    #print("Prediction took:{}".format(total_time))
    #print(predictions)
    if sum(predictions) > 0:
        print("*** Detected clap by logreg ***")
        return True
    return False


def nn_detector( signal, sample_rate, frames_to_classify = 15 ):
    start_time = time.time()
    features = get_spectrogram( signal, sample_rate, window = 15, step = 15,min_freq = 700, max_freq = 8000 )
    input = []
    i = 0
    while i < features.shape[0]:
        subset = features[i:i + frames_to_classify]
        if len(subset) < frames_to_classify:
            i += frames_to_classify
            continue
        input.append( np.expand_dims(compose_vector_of_functionals(subset), axis = 0) )
        i += frames_to_classify//2

    input = np.vstack( input )
    input = (input - train_mean) / (train_std + 1e-8)
    pred = model.predict( input )
    pred = pred.ravel()

    pred = [1 if p > 0.5 else 0 for p in pred]
    print(pred)
    total_time = time.time() - start_time
    print("Prediction took:{}".format( total_time ))
    #print(pred)

    if sum(pred) > 0:
        print("*** Detected clap by NN ***")
        return True
    return False

def rms_detector( signal, sample_rate, threshold = 6.0 ):
    averaged_signal = np.mean(signal, axis=0)
    S, phase = librosa.magphase(librosa.stft(averaged_signal))

    rms = librosa.feature.rmse(S=S)
    rms = rms.ravel()
    max_rms = rms.max()
    print("Max RMS:{}".format(max_rms))

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
            clap_nn = nn_detector( res, SAMPLE_RATE, frames_to_classify = 15 )
            clap_by_rms = rms_detector(res, SAMPLE_RATE, threshold=8.0)
            clap = clap_by_rms & clap_nn
        else:
            clap_logistic = logistic_detector( res, SAMPLE_RATE, frames_to_classify = 10 )
            clap_by_rms = rms_detector( res, SAMPLE_RATE, threshold = 10.0 )
            clap = clap_by_rms & clap_logistic
            #clap = clap_logistic

        if clap:
            print(">>>>&&&& There was a clap")
            if not DEBUG:
                bottle = yarp.Bottle()
                result = yarp.Bottle()
                print("Sending clap message")
                bottle.clear()
                bottle.addString("triggerNextMove")
                rpc_port.write( bottle, result )
                print("Got the result from RPC server")


        if len(whole_rec) > 0:
            whole_rec = np.hstack( (whole_rec, res) )
        else:
            whole_rec = res
        print("1 sec is gone:{}".format(whole_rec.shape[1]))
        #truncate
        #if whole_rec.shape[1] > SAMPLE_RATE * 2:
        #    whole_rec =

except KeyboardInterrupt:
    librosa.output.write_wav(RES_WAV_FILE + str(time.time()) + ".wav", whole_rec, SAMPLE_RATE)
    print("Written file")
except Exception,e:
    print(e)
finally:
    print("grace close")
    p.close()
    rpc_port.close()
    yarp.Network.fini()
    sys.exit()


