#getting base image
FROM ubuntu:18.04

MAINTAINER ajith <inocajith21.5@gmail.com>

RUN apt-get update
RUN apt-get install gcc-7 g++-7 -y
RUN apt install build-essential -y
RUN apt install libx11-dev -y

ADD UnsupervisedClustering /home/

WORKDIR /home/src

CMD ["g++-7", "main.cpp", "-o", "main.out", "-lpthread", "-lX11"]


CMD ["./main.out"]

