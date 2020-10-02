import cv2
import numpy as np
import matplotlib.pyplot as plt

# pick one upright image
PATH_TO_IMAGE = "/Users/ongcj/Dev/mdp/ir/data/processed/nine_resized/IMG_0262_RESIZED_RANDOM_RESIZE_RESIZED.JPG"


def left_perspective_coordinates(img, percent):
    y, x, ch = img.shape
    # calculate new coordinates
    # reduce left by 20%, need to reduce y
    beta = y/100 * percent

    current_coordinates = np.float32([[0, 0], [x, 0], [0, y], [x, y]])
    new_coordinates = np.float32([[0, 0],
                                  [x, beta], # change
                                  [0, y],
                                  [x, y - beta]] # change
                                 )

    return current_coordinates, new_coordinates, (x, y)


def right_perspective_coordinates(img, percent):
    y, x, ch = img.shape
    # calculate new coordinates
    # reduce left by 20%, need to reduce y
    alpha = x/100 * percent

    current_coordinates = np.float32([[0, 0], [x, 0], [0, y], [x, y]])
    new_coordinates = np.float32([[0, alpha], # change
                                  [x, 0],
                                  [0, y - alpha], # change
                                  [x, y]]
                                 )

    return current_coordinates, new_coordinates, (x, y)


if __name__ == "__main__":
    img = cv2.imread(PATH_TO_IMAGE)

    for percent in range(10, 25, 5):
        pts1, pts2, end_size = right_perspective_coordinates(img, percent)
        M = cv2.getPerspectiveTransform(pts1, pts2)
        dst = cv2.warpPerspective(img, M, end_size)
        plt.subplot(121), plt.imshow(img), plt.title('Input')
        plt.subplot(122), plt.imshow(dst), plt.title('Output')
        plt.show()
