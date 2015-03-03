#!/bin/bash

qmake-qt4 recordTracker.pro
uic-qt4 recordTracker.ui -o ui_mainwindow.h
make

