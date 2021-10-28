#!/bin/sh

docker run --gpus all --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864 -it --rm --ipc=host opencl bash
