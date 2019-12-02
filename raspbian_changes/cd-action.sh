#!/bin/bash

# Taken from https://somewideopenspace.wordpress.com/yet-another-headless-cd-ripper-and-mpd-on-raspberry-pi-installation-guide-yahcdramorpig/start-ripping-when-cd-is-inserted/

LOGFILE=/var/log/cdrip.log
echo "$(date)" >> $LOGFILE
(
  # Wait for lock on /var/lock/audio-cd-rip.lock (fd 200) for two hours
  flock -x -w 7200 200 || exit 1 # Prevent multiple acces

  # First, make sure that we are using the right output
  . /home/pi/outputs.sh
  SetOut 5

  abcde -N 2>&1 >> $LOGFILE
  rc=$?
  if [[ $rc != 0 ]] ; then
    exit $rc
  fi
) 200>/var/lock/audio-cd-rip.lock
