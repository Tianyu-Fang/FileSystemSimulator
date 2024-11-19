cmake_minimum_required(VERSION 3.29)
project(FileSystemSimulator)

set(CMAKE_CXX_STANDARD 11)

add_executable(FileSystemSimulator main.cpp
        Inode.cpp
        Inode.h
        FileSystem.cpp
        FileSystem.h)
