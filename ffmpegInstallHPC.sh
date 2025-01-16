#!/usr/bin/env bash

wget https://johnvansickle.com/ffmpeg/release-source/ffmpeg-4.1.tar.xz

xz -d ffmpeg-4.1.tar.xz
tar -xf ffmpeg-4.1.tar
cd ffmpeg-4.1/
./configure --enable-shared --prefix=$HOME/ffmpeg/ --disable-x86asm
make
make install
echo "export PATH=\"\$PATH:$HOME/ffmpeg/bin\"" >> "$HOME/.bashrc"
echo "export LD_LIBRARY_PATH=\"\$LD_LIBRARY_PATH:$HOME/ffmpeg/lib\"" >> "$HOME/.bashrc"
