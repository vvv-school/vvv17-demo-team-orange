import numpy as np
import joblib
from prepare_data import get_spectrogram, compose_vector_of_functionals, spectrogram_from_file_librosa
from rnn_model import get_model
import datetime

model = get_model( num_features= 248 )
model.load_weights( "./models/rnn_model.h5" )
train_mean, train_std = joblib.load( "./cache/train_mean_std.dat" )

def nn_detector( wave_file, frames_to_classify = 15 ):
    features = spectrogram_from_file_librosa(wave_file, step=15, min_freq = 700,
                                                    window = 15, max_freq = 8000)
    input = []

    initial_timing = datetime.datetime(hour=0, minute=0, second=0,
                                       year=2016, month=1, day=1)
    timings = []

    i = 0
    while i < features.shape[0]:
    #for i in range(0, features.shape[0], frames_to_classify):
        subset = features[i:i + frames_to_classify]
        if len(subset) < frames_to_classify:
            i += frames_to_classify // 2
            continue
        input.append( np.expand_dims(compose_vector_of_functionals(subset), axis = 0) )
        timings.append( initial_timing + datetime.timedelta(milliseconds=15 * i) )
        i += frames_to_classify // 2
    print("predicting")
    input = np.vstack( input )
    input = (input - train_mean) / (train_std + 1e-8)
    pred = model.predict( input )
    pred = pred.ravel()

    pred = [1 if p > 0.5 else 0 for p in pred]

    for (p,t) in zip(pred, timings):
        if p >0.5:
            print("{}:{}".format( t.minute * 60 + t.second, t.microsecond ))
    return False


if __name__ == "__main__":
    nn_detector("./tr00112.wav")