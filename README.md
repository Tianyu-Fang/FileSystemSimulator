# FileSystemSimulator

Design of LLFS:
https://docs.google.com/document/d/1c_UizpalPAU8pcpU6vWieuCdHuMXlYm0XLhng7hHbIA/edit?usp=sharing
 
```
g++ -std=c++11 -o FileSystemSimulator main.cpp FileSystem.cpp Inode.cpp
./FileSystemSimulator

```
Available commands:
ls - list directory
cd - change directory
pwd - print working directory
mkdir - make directory
rmdir - remove directory
touch - create file
rm - remove file
chmod - change file permission
