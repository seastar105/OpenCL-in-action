ARG FROM_IMAGE=nvidia/opencl
FROM ${FROM_IMAGE}

RUN mkdir -p /workspace
ADD . /workspace
RUN apt update -y && apt install build-essential -y
