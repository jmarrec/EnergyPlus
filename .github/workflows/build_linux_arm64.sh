#!/bin/bash
set -e
set -x

apt-get -y install texlive texlive-xetex texlive-science libxkbcommon-x11-0 xorg-dev libgl1-mesa-dev gfortran-11

git clone https://github.com/pyenv/pyenv.git ~/.pyenv
export PATH=$PATH:~/.pyenv/bin
eval "$(pyenv init -)"
PYTHON_CONFIGURE_OPTS="--enable-shared" pyenv install $Python_REQUIRED_VERSION

export C=/usr/bin/gcc-11
export CXX=/usr/bin/g++-11
export FC=/usr/bin/f95

cmake -G Ninja -DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE \
  -DBUILD_TESTING:BOOL=OFF -DBUILD_FORTRAN:BOOL=ON \
  -DDOCUMENTATION_BUILD:STRING=BuildOnlyWithPackage -DTEX_INTERACTION:STRING="batchmode" -DENABLE_PCH:BOOL=OFF \
  -DENABLE_GTEST_DEBUG_MODE=OFF \
  -DBUILD_PACKAGE:BOOL=ON -DCPACK_BINARY_IFW:BOOL=OFF -DCPACK_BINARY_STGZ:BOOL=OFF -DCPACK_BINARY_TGZ:BOOL=ON \
  -DLINK_WITH_PYTHON:BOOL=ON -DPython_REQUIRED_VERSION:STRING=$Python_REQUIRED_VERSION \
  -DPython_ROOT_DIR:PATH=$HOME/.pyenv/versions/$Python_REQUIRED_VERSION \
  -DPYTHON_EXECUTABLE:FILEPATH=$HOME/.pyenv/versions/$Python_REQUIRED_VERSION/bin/python3.8 \
  -DPYTHON_INCLUDE_DIR:PATH=$HOME/.pyenv/versions/$Python_REQUIRED_VERSION/include/python3.8 \
  -DPYTHON_LIBRARY:FILEPATH=$HOME/.pyenv/versions/$Python_REQUIRED_VERSION/lib/libpython3.8.so \
  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON \
  -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc-11 \
  -DCMAKE_Fortran_COMPILER_AR:FILEPATH=/usr/bin/gcc-ar-11 \
  -DCMAKE_Fortran_COMPILER_RANLIB:FILEPATH=/usr/bin/gcc-ranlib-11 \
  ../

ninja

ninja package
