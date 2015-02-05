###Kinect-Experiments

This is the current state of my experiments with the Microsoft Kinect.

It consists of two applications, LiveTraker and RecordTraker. Their purpose is to track objects using the Kinect, for LiveTracker in realtime and for RecordTracker, using pre-recorded data produced by the utility Record that is part of libfreenect.

Dependencies:

* libfreenect
* QT5 (Can work with QT4 but need to mess around a litle)
* OpenCV
* CVBlob

Libfreenect and CVBlob have been added to this repository as submodules.

I am also using a widget from here:
http://qt-project.org/forums/viewthread/19757
