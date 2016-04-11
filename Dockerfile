# Ubuntu 14.04
FROM ubuntu:14.04

# Install.
RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y build-essential && \
  apt-get install -y software-properties-common && \
  apt-get install -y byobu curl git htop man unzip vim wget && \
  rm -rf /var/lib/apt/lists/*

# Add files.

ADD rng-mlfg_64bit src/rng-mlfg_64bit
ADD rng-mt19937_32bit src/rng-mt19937_32bit
ADD rng-tinymt32 src/rng-tinymt32
ADD rng-tinymt64 src/rng-tinymt64
ADD rng-well512a src/rng-well512a
ADD rng-well1024a src/rng-well1024a
ADD rng-mrg32k3a src/rng-mrg32k3a

# Clean of Dockerfiles
RUN rm src/*/Dockerfile

# Build run
RUN cd src/rng-mlfg_64bit/src; make all
RUN cd src/rng-mt19937_32bit/src; make all
RUN cd src/rng-tinymt32/src; make all
RUN cd src/rng-tinymt64/src; make all
RUN cd src/rng-well512a/src; make all
RUN cd src/rng-well1024a/src; make all
RUN cd src/rng-mrg32k3a/src; make all

# Moving to workdir
WORKDIR /src

# Define default command.
CMD ["bash"]