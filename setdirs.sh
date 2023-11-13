#!/bin/bash
export CC=gcc
export CXX=g++
export PhysicalCores=$(cat /proc/cpuinfo | grep -m 1 "cpu cores" | awk '{print $ 4;}')

#export BOOST_ROOT=/home/ens/neil/libs/boost_1_78_0
export BOOST_ROOT=/home/ens/neil/libs/boost_1_80_0
export OSGDIR=/home/ens/neil/libs/osg
#export ODE_SRC=/home/ens/neil/libs/ode-0.11.1
export ODE_SRC=/home/ens/neil/libs/ode-0.16.2
export ODE_DIR=/home/ens/neil/libs/ode-bin
export PVLE_DIR=/home/ens/neil/libs/pvle
#export PVLE_DIR=/home/ens/neil/libs/pvle-profiling
export OPENALDIR=/home/ens/neil/libs/openal-soft
export ALUTDIR=/home/ens/neil/libs/freealut

export CMAKE_DIR=/home/ens/neil/libs/cmake-3.24.2-linux-x86_64/bin/
#export SWIG_DIR=/home/ens/neil/lib/swig-2.0.11

export PATH="$CMAKE_DIR:$PATH:.:$OSGDIR/lib:$BOOST_LOCALE_PATH:$BOOST_ROOT/stage/lib:/home/ens/neil/git/bin"
#export PATH="$PATH:.:$OSGDIR/lib:$BOOST_LOCALE_PATH:$BOOST_ROOT/stage/lib:/home/ens/neil/git/bin"
#export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:.:$OSGDIR/lib:$BOOST_ROOT/stage/lib:$ODE_DIR/lib:/home/ens/neil/git/bin:/usr/lib"
export LD_LIBRARY_PATH=".:$OSGDIR/lib:$BOOST_ROOT/stage/lib:$ODE_DIR/lib:$OPENALDIR:/home/ens/neil/git/bin:/usr/lib"

export CXXFLAGS="-std=c++11 $CXXFLAGS"

alias cm='$CMAKE_DIR/cmake-gui . &'

# Required for EPEM-UMLV to find GL driver
export LIBGL_DRIVERS_PATH=$LIBGL_DRIVERS_PATH:/usr/lib/x86_64-linux-gnu/dri
