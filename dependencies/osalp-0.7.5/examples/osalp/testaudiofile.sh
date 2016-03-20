#!/bin/sh
#
# OSALP Test script.
#
# This script is just a quick sanity check of OSALP on lossless conversions.
# Taken from sox test.sh script.

# verbose options
#noise=-v

./osalp $noise -f AU monkey.au -B -f WAV monkey1.wav


ext=AIFF
./osalp $noise -f WAV monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f WAV monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between WAV and $ext was successful"
else
    echo "Error converting between WAV and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=AIFC
./osalp $noise -f WAV monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f WAV monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between WAV and $ext was successful"
else
    echo "Error converting between WAV and $ext."
fi
rm -f convert.$ext monkey2.wav

# AU doesn't support unsigned so use signed
ext=AU
./osalp $noise -f WAV monkey1.wav -b -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f WAV monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between WAV and $ext was successful"
else
    echo "Error converting between WAV and $ext."
fi
rm -f convert.$ext monkey2.wav


exit
