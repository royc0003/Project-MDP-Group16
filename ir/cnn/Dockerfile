FROM python:3.6.12-slim

WORKDIR /usr/src/app

COPY requirements.txt ./

RUN apt-get update
RUN apt-get install -y libgl1-mesa-dev
RUN apt-get install -y libglib2.0-0


RUN pip install --no-cache-dir -r requirements.txt

COPY . .

#CMD [ "python", "./latop_cnn.py" ]