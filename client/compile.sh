#!/bin/bash

gcc -Wall bbfs.c log.c `pkg-config fuse --cflags --libs` -I./include -o bbfs