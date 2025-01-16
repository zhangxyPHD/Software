#!/bin/bash

module purge
module load openmpi/4/gcc/4.1.1
module load gcc/11/11.1.0

# Download and install Basilisk
echo "Downloading and installing Basilisk..."
tar xzf basilisk.tar.gz
cd basilisk/src || exit
cp config.gcc config
make

# Set environment variables
echo "Setting up environment variables..."
echo "export BASILISK=$PWD" >> ~/.bashrc
echo 'export PATH=$PATH:$BASILISK' >> ~/.bashrc

# Apply changes to current session
source ~/.bashrc

echo "Installation complete. Basilisk is ready to use."
