# Install Ubuntu in Windows
## Tutorial
https://learn.microsoft.com/zh-cn/windows/wsl/install-manual
## Some command
```
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi
wsl --set-default-version 2
```
# Transfer WSL
# Tutorial
https://www.cnblogs.com/ajianbeyourself/p/17985496

## Some command

```
wsl --help
wsl --shutdown
wsl --export Ubuntu-22.04 D:\wsl-Ubuntu-22.04.tar
wsl --import Ubuntu-22.04 D:\wsl\Ubuntu2204 D:\wsl-Ubuntu-22.04.tar --version 2
wsl -d Ubuntu-22.04 -u <zxy>
```

