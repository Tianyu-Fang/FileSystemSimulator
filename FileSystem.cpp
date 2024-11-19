#include "FileSystem.h"
#include <iostream>
#include <sstream>

// Print the help message
void FileSystem::print_help() {
    std::cout << "Available commands:\n";
    std::cout << "\tls - list directory\n";
    std::cout << "\tcd - change directory\n";
    std::cout << "\tpwd - print working directory\n";
    std::cout << "\tmkdir - make directory\n";
    std::cout << "\trmdir - remove directory\n";
    std::cout << "\ttouch - create file\n";
    std::cout << "\trm - remove file\n";
    std::cout << "\tchmod - change file permission\n";
}

// Recursive function to print tree structure
void FileSystem::print_tree(Inode* root, std::string prev) {
    if (!root) return;
    std::string next, branch, branch_next;
    branch = root->link ? "|" : "`";
    branch_next = root->link ? "|" : " ";
    next = prev + (prev.empty() ? "" : "   ") + branch_next;

    std::cout << prev + branch + "-- " << root->name;
    if (root->type == 'd') std::cout << "/";
    std::cout << std::endl;
    print_tree(root->child, next);
    print_tree(root->link, prev);
}

// List contents of a directory
void FileSystem::print_ls(Inode* pwd) {
    if (!pwd) return;
    std::cout << pwd->type << pwd->get_perm() << " " << pwd->mdate << " " << pwd->name;
    if (pwd->type == 'd') std::cout << "/";
    std::cout << std::endl;
    print_ls(pwd->link);
}

// Display detailed information about a file or directory
void FileSystem::print_stat(Inode* root, Inode* pwd, std::string path) {
    Inode* node = find_node(root, pwd, path);
    if (!node) return;
    std::cout << "File: " << node->name << (node->type == 'd' ? "/" : "") << "\n";
    std::cout << "Access: " << node->type << node->get_perm() << "\n";
    std::cout << "Modified: " << node->mdate << "\n";
    std::cout << "Created: " << node->cdate << "\n";
}

// Get current directory path as a string
std::string FileSystem::pwd_str(Inode* root, Inode* pwd) {
    std::string res;
    if (pwd == root) return "/";
    while (pwd != root) {
        res = "/" + pwd->name + res;
        pwd = pwd->parent;
    }
    return res;
}

void find_names_helper(Inode* root, Inode* current, const std::string& name, std::list<std::string>& results, FileSystem* fs) {
    if (!current) return;
    if (current->name == name) {
        results.push_back(fs->pwd_str(root, current));  // Store the path of the matching node
    }
    find_names_helper(root, current->child, name, results, fs);
    find_names_helper(root, current->link, name, results, fs);
}

// Finds all paths matching a given name
std::list<std::string> FileSystem::find_names(Inode* root, Inode* pwd, std::string name) {
    std::list<std::string> results;
    find_names_helper(root, pwd, name, results, this);
    return results;
}

// Split a string by delimiter into a list of substrings
std::list<std::string> FileSystem::split(std::string str, char delim) {
    std::list<std::string> res;
    std::stringstream ss(str);
    std::string temp;
    while (getline(ss, temp, delim)) {
        if (!temp.empty()) res.push_back(temp);
    }
    return res;
}

// Join a list of strings with a delimiter
std::string FileSystem::join(std::list<std::string> str, char delim) {
    std::string res;
    for (auto it = str.begin(); it != str.end(); ++it) {
        res += *it;
        if (std::next(it) != str.end()) res += delim;
    }
    return res;
}

// Helper function to split a path into parent path and name
std::pair<std::string, std::string> FileSystem::split_name(std::string str) {
    std::list<std::string> parts = split(str, '/'); // Split the path by '/'
    if (parts.empty()) {
        // Handle the case where the path is empty.
        return {"", ""};
    }

    std::string name = parts.back(); // Extract the last element as the name
    parts.pop_back(); // Remove the last element from parts list

    std::string parent_path = join(parts, '/'); // Join the remaining parts as the parent path
    return {parent_path, name}; // Return the parent path and name as a pair
}



// Create a new file or directory
Inode* FileSystem::create(Inode* root, Inode* pwd, std::string path, char type) {
    auto split_result = split_name(path);
    std::string parent_path = split_result.first;
    std::string name = split_result.second;

    // Ensure the parent directory exists by recursively creating it if needed
    Inode* parent_dir = cd(root, pwd, parent_path);
    if (!parent_dir) {
        if (parent_path.empty() || parent_path == "/") {
            parent_dir = root; // Root directory for absolute paths
        } else {
            // Recursively create missing directories in the path
            parent_dir = create(root, pwd, parent_path, 'd');
            if (!parent_dir) {
                std::cout << "Failed to create directory: " << parent_path << "\n";
                return nullptr;
            }
        }
    }

    // Check if the item already exists
    Inode* existingNode = find_on_pwd(parent_dir, name);
    if (existingNode) {
        std::cout << "The file or directory '" << path << "' already exists\n";
        return existingNode;
    }

    // Create the new inode for the directory or file
    Inode* newNode = new Inode(parent_dir, name);
    newNode->type = type;

    // Link it to the parent directory
    if (!parent_dir->child) {
        parent_dir->child = newNode;
    } else {
        Inode* sibling = parent_dir->child;
        while (sibling->link) sibling = sibling->link;
        sibling->link = newNode;
    }
    return newNode;
}



// Find an inode by path
Inode* FileSystem::find_node(Inode* root, Inode* pwd, std::string path) {
    auto split_result = split_name(path);
    std::string parent_path = split_result.first;
    std::string name = split_result.second;

    pwd = cd(root, pwd, parent_path); // Navigate to the parent directory
    if (!pwd) return nullptr;

    return find_on_pwd(pwd, name); // Find the specific file or directory
}


// Helper function to find an inode by name in a directory
Inode* FileSystem::find_on_pwd(Inode* pwd, std::string name) {
    Inode* curr = pwd->child;
    while (curr) {
        if (curr->name == name) return curr;
        curr = curr->link;
    }
    return nullptr;
}

// Remove a file or directory
void FileSystem::remove(Inode* root, Inode* pwd, std::string path) {
    auto split_result = split_name(path);
    std::string parent_path = split_result.first;
    std::string name = split_result.second;

    Inode* target = find_node(root, pwd, parent_path);
    if (!target) {
        std::cout << "File or directory does not exist\n";
        return;
    }

    Inode* node_to_remove = find_on_pwd(target, name);
    if (!node_to_remove) {
        std::cout << "File or directory does not exist\n";
        return;
    }

    // Unlink and delete the node
    if (node_to_remove->parent->child == node_to_remove) {
        node_to_remove->parent->child = node_to_remove->link;
    } else {
        Inode* sibling = node_to_remove->parent->child;
        while (sibling->link != node_to_remove) sibling = sibling->link;
        sibling->link = node_to_remove->link;
    }

    delete node_to_remove;
}

// Initialize a sample file system tree
void FileSystem::linux_tree(Inode* root) {
    std::vector<std::string> dirs = {"bin", "etc", "home", "home/user", "tmp"};
    std::vector<std::string> files = {"bin/bash", "bin/ls", "home/user/file.txt"};

    for (auto& dir : dirs) create(root, root, dir, 'd');
    for (auto& file : files) create(root, root, file, '-');
}

// Change file permissions
void FileSystem::chmod(Inode* root, Inode* pwd, std::string path, std::string new_modes) {
    Inode* target = find_node(root, pwd, path);
    if (!target) {
        std::cout << "File or directory does not exist\n";
        return;
    }

    int new_perm = std::stoi(new_modes);
    if (new_perm >= 0 && new_perm <= 7) {
        target->perm = new_perm;
    } else {
        std::cout << "Invalid permission code\n";
    }
}

Inode* FileSystem::cd(Inode* root, Inode* pwd, std::string path) {
    if (path == "/" || path.empty()) {
        return root; // Set the current directory to root
    }

    // Split the path into components
    std::list<std::string> path_parts = split(path, '/');
    Inode* current = (path[0] == '/') ? root : pwd; // Start at root if path is absolute

    for (const std::string& part : path_parts) {
        if (part == "." || part.empty()) {
            // Current directory (".") or empty part - no change needed
            continue;
        } else if (part == "..") {
            // Parent directory ("..")
            if (current->parent) {
                current = current->parent;
            }
        } else {
            // Navigate to the child with the matching name
            Inode* next = find_on_pwd(current, part);
            if (!next || next->type != 'd') {
                // If the directory does not exist or is not a directory, return nullptr
                std::cout << "cd: " << path << ": No such directory\n";
                return nullptr;
            }
            current = next;
        }
    }

    return current;
}