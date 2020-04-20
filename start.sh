#!/bin/bash

if [[ "$(tty)" == "/dev/tty1" ]]; then
  script -f ~/screen
fi
