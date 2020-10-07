FROM sgtwilko/rpi-raspbian-opencv:latest
COPY requirements.txt /
#RUN wget https://www.python.org/ftp/python/3.5.2/Python-3.5.2.tar.xz
#RUN tar -xvf Python-3.5.2.tar.xz
#RUN cd Python-3.5.2
#RUN ./configure
#RUN make
#RUN make altinstall
RUN apt-get update
RUN apt-get install build-essential tk-dev
RUN apt-get install libncurses5-dev libncursesw5-dev libreadline6-dev
RUN apt-get install libdb5.3-dev libgdbm-dev libsqlite3-dev libssl-dev
RUN apt-get install libbz2-dev libexpat1-dev liblzma-dev zlib1g-dev
RUN apt-get install libffi6 libffi-dev
RUN apt-get install cmake

RUN apt-get install git
RUN git clone https://github.com/pyenv/pyenv.git ~/.pyenv
RUN echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc && echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
RUN source ~/.bashrc
RUN pyenv install 3.5.6

RUN git clone https://github.com/pyenv/pyenv-virtualenv.git $(pyenv root)/plugins/pyenv-virtualenv


RUN pyenv virtualenv 3.5.6 ir-cnn
RUN eval "$(pyenv init -)"
RUN eval "$(pyenv virtualenv-init -)"
RUN pyenv activate ir-cnn
RUN pip install -U pip
RUN pip install setuptools==41.0.0
RUN pip install scikit-build
RUN apt-get install python3-dev
RUN pip install -r requirements.txt