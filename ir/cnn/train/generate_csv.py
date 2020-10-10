import csv
from os import listdir
from os.path import isfile, join

if __name__ == "__main__":

    DATA_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/cnn/cnn_data'
    DATA_SUB_DIR = [
        'circle',
        'up',
        'down',
        'left',
        'right',

        'eight',
        'nine',
        'seven',
        'six',
        'zero',

        'v',
        'w',
        'x',
        'y',
        'z'

        # # sub group 1
        # 'y',
        # 'down',
        # 'nine',

        # 'eight_2',
        # 'nine_2',
        # 'seven_2',
        # 'six_2',
        # 'zero_2'

    ]
    all_directory = [join(DATA_DIRECTORY, d) for d in DATA_SUB_DIR]

    with open('data.csv', 'w', newline='') as csvfile:
        csv_writer = csv.writer(csvfile, delimiter=',')
        csv_writer.writerow(['img_path', 'labels'])

    for directory in all_directory:
        print(directory)
        print(directory.split('/')[-1])
        all_images = [join(directory, f) for f in listdir(directory) if isfile(join(directory, f)) and (f.endswith('.jpg') or f.endswith('.JPG'))]
        print(all_images)
        label_list = [directory.split('/')[-1]] * all_images.__len__()
        rows = zip(all_images, label_list)
        with open('data.csv', 'a', newline='') as csvfile:
            csv_writer = csv.writer(csvfile, delimiter=',')
            for row in rows:
                csv_writer.writerow(row)


