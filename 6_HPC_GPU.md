# Install conda
```
1. 下载安装脚本
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh

2. 执行安装 (一路回车即可，建议安装在自己的 home 目录下)
bash Miniconda3-latest-Linux-x86_64.sh

3. 初始化 (这一步很重要，否则无法使用 conda 命令)
source ~/.bashrc
conda init
conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main
conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r
```

# Install torch enviroment
```
1. 创建名为 siren_env 的虚拟环境，指定 Python 3.9
conda create -n torch python=3.9 -y

2. 激活环境
conda activate torch

3. 安装 PyTorch (请根据超算支持的 CUDA 版本调整，这里以 CUDA 12.1 为例)
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu121

4. 安装其他依赖
pip install numpy pandas matplotlib scikit-learn
```

# Bash script

```
#!/bin/bash
#SBATCH --job-name=SIREN_DeepSDF
#SBATCH --partition=gpu_v100s       # 使用指定的 V100 队列
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24          # 匹配你代码中的 NUM_WORKERS (建议略小于或等于此值)
#SBATCH --gres=gpu:1                # 申请 1 张 V100 显卡
#SBATCH --mem=128G                   # 内存申请
#SBATCH --time=1-00:00:00           # 运行时间限制 (1天)
#SBATCH --output=siren_%j.out
#SBATCH --error=siren_%j.err

# --- 1. 清理环境并加载系统 CUDA ---
module purge
# 建议加载 12.1 或 11.6，取决于你安装 torch 时选的版本
module load cuda/12.1.0 

# --- 2. 激活 Conda 环境 ---
# 注意：在脚本中直接用 'conda activate' 有时会失效
# 推荐使用 source 方式指向 conda 安装路径下的 profile.d
source ~/miniconda3/etc/profile.d/conda.sh   # 或者 ~/miniconda3/...
conda activate torch                # <--- 修改为你的环境名

# --- 3. 环境自检 ---
echo "---------- Environment Check ----------"
echo "Date: $(date)"
echo "Node: $SLURM_NODELIST"
echo "Python: $(which python)"

python - <<EOF
import torch
print(f"PyTorch Version: {torch.__version__}")
print(f"CUDA Version in Torch: {torch.version.cuda}")
print(f"GPU Available: {torch.cuda.is_available()}")
if torch.cuda.is_available():
    print(f"GPU Device: {torch.cuda.get_device_name(0)}")
EOF

nvidia-smi

# --- 4. 运行 SIREN 程序 ---
echo "---------- Starting Training ----------"
# 确保此时你在代码目录下
cd $SLURM_SUBMIT_DIR

# 运行你的 SIREN 代码
python 2_ML_deep_sdf_SIREN.py

echo "---------- Training Finished ----------"
```

# command
```
watch -n 1 nvidia-smi
```