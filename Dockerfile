FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    gcc \
    git \
    python3 \
    python3-pip \
    cmake \
    pkg-config \
    sqlite3 \
    qt5-qmake \
    qt5-default \
    mercurial \
    gsl-bin \
    gnuplot \
    libgsl-dev \
    libgtk-3-dev \
    vtun \
    lxc \
    uncrustify \
    doxygen \
    graphviz \
    imagemagick \
    texlive \
    texlive-extra-utils \
    texlive-latex-extra \
    texlive-font-utils \
    texlive-fonts-recommended \
    dvipng \
    latexmk \
    python3-sphinx \
    dia \
    tcpdump \
    libc6-dev \
    binutils-dev \
    gdb \
    valgrind \
    lcov \
    xvfb \
    cvs \
    rcs \
    bzr \
    libjpeg-dev \
    python3-pygraphviz \
    python3-dev \
    python3-setuptools \
    openmpi-bin \
    openmpi-common \
    openmpi-doc \
    libopenmpi-dev \
    autoconf \
    unrar \
    gdb \
    vim \
    emacs \
    mpich \
    libmpich-dev \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

# Clone and build NS3
RUN git clone https://gitlab.com/nsnam/ns-3-allinone.git && \
    cd ns-3-allinone && \
    ./download.py -n ns-3.33 && \
    cd ns-3.33 && \
    ./waf configure --enable-examples --enable-tests && \
    ./waf build

COPY iot_simulation.cpp /ns-3-allinone/ns-3.33/scratch/

WORKDIR /ns-3-allinone/ns-3.33/
RUN ./waf build

ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
