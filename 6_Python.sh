#!/bin/bash
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
chmod +x Miniconda3-latest-Linux-x86_64.sh
./Miniconda3-latest-Linux-x86_64.sh -b -p ~/miniconda3
echo 'export PATH="$HOME/miniconda3/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
# init

# 创建名为py39的Python 3.9环境
conda create -n py39 python=3.9 -y
conda init bash
echo 'conda activate py39' >> ~/.bashrc
source ~/.bashrc
conda activate py39
# 安装所需的包
conda install numpy pandas matplotlib -y

# 验证安装
python -c "import numpy, pandas, matplotlib; print('所有包安装成功！')"