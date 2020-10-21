from keras.models import Model
from sklearn.preprocessing import LabelBinarizer
from keras.layers import Input, Dense, Conv2D, BatchNormalization, Flatten, MaxPooling2D, Lambda, ReLU
from keras.optimizers import Adam
import cv2
import imutils
import numpy as np
import math
from .helper import find_contour
import os


class CNN:
    def __init__(self, version):
        self.dim = 64
        self.lb = LabelBinarizer().fit(['circle', 'down', 'eight', 'left', 'nine', 'right', 'seven', 'six', 'up', 'v',
                                        'w', 'x', 'y', 'z', 'zero'])
        self.version = version
        self.font = cv2.FONT_HERSHEY_SIMPLEX
        self.local_image_count = 0
        if self.version == "gray":
            self.channels = 1
            self.model = self.getArchitecture()
            self.model.load_weights("./model/final.h5")  # load saved weights

        else:
            self.channels = 3
            self.model = self.getArchitecture()
            self.model.load_weights("./train/final_colour_final_1.h5")  # load saved weights

    # returns probability and class
    def predict(self, frame):
        img = np.reshape(frame, newshape=(1, self.dim, self.dim, self.channels))
        pred = self.model.predict(img)
        return self.lb.classes_[pred.argmax(axis=-1).item()], np.max(pred)

    # take in raw img and predict
    def raw_predict(self, img):
        # gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = img
        bbs = find_contour(img)
        results = {
            "l": None,
            "c": None,
            "r": None
        }
        for rotated_bb, position in bbs:
            potential_image = gray[rotated_bb[1]:rotated_bb[3], rotated_bb[0]:rotated_bb[2]]
            height, width , _ = potential_image.shape
            if height != 0:
                # print("here")
                # print(potential_image.shape)
                potential_image = cv2.resize(potential_image, (64, 64), interpolation=cv2.INTER_CUBIC)
                # potential_image = cv2.GaussianBlur(potential_image, (5, 5), 0)
                potential_image = potential_image / 255
                reg_img = self.predict(potential_image)
                category, prob = reg_img
                # cv2.rectangle(gray, (rotated_bb[0], rotated_bb[1]), (rotated_bb[2], rotated_bb[3]), (255, 255, 255),
                #               2)
                if prob >= 0.95:

                    if results.get(position) is None:
                        results[position] = {
                            "category": category,
                            "prob": prob
                        }

                    else:
                        prev_prob = results.get(position).get("prob")
                        if prob > prev_prob:
                            results[position] = {
                                "category": category,
                                "prob": prob
                            }

                    cv2.rectangle(gray, (rotated_bb[0], rotated_bb[1]), (rotated_bb[2], rotated_bb[3]), (255, 255, 255),
                                  2)
                    cv2.putText(gray, category + " " + str(position),
                                (rotated_bb[0], rotated_bb[1] + 40), self.font, 2, (255, 255, 255),
                                2)

        self.save_to_local(gray)
        for category in results:
            result = results.get(category)
            if result is not None:
                result["prob"] = str(result.get("prob"))

        return gray, results

    def save_to_local(self, img):
        cv2.imwrite('/Users/khoapham/Desktop/Projects/Project-MDP-Group16/ir/cnn/model/reg_image/' + str(
            self.local_image_count) + '.jpg', img)
        self.local_image_count += 1

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
