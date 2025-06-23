import os
import sys

def scan_local_tar_gz(output_file):
    tar_gz_files = []
    
    # 遍历当前目录及子目录
    for root, _, files in os.walk('.'):
        for file in files:
            if file.endswith('.tar.gz'):
                file_path = os.path.join(root, file)
                # 获取文件大小（字节）
                size = os.path.getsize(file_path)
                # 记录相对路径和大小
                tar_gz_files.append((file_path, size))
    
    # 写入输出文件
    with open(output_file, 'w') as f:
        for path, size in tar_gz_files:
            # 使用绝对路径确保唯一性
            abs_path = os.path.abspath(path)
            f.write(f"{abs_path}|{size}\n")
    
    print(f"找到 {len(tar_gz_files)} 个tar.gz文件，列表已保存至 {output_file}")

if __name__ == '__main__':
    output_file = sys.argv[1] if len(sys.argv) > 1 else 'local_files.txt'
    scan_local_tar_gz(output_file)