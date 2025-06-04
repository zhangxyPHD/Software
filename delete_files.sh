#!/bin/bash
# 文件名：delete_tar_gz_files.sh
# 功能：根据 tar_gz_files.txt 中的记录删除文件

# 定义输入文件名
INPUT_FILE="tar_gz_files.txt"

# 检查输入文件是否存在
if [ ! -f "$INPUT_FILE" ]; then
  echo "错误：文件 $INPUT_FILE 不存在！"
  exit 1
fi

# 逐行读取并删除文件
while IFS= read -r file; do
  if [ -f "$file" ]; then
    rm -v "$file"  # -v 参数显示被删除的文件
  else
    echo "警告：文件 $file 不存在，跳过"
  fi
done < "$INPUT_FILE"

echo "操作完成"