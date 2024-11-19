#ifndef INODE_H
#define INODE_H

#include <string>
#include <list>
#include <ctime>
#include <iostream>

std::string time_now();

class Inode {
public:
    std::string name;
    std::list<std::string> content;
    char type;
    std::string cdate;
    std::string mdate;
    int perm;
    Inode* link;
    Inode* parent;
    Inode* child;

    Inode(Inode* pwd, std::string n);

    std::string get_perm() const;
};

#endif // INODE_H
