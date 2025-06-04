# information of the HPC
```
scontrol show node
```

# ffmpeg

[ffmpegInstall.sh](ffmpegInstall.sh)

# Some useful conmmand
```
alias li='squeue -u xzhang --format "%.18i %.50j %.8u %.2t %.10M %.6D %R"'
```

# 找到当前文件夹及其子文件夹中的所有.tar.gz文件并解压它们
```
find . -name "*.tar.gz" -exec tar -xzvf {} \;

```
查找文件：find . -name "*.tar.gz"

find .：从当前目录开始递归搜索。

-name "*.tar.gz"：匹配所有以.tar.gz结尾的文件名。

解压文件：-exec tar -xzvf {} \;

-exec：对每个找到的文件执行后续命令。

tar -xzvf：解压选项组合：

-x：解压文件。

-z：通过gzip解压缩。

-v：显示解压过程（可选，可省略-v以静默解压）。

-f：指定文件名。

{}：被找到的文件名占位符。

\;：表示-exec命令结束。

# clear thread
```
//列出所有进程
ps -u $USER
//杀掉进程
pkill -9 -u $USER
```


# find files and delete
[find file](find_files.sh)
[delete file](delete_files.sh)