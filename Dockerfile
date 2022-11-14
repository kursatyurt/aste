# Ubuntu base
FROM ubuntu:latest

RUN apt-get update
RUN apt-get upgrade -y

# Set locale
RUN apt-get install locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y install tzdata

# Install dependencies
RUN apt-get update && apt-get install -y --show-progress --autoremove \
    build-essential \
    git \
    python3 \
    less \
    wget \
    bzip2 \
    pkg-config \
    cmake \
    vim \
    ca-certificates &&\
    wget https://github.com/precice/precice/releases/download/v2.5.0/libprecice2_2.5.0_jammy.deb  &&\
    apt-get -y install ./libprecice2_2.5.0_jammy.deb &&\
    rm -f libprecice2_2.5.0_jammy.deb &&\
    apt-get -y install libvtk7-dev &&\
    python3 -m pip install sympy scipy jinja2 &&\


RUN rm -rf /var/lib/apt/lists/*

COPY . /aste

ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

RUN rm -rf /aste/build/*

WORKDIR /aste
