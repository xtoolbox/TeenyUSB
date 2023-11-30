#!/usr/bin/env bash

sudo apt-get install gcc-arm-none-eabi
cd sample
make all -j8
