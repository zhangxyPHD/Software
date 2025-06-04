#!/bin/bash
# 文件名：save_tar_gz_list.sh
# 功能：记录当前目录下所有 .tar.gz 文件到 tar_gz_files.txt

# 定义输出文件名
OUTPUT_FILE="tar_gz_files.txt"

# 清空或创建输出文件
> "$OUTPUT_FILE"

# 查找当前目录（不含子目录）的所有 .tar.gz 文件，保存到文件
find . -maxdepth 1 -type f -name "*.tar.gz" -print >> "$OUTPUT_FILE"

echo "已记录所有 .tar.gz 文件到 $OUTPUT_FILE"