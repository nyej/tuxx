#!/usr/bin/env bash

image=$(docker build -q .)
docker run -it --rm $image

if [[ $# -eq 0 ]]; then
    # any argument provided prevents image deletion.
    docker rmi $image
fi
