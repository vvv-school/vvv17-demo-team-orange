import datetime
from collections import defaultdict
import soundfile
import numpy as np
from librosa.feature import melspectrogram
from path import Path
import os

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

def check_if_timing_in_annotations( timing, annotations, threshold = 15 ):
    timing_second = get_msec(timing)
    for ann in annotations:
        ann_start = get_msec(ann["start_ts"])
        ann_end = get_msec(ann["end_ts"])
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

        return np.transpose( melspectrogram(audio, sr = sample_rate,hop_length=hop_length,
                                            n_fft = fft_length, fmax = max_freq) ).astype(np.float32)

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
    for audio_file, lst_annotations in annotations_by_wav_file.items():
        sequence_X = []
        sequence_Y = []
        spectrogram = spectrogram_from_file_librosa(audio_file, step=15,
                                                    window = 25, max_freq = 8000)

        initial_timing = datetime.datetime( hour = 0, minute = 0, second  = 0,
                                            year = 2016, month = 1, day = 1 )
        timings = [initial_timing + datetime.timedelta( milliseconds = 15 * i )
                   for i in range(spectrogram.shape[0]) ]
        features = [{"feature" : f, "timing" : t }
                    for f, t in zip( spectrogram, timings )]
        for data_struct in features:
            mfcc = data_struct["feature"]
            timing = data_struct["timing"]

            sequence_X.append(mfcc)
            if check_if_timing_in_annotations(timing, lst_annotations, threshold = 10):
                sequence_Y.append(1)
            else:
                sequence_Y.append(0)
        X.append( sequence_X )
        Y.append( sequence_Y )

    return X,Y


def get_data( wav_file, annotation_file ):
    pass

if __name__ == "__main__":
    #print( load_annotations( "./data/file_2.txt" ) )
    X, Y = create_temporal_dataset( "./data" )