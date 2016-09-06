#!/bin/sh

ARCHIVE=qsdata-exe-`date -I`.zip
DIRECTORY=../../build-qsdata-Desktop_Qt_5_7_0_MSVC2015_64bit-Release/qsdata/release

cd  $DIRECTORY
zip $ARCHIVE   -r . -x '*.obj'
mv  $ARCHIVE   ../../../qsdata/var
cd             ../../../qsdata/var
