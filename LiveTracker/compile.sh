#!/bin/bash
qmake-qt4 liveTracker.pro
make clean;rm qttrack;make;./qttrack
