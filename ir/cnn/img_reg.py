import cv2
import numpy as np

THRESHOLD = 0.999  # threshold to filter images
img_bound = 240  # top boundary to crop image
right_bound = 420  # right side of the image
left_bound = 150  # left side of the image
class_mapping = {'up': 1, 'down': 2, 'right': 3, 'left': 4, 'circle': 5, 'one': 6, 'two': 7,
                 'three': 8, 'four': 9, 'five': 10, 'a': 11, 'b': 12, 'c': 13, 'd': 14, 'e': 15}  # mapping for class id


# this function extracts the possible bounding rectangles
def getBoundingBoxes(contours):
    boxes = []

    # sort them in descending order based on the size of the bounding rect
    # then iterate through it
    for contour in sorted(contours, key=cv2.contourArea, reverse=True):
        area = cv2.contourArea(contour)
        if area < 2000:  # too small, exit from iteration
            break
        elif area > 12000:  # too big, skip
            continue
        rect = cv2.boundingRect(contour)
        x, y, w, h = rect
        if w < 20 or h < 20:  # too small, ignore
            break
        if w > 2 * h:  # width too big, unlikely our target
            continue
        boxes.append(np.array(rect))
    return np.array(boxes)


# main function to be called by HTTP server for the image recognition task
def run(frame, graph, model, count):
    cropped = frame[img_bound:, :]  # crop the frame
    gray = cv2.cvtColor(cropped, cv2.COLOR_RGB2GRAY)  # convert to grayscale
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)  # apply gaussian blur to blur the background
    thresh0 = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 51, 0)
    contours, _ = cv2.findContours(thresh0, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

    boxes = getBoundingBoxes(contours)
    predictions = []
    if len(boxes) > 0:
        for box in boxes:
            x, y, w, h = box
            if model.version == "gray":
                target = gray[y:y + h, x:x + w]  # crop out the bounding box image
            else:
                target = cropped[y:y + h, x:x + w]  # crop out the bounding box image
            resized = cv2.resize(target, dsize=(model.dim, model.dim),
                                 interpolation=cv2.INTER_CUBIC)  # resize to match CNN input size
            normed = resized / 255  # normalize before pass through CNN
            predictions.append((model.predict(graph, normed), box))

    # if no predictions
    if len(predictions) == 0:
        return -1, None, None, None

    bestResults = [None, 0, None]  # class label, prob, box

    # Passing through all the possible bounding rectangles through the
    # CNN may produce multiple predictions of different classes.
    # Below extracts the predictions that are above our defined threshold,
    # and find the best prediction - highest probability
    for pred in predictions:
        prob = pred[0][1].item()
        if prob > THRESHOLD and prob > bestResults[1]:
            bestResults[1] = prob
            bestResults[0] = pred[0][0]
            bestResults[2] = pred[1]

    # below occurs when every prediction fails the threshold comparison
    if bestResults[1] == 0:
        return -1, None, None, None

    x, y, w, h = bestResults[2]
    y = y + img_bound
    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 1)  # write the bounding rectangle on the image
    text = "{}: {:.4f}%".format(bestResults[0].upper(), bestResults[1] * 100)
    cv2.putText(frame, text, (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0),
                2)  # write the class label of the image
    filename = './processed/p_img' + str(count) + '.jpg'  # name of processed image
    cv2.imwrite(filename, frame)  # save the processed image

    # below finds the position of the bounding rectangle, to facilitate coordinates finding
    pos = None
    if x < left_bound:
        pos = "l"
    elif x > right_bound:
        pos = "r"
    else:
        pos = "c"
    return class_mapping.get(bestResults[0]), filename, w * h, pos