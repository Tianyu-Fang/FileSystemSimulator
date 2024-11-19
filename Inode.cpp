#include "Inode.h"

// Returns the current time as a formatted string.
std::string time_now() {
    std::string res;
    time_t now = time(0);
    tm *ltm = localtime(&now);
    res += std::to_string(ltm->tm_mday) + " ";
    std::string months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    res += months[ltm->tm_mon] + " ";
    res += (ltm->tm_hour < 10 ? "0" : "") + std::to_string(ltm->tm_hour) + ":";
    res += (ltm->tm_min < 10 ? "0" : "") + std::to_string(ltm->tm_min);
    return res;
}

// Constructor for Inode class.
Inode::Inode(Inode* pwd, std::string n) : name(n), type('-'), perm(6), parent(pwd), link(nullptr), child(nullptr) {
    cdate = mdate = time_now();
}

// Returns permission as a string.
std::string Inode::get_perm() const {
    std::string res;
    if (perm == 0) res = "---";
    else if (perm == 1) res = "--x";
    else if (perm == 2) res = "-w-";
    else if (perm == 3) res = "-wx";
    else if (perm == 4) res = "r--";
    else if (perm == 5) res = "r-x";
    else if (perm == 6) res = "rw-";
    else if (perm == 7) res = "rwx";
    return res;
}
