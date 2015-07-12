This is a modified record utility for the Kinect. The original code is from libfreenect's fakenect-record. The dump files are no longer saved, an option has been added to set a maximum record time and a motion detection trigger that enables recording.

For example to record 20 seconds of data with the trigger enabled:
$ krec -trigger 50 -rectime 20 test

The trigger sensitivity increases the lower it's value is.

