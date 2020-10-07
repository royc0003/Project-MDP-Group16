from keras.models import Model
from sklearn.preprocessing import LabelBinarizer
from keras.layers import Input, Dense, Conv2D, BatchNormalization, Flatten, MaxPooling2D, Lambda, ReLU
from keras.optimizers import Adam
import joblib
import numpy as np


class CNN:
    def __init__(self, version):
        self.dim = 64
        # self.lb = LabelBinarizer().fit(['eight', 'nine', 'seven', 'six', 'zero'])
        self.lb = LabelBinarizer().fit(
        ['circle', 'down', 'eight', 'left', 'nine', 'right', 'seven', 'six',
         'up', 'v', 'w', 'x', 'y', 'z', 'zero']

                # ['eight_2', 'nine_2', 'seven_2', 'six_2', 'zero_2']

        )

        # ['circle', 'down', 'eight', 'left', 'nine', 'right', 'seven', 'six',
        #  'up', 'v', 'w', 'x', 'y', 'z', 'zero']
        print(self.lb.classes_)
        self.version = version

        if self.version == "gray":
            self.channels = 1
            self.model = self.getArchitecture()
            self.model.load_weights("./final.h5")  # load saved weights

    # returns probability and class
    def predict(self, frame):
        img = np.reshape(frame, newshape=(1, self.dim, self.dim, self.channels))
        pred = self.model.predict(img)
        return (self.lb.classes_[pred.argmax(axis=-1).item()], np.max(pred))

    # graph architecture of cnn
    def getArchitecture(self):
        input_layer = Input(shape=(self.dim, self.dim, self.channels))
        x = Conv2D(32, (3, 3), padding="same")(input_layer)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = Conv2D(32, (3, 3), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = MaxPooling2D()(x)

        x = Conv2D(64, (5, 5), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = Conv2D(64, (5, 5), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = MaxPooling2D()(x)

        x = Conv2D(128, (5, 5), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = Conv2D(128, (5, 5), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = MaxPooling2D()(x)

        x = Conv2D(256, (5, 5), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = Conv2D(256, (5, 5), padding="same")(x)
        x = BatchNormalization()(x)
        x = ReLU()(x)
        x = MaxPooling2D()(x)

        x = Flatten()(x)
        x = Dense(512, activation="relu")(x)
        output = Dense(15, activation="softmax")(x)

        # Connect the inputs with the outputs
        cnn = Model(inputs=input_layer, outputs=output)
        cnn.compile(loss="categorical_crossentropy", optimizer=Adam(lr=0.00006, decay=1e-6))
        return cnn
