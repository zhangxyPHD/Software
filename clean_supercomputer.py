import os
import sys

def delete_matching_files(list_file):
    if not os.path.exists(list_file):
        print(f"错误：文件 {list_file} 不存在")
        return

    # 读取本地文件列表
    local_files = {}
    with open(list_file, 'r') as f:
        for line in f:
            path, size_str = line.strip().split('|', 1)
            local_files[os.path.basename(path)] = int(size_str)  # 使用文件名作为键
    
    # 遍历超算当前目录
    deleted_count = 0
    for file in os.listdir('.'):
        if file.endswith('.tar.gz') and file in local_files:
            try:
                super_size = os.path.getsize(file)
                local_size = local_files[file]
                
                if super_size == local_size:
                    os.remove(file)
                    print(f"已删除: {file} (大小: {local_size} 字节)")
                    deleted_count += 1
            except Exception as e:
                print(f"处理 {file} 时出错: {str(e)}")
    
    print(f"清理完成！共删除 {deleted_count} 个文件")

if __name__ == '__main__':
    input_file = sys.argv[1] if len(sys.argv) > 1 else 'local_files.txt'
    delete_matching_files(input_file)