#!/bin/bash

qmake recordTracker.pro
uic recordTracker.ui -o ui_mainwindow.h
make

