#!/bin/sh

cd build 
cmake ..
make -j 16 && /opt/SEGGER/JLink/JLinkExe -commanderscript ../flash.jlink
