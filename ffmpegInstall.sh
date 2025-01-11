#!/usr/bin/env bash

wget https://johnvansickle.com/ffmpeg/release-source/ffmpeg-4.1.tar.xz

xz -d ffmpeg-4.1.tar.xz
tar -xf ffmpeg-4.1.tar
cd ffmpeg-4.1/
./configure --enable-shared --prefix=$HOME/ffmpeg/
make
make install
echo "export PATH=\"\$PATH:$HOME/ffmpeg/bin\"" >> "$HOME/.bashrc"
echo "export LD_LIBRARY_PATH=\"\$LD_LIBRARY_PATH:$HOME/ffmpeg/lib\"" >> "$HOME/.bashrc"

git clone https://code.videolan.org/videolan/x264.git
cd x264
./configure --enable-shared --prefix=$HOME/x264/
make
make install

echo "export LD_LIBRARY_PATH=\"\$LD_LIBRARY_PATH:$HOME/x264/lib\"" >> "$HOME/.bashrc"
echo "export PKG_CONFIG_PATH=\"\$PKG_CONFIG_PATH:$HOME/x264/lib/pkgconfig\"" >> "$HOME/.bashrc"