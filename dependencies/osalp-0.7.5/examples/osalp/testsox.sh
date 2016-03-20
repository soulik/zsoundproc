#!/bin/sh
#
# SOX Test script.
#
# This script is just a quick sanity check of SOX on lossless conversions.

# verbose options
#noise=-v

./osalp $noise -f au monkey.au -B -f ub raw1.ub

# Convert between unsigned bytes and signed bytes
./osalp $noise -r 8012 -c 1 -f ub raw1.ub -f sb raw1.sb
./osalp $noise -r 8012 -c 1 -f sb raw1.sb -f ub raw2.ub
if cmp -s raw1.ub raw2.ub
then
    echo "Conversion between unsigned bytes and signed bytes was successful"
else
    echo "Error converting between signed and unsigned bytes"
fi
rm -f raw1.sb raw2.ub

./osalp $noise -r 8012 -c 1 -f ub raw1.ub -f sw raw1.sw
./osalp $noise -r 8012 -c 1 -f sw raw1.sw -f ub raw2.ub
if cmp -s raw1.ub raw2.ub
then
    echo "Conversion between unsigned bytes and signed words was successful"
else
    echo "Error converting between signed words and unsigned bytes"
fi
rm -f raw1.sw raw2.ub

./osalp $noise -r 8012 -c 1 -f ub raw1.ub -f al raw1.al
./osalp $noise -r 8012 -c 1 -f al raw1.al -f ub raw2.ub
if cmp -s raw1.ub raw2.ub
then
    echo "Conversion between unsigned bytes and alaw bytes was successful"
else
    echo "Error converting between alaw and unsigned bytes"
fi
rm -f raw1.al raw2.ub


./osalp $noise -r 8012 -c 1 -f ub raw1.ub -f uw raw1.uw
./osalp $noise -r 8012 -c 1 -f uw raw1.uw -f ub raw2.ub
if cmp -s raw1.ub raw2.ub
then
    echo "Conversion between unsigned bytes and unsigned words was successful"
else
    echo "Error converting between unsigned words and unsigned bytes"
fi
rm -f raw1.uw raw2.ub

./osalp $noise -r 8012 -c 1 -f ub raw1.ub -f sl raw1.sl
./osalp $noise -r 8012 -c 1 -f sl raw1.sl -f ub raw2.ub
if cmp -s raw1.ub raw2.ub
then
    echo "Conversion between unsigned bytes and signed long was successful"
else
    echo "Error converting between signed long and unsigned bytes"
fi
rm -f raw1.sl raw2.ub

#./osalp $noise -r 8012 -c 1 raw1.ub -f -l raw1.raw
#./osalp $noise -r 8012 -c 1 -f -l raw1.raw raw2.ub
#if cmp -s raw1.ub raw2.ub
#then
#    echo "Conversion between unsigned bytes and float was successful"
#else
#    echo "Error converting between float and unsigned bytes"
#fi
#rm -f raw1.raw raw2.ub

#rm -f raw1.ub
./osalp $noise -f au monkey.au -f sw raw1.sw

./osalp $noise -r 8012 -c 1 -f sw raw1.sw -f ul raw1.ul
./osalp $noise -r 8012 -c 1 -f ul raw1.ul -f sw raw2.sw
if cmp -s raw1.sw raw2.sw
then
    echo "Conversion between signed words and ulaw bytes was successful"
else
    echo "Error converting between ulaw and signed words"
fi
rm -f raw1.ul raw2.sw

rm -f raw1.sw

./osalp $noise -f au monkey.au -B -f wav monkey1.wav

echo ""

ext=8svx
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=aiff
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

# AU doesn't support unsigned so use signed
ext=au
./osalp $noise -f wav monkey1.wav -b -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=avr
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=dat
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=hcom
# HCOM has to be at specific sample rate.
./osalp $noise -r 5512 -f wav monkey1.wav -B -f wav nmonkey1.wav
./osalp $noise -f wav nmonkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s nmonkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext nmonkey1.wav monkey2.wav

ext=maud
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=sf
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=smp
./osalp $noise -f wav monkey1.wav -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s monkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext monkey2.wav

ext=voc
./osalp $noise -r 8000 -f wav monkey1.wav -B -f wav nmonkey1.wav
./osalp $noise -f wav nmonkey1.wav -B -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s nmonkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext nmonkey1.wav monkey2.wav

ext=wve
./osalp $noise -r 8000 -f wav monkey1.wav -B -f wav nmonkey1.wav
./osalp $noise -f wav nmonkey1.wav -B -f $ext convert.$ext
./osalp $noise -f $ext convert.$ext -B -f wav monkey2.wav
if cmp -s nmonkey1.wav monkey2.wav
then
    echo "Conversion between wav and $ext was successful"
else
    echo "Error converting between wav and $ext."
fi
rm -f convert.$ext nmonkey1.wav monkey2.wav

exit
