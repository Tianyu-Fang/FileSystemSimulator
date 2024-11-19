#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Inode.h"
#include <list>
#include <string>
#include <vector>

class FileSystem {
public:
    void linux_tree(Inode* root);
    void print_help();
    void print_tree(Inode* root, std::string prev = "");
    void print_ls(Inode* pwd);
    void print_stat(Inode* root, Inode* pwd, std::string path);
    std::string pwd_str(Inode* root, Inode* pwd);
    std::list<std::string> find_names(Inode* root, Inode* pwd, std::string name);
    Inode* find_node(Inode* root, Inode* pwd, std::string path);
    Inode* create(Inode* root, Inode* pwd, std::string path, char type);
    void remove(Inode* root, Inode* pwd, std::string path);
    void chmod(Inode* root, Inode* pwd, std::string path, std::string new_modes);
    std::list<std::string> split(std::string str, char delim);
    Inode* cd(Inode* root, Inode* pwd, std::string path);



private:
    std::string join(std::list<std::string> str, char delim);
    // std::string* split_name(std::string str);
    std::pair<std::string, std::string> split_name(std::string str);
    Inode* find_on_pwd(Inode* pwd, std::string name);
};

#endif // FILESYSTEM_H
