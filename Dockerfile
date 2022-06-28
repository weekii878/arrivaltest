FROM ubuntu:20.04

WORKDIR /arrivaltest

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install libboost-all-dev cmake build-essential -y

COPY . .

ENTRYPOINT ["./build_and_run.sh"]
