#!/bin/bash

# Update and install OpenMPI
echo "Installing OpenMPI..."
sudo apt update
sudo apt install -y openmpi-bin openmpi-common libopenmpi-dev

# Install GCC and G++
echo "Installing GCC and G++..."
sudo apt install -y gcc g++

# Install Basilisk dependencies
echo "Installing Basilisk dependencies..."
sudo apt install -y darcs make gawk
sudo apt install -y gnuplot imagemagick ffmpeg graphviz valgrind gifsicle pstoedit

# Download and install Basilisk
echo "Downloading and installing Basilisk..."
wget http://basilisk.fr/basilisk/basilisk.tar.gz
tar xzf basilisk.tar.gz
cd basilisk/src || exit
ln -s config.gcc config
make

# Set environment variables
echo "Setting up environment variables..."
echo "export BASILISK=$PWD" >> ~/.bashrc
echo 'export PATH=$PATH:$BASILISK' >> ~/.bashrc

# Apply changes to current session
source ~/.bashrc

echo "Installation complete. Basilisk is ready to use."

# for latex

sudo apt install -y texlive-full

