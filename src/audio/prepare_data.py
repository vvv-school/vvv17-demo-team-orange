import datetime
from collections import defaultdict
import soundfile
import numpy as np
from scipy.stats import kurtosis, skew, linregress
from librosa.feature import melspectrogram
from path import Path
import os
import librosa
import functools

percentile_1 = functools.partial( np.percentile, axis = 0, q = 0.01 )
percentile_99 = functools.partial( np.percentile, axis = 0, q = 0.99)

def linear_slope(arr):
    all_size = len(arr)
    x = np.arange(all_size)
    slope, _, _, _, _ = linregress(x, arr)
    return slope


def linear_intercept(arr):
    all_size = len(arr)
    x = np.arange(all_size)
    _, intercept, _, _, _ = linregress(x, arr)
    return intercept

percentile_1 = functools.partial( np.percentile, axis = 0, q = 0.01 )
percentile_99 = functools.partial( np.percentile, axis = 0, q = 0.99)

DEAFULT_FUNCTIONALS = [ np.mean, np.std, kurtosis, skew,
                    percentile_1, percentile_99,linear_slope, linear_intercept ]


def apply_functional( spectrogram, func ):
    return np.apply_along_axis( func1d = func, axis = 0, arr = spectrogram )



def compose_vector_of_functionals( spectrogram, lst_functionals = None ):
    lst_functionals = lst_functionals or DEAFULT_FUNCTIONALS
    intermediate_vectors = []
    for func in lst_functionals:
        res = apply_functional( spectrogram, func )
        intermediate_vectors.append( res )
    return np.hstack( intermediate_vectors )

def load_annotations( file ):
    res = []
    with open( file ) as f:
        for l in f:
            l = l.strip()
            if len(l) > 0:
                tokens = l.split()
                time_start, time_end = float( tokens[0] ), float( tokens[1] )
                res.append( {
                    "time_start" : time_start,
                    "time_end" : time_end
                } )
        return res

def get_msec(ts):
    return 60 * 60 * ts.hour* 1000 + 60*ts.minute*1000 + ts.second * 1000 + ts.microsecond / 1000

def get_msec_from_sec(second):
    return int(second * 1000)

def check_if_timing_in_annotations( timing, annotations, threshold = 15 ):
    timing_second = get_msec(timing)
    for ann in annotations:
        ann_start = get_msec_from_sec(ann["time_start"])
        ann_end = get_msec_from_sec(ann["time_end"])
        if timing_second > ann_start - threshold and timing_second < ann_end + threshold:
            return True
    return False


def spectrogram_from_file_librosa( filename,step  = 10, window = 20, max_freq = None ):
    with soundfile.SoundFile(filename) as sound_file:
        audio = sound_file.read(dtype='float32')
        sample_rate = sound_file.samplerate
        if audio.ndim >= 2:
            audio = np.mean(audio, 1)
        if max_freq is None:
            max_freq = sample_rate / 2
        if max_freq > sample_rate / 2:
            raise ValueError("max_freq must not be greater than half of "
                             " sample rate")
        if step > window:
            raise ValueError("step size must not be greater than window size")
        hop_length = int(0.001 * step * sample_rate)
        fft_length = int(0.001 * window * sample_rate)

        #return np.transpose( melspectrogram(audio, sr = sample_rate,hop_length=hop_length,
        #                                    n_fft = fft_length, fmax = max_freq) ).astype(np.float32)
        S = melspectrogram(audio, sr = sample_rate,hop_length=hop_length,
                                            n_fft = fft_length, fmax = max_freq, n_mels = 40)
        mfcc = librosa.feature.mfcc(n_mfcc = 13, S=librosa.logamplitude(S))
        mfcc_delta = librosa.feature.delta(mfcc)

        total_mfcc = np.vstack( (mfcc, mfcc_delta) )
        #total_mfcc = mfcc
        return np.transpose(total_mfcc ).astype(np.float32)


def create_temporal_dataset( directory ):
    """
    Returns list of sequences with frame-by_frame annotations.
    Raw spectrograms are used as features.
    """
    annotations_by_wav_file = defaultdict(list)
    for f in Path(directory).walkfiles("*.txt"):
        filename = str(f)
        annotations = load_annotations( filename )
        wav_file = os.path.abspath( filename[:-4]+".wav" )
        for ann in annotations:
            annotations_by_wav_file[ wav_file ].append( ann )

    X, Y = [], []
    STEP_SIZE_MS = 15
    for audio_file, lst_annotations in annotations_by_wav_file.items():
        sequence_X = []
        sequence_Y = []
        spectrogram = spectrogram_from_file_librosa(audio_file, step=STEP_SIZE_MS,
                                                    window = 25, max_freq = 8000)


        initial_timing = datetime.datetime( hour = 0, minute = 0, second  = 0,
                                            year = 2016, month = 1, day = 1 )
        timings = [initial_timing + datetime.timedelta( milliseconds = STEP_SIZE_MS * i )
                   for i in range(spectrogram.shape[0]) ]
        features = [{"feature" : f, "timing" : t }
                    for f, t in zip( spectrogram, timings )]
        for data_struct in features:
            mfcc = data_struct["feature"]

            timing = data_struct["timing"]

            sequence_X.append(mfcc)
            if check_if_timing_in_annotations(timing, lst_annotations, threshold = 15):
                sequence_Y.append(1)
            else:
                sequence_Y.append(0)
        X.append( sequence_X )
        Y.append( sequence_Y )

    return X,Y


def get_sequences(lst_all_sequences, lst_all_sequence_labels, sequence_length, sequence_hop):

    res_X = []
    res_Y = []
    for seq_X, seq_Y in zip( lst_all_sequences, lst_all_sequence_labels ):
        for i in range(0, len( seq_X ), sequence_hop):
            subset_X = seq_X[i:i+sequence_length]
            subset_X = np.array(subset_X)

            subset_Y = np.array(seq_Y[i:i+sequence_length])

            if len(subset_X) < sequence_length:
                continue
            if np.sum(subset_Y) == 0:
                label = 0
            else:
                label = 1

            if label == 0:
                if np.random.random() > 0.3:
                    continue
            squashed_X = compose_vector_of_functionals( subset_X )
            res_X.append(  np.expand_dims( squashed_X, axis = 0 )  )
            res_Y.append( label )

    return np.vstack( res_X ), np.vstack( res_Y )


def get_data( wav_file, annotation_file ):
    pass

if __name__ == "__main__":
    #print( load_annotations( "./data/file_2.txt" ) )
    X, Y = create_temporal_dataset( "./data" )

    X_seq, Y_seq = get_sequences( X, Y, sequence_length = 3, sequence_hop = 3 )
    print(X_seq.shape)
    print(Y_seq.shape)