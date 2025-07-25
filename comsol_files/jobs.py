import subprocess
import os
import time

# 参数设置
L_rise = 0.01
Bos = ["2.5e-4", "5e-4", "1e-3", "2.5e-3"]
Gas = ["40"]
refine_num = 10
t_step = 0.05
t_max = 200
cores = 12
NPARA = 2  # 最大并行任务数

# 生成任务列表（过滤已存在结果的任务）
tasks = []
for Bo in Bos:
    for Ga in Gas:
        filename = f"L_rise{L_rise}_Ga{Ga}_Bo{Bo}_Level{refine_num}"
        results_file = f"results_{filename}.csv"
        if os.path.exists(results_file):
            print(f"Results file {results_file} already exists. Skipping.")
            continue
        tasks.append((Bo, Ga, filename))

active_processes = []
current_task_index = 0

# 主循环处理任务
while current_task_index < len(tasks) or active_processes:
    # 移除已完成的进程
    active_processes = [p for p in active_processes if p.poll() is None]
    
    # 启动新任务（如果有可用槽位）
    while len(active_processes) < NPARA and current_task_index < len(tasks):
        Bo, Ga, filename = tasks[current_task_index]
        current_task_index += 1
        
        # 构建命令
        command = [
            'C:/Users/24620/anaconda3/python.exe', 'run.py',
            '--Bo', Bo,
            '--Ga', Ga,
            '--Lrise', str(L_rise),
            '--refine_num', str(refine_num),
            '--t_step', str(t_step),
            '--t_max', str(t_max),
            '--cores', str(cores),
            '--name', filename
        ]
        
        # 启动子进程
        proc = subprocess.Popen(command)
        active_processes.append(proc)
        print(f"{filename} starts.")
        time.sleep(10)  # 每个任务启动后等待10秒
    
    # 检查间隔
    time.sleep(1)

# 等待剩余进程完成
for proc in active_processes:
    proc.wait()

print("All tasks have completed.")