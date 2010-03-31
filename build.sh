#!/bin/bash
BUILDDIR=_build_
if ! [ -d $BUILDDIR ]; then
    mkdir $BUILDDIR
fi
cd $BUILDDIR
cmake .. -DCMAKE_BUILD_TYPE=Debug &&\
#cmake .. -DCMAKE_BUILD_TYPE=Release &&\
make
