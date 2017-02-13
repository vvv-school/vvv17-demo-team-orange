from collections import Counter
from docutils.nodes import Sequential
import numpy as np
import os
import joblib
import sklearn
from sklearn.cross_validation import train_test_split
from sklearn.decomposition import PCA
from sklearn.linear_model import LogisticRegression
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.svm import LinearSVC
from sklearn.svm import SVC

np.random.seed(12345)
from keras.callbacks import EarlyStopping, ModelCheckpoint
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix

from prepare_data import create_temporal_dataset, get_sequences
from keras.layers import GRU, TimeDistributed, Dense, LSTM, Convolution1D, BatchNormalization, GaussianNoise, \
    GlobalAveragePooling1D, Flatten, Dropout
from keras.models import Sequential
from keras.optimizers import Adam
from keras.models import Model
from keras.regularizers import l2

def get_feature_mean_std(data):
    #feats = np.vstack([f for seq in sequences for f in seq])
    feats_mean = np.mean(data, axis=0)
    feats_std = np.std(data, axis=0)
    return feats_mean, feats_std

def normalize_sequence(seq, means, std):
    res = []
    for sample in seq:
        res.append( (sample - means)/(std + 1e-8) )
    return np.array(res)



def get_model(num_features):
    reg = 1e-3
    model = Sequential()
    model.add(GaussianNoise(sigma = 0.15, input_shape=(num_features,)))
    model.add( Dropout(p=0.2) )
    model.add( Dense(50, activation = "relu",W_regularizer= l2( reg )  ) )
    model.add( BatchNormalization() )

    model.add(Dense(output_dim=1, activation="sigmoid"))
    optimizer = Adam(lr=1e-4)
    model.compile(optimizer, loss="binary_crossentropy", metrics=["accuracy",
                                                                  "fmeasure"])

    return model


if __name__ == "__main__":
    CACHE_FILE = "./cache/dataset.dat"
    #if not os.path.exists( CACHE_FILE ):
    X, Y = create_temporal_dataset("./data")
    X,Y = sklearn.utils.shuffle( X,Y )

    TRAIN_NN_MODEL = True
    #    joblib.dump(CACHE_FILE, (X,Y))
    #else:
    #    X, Y = joblib.load( CACHE_FILE )

    #X, Y = get_sequences(X, Y, sequence_length = NUM_FRAMES, sequence_hop=2)
    X, Y = get_sequences(X, Y, max_seq_length = 15)

    #train_size = 0.70
    #train_samples = int(len(X) * train_size)
    #X_train_seq, Y_train_seq = X[:train_samples], Y[:train_samples]
    #X_val_seq, Y_val_seq = X[train_samples:], Y[train_samples:]
    X_train_seq, X_val_seq, Y_train_seq, Y_val_seq = train_test_split(X,Y, test_size = 0.15)

    print(Counter(Y_train_seq.ravel()), Counter(Y_val_seq.ravel()))
    print(X.shape)
    print("Total train/val sequences: {}/{}".format(len(X_train_seq), len(X_val_seq)))


    if TRAIN_NN_MODEL:
        train_mean, train_std = get_feature_mean_std(X_train_seq)
        joblib.dump( (train_mean, train_std), "./cache/train_mean_std.dat" )
        #print(train_mean)
        #print(X_train_seq[0])
        #X_train_seq = np.array([(seq - train_mean) / (train_std + 1e-8) for seq in X_train_seq ])
        #X_val_seq = np.array([(seq - train_mean) / (train_std + 1e-8) for seq in X_val_seq ])
        #print(X_train_seq[0])
        X_train_seq = ( X_train_seq - train_mean ) / (train_std+ 1e-8)
        X_val_seq = ( X_val_seq - train_mean ) / (train_std + 1e-8)
        #X_train_seq = np.array([normalize_sequence(seq, train_mean, train_std) for seq in X_train_seq])
        #X_val_seq = np.array([normalize_sequence(seq, train_mean, train_std) for seq in X_val_seq])

        model = get_model(X_train_seq.shape[-1])
        model.summary()
        early_stopping = EarlyStopping(monitor='val_loss', mode="min", patience=7)
        #checkpointer = ModelCheckpoint(monitor='val_fmeasure', mode="max",
        #                               save_best_only=True, filepath="./models/rnn_model.h5", verbose=1)
        checkpointer = ModelCheckpoint(monitor='val_loss', mode="min",
                                                                      save_best_only=True,
                                       filepath="./models/rnn_model.h5", verbose=1)

        model.fit(X_train_seq, Y_train_seq, validation_data=(X_val_seq, Y_val_seq), verbose=2,
                  batch_size=16, callbacks=[ early_stopping, checkpointer ], nb_epoch = 25, class_weight='auto')

        model.load_weights("./models/rnn_model.h5")
        Y_val_predictions = model.predict(X_val_seq, batch_size=16)
        Y_val_predictions = Y_val_predictions.ravel()
        Y_val_predictions = np.array([1 if v > 0.75 else 0 for v in Y_val_predictions])
        # print(Y_val_predictions)
        # print(Y_val_predictions[0])
        Y_train_prediction = model.predict(X_train_seq, batch_size=16)
        Y_train_prediction = Y_train_prediction.ravel()
        Y_train_prediction = [1 if v > 0.75 else 0 for v in Y_train_prediction]
    else:
        print("back to 90's")
        model = Pipeline([
            ("normalize", StandardScaler()),
            #("pca", PCA( n_components = 25 )),
            #("clf", LinearSVC( C = 1, class_weight = "auto" ))
            ("clf", LogisticRegression( class_weight= "auto" ) )
        ])
        model.fit( X_train_seq, Y_train_seq )
        joblib.dump(model,"./cache/logistic.dat")
        Y_val_predictions = model.predict(X_val_seq)
        # print(Y_val_predictions)
        # print(Y_val_predictions[0])
        Y_train_prediction = model.predict(X_train_seq)
        #Y_train_prediction = Y_train_prediction.ravel()
        #Y_train_prediction = [1 if v > 0.75 else 0 for v in Y_train_prediction]

    print(confusion_matrix(Y_train_seq, Y_train_prediction))
    print(classification_report(Y_train_seq, Y_train_prediction))

    #Y_val_predictions = [1 if v > 0.75 else 0 for v in Y_val_predictions ]
    print(confusion_matrix(Y_val_seq, Y_val_predictions))
    print(classification_report(Y_val_seq, Y_val_predictions))