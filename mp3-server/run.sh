#!/usr/bin/env bash 

docker run -it --rm -p 8081:8081 -v $(pwd):/home/node/app -w /home/node/app node:alpine node app.js