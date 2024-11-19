#include "Inode.h"
#include "FileSystem.h"
#include <iostream>
#include <string>
#include <list>

// Entry point of the program
int main() {
    FileSystem fs;
    Inode* root = new Inode(nullptr, "");  // Create the root directory
    root->type = 'd';  // 'd' signifies a directory
    Inode* pwd = root; // Pointer to the current working directory

    fs.linux_tree(root);  // Initialize a basic directory structure
    fs.print_help();      // Display help information

    std::string cmd;
    std::cout << "\n" << fs.pwd_str(root, pwd) << ">> ";  // Display prompt

    // Command loop
    while (std::getline(std::cin, cmd)) {
        std::list<std::string> args = fs.split(cmd, ' ');
        if (args.empty()) {
            std::cout << "\n" << fs.pwd_str(root, pwd) << ">> ";
            continue;
        }

        std::string command = args.front();
        args.pop_front();

        if (command == "help") {
            fs.print_help();
        }
        else if (command == "ls") {
            if (args.empty()) {
                fs.print_ls(pwd->child);
            } else {
                for (const auto& arg : args) {
                    Inode* target = fs.find_node(root, pwd, arg);
                    if (target) {
                        std::cout << arg << ":\n";
                        fs.print_ls(target->child);
                    } else {
                        std::cout << "ls: cannot access '" << arg << "': No such file or directory\n";
                    }
                }
            }
        }
        else if (command == "pwd") {
            std::cout << fs.pwd_str(root, pwd) << std::endl;
        }
        else if (command == "cd") {
            if (args.empty()) {
                pwd = root; // Change to root if no argument is provided
            } else {
                Inode* target = fs.find_node(root, pwd, args.front());
                if (target && target->type == 'd') {
                    pwd = target;
                } else {
                    std::cout << "cd: " << args.front() << ": No such directory\n";
                }
            }
        }
        else if (command == "mkdir") {
            if (args.empty()) {
                std::cout << "mkdir: missing operand\n";
            } else {
                for (const auto& dir : args) {
                    fs.create(root, pwd, dir, 'd');
                }
            }
        }
        else if (command == "touch") {
            if (args.empty()) {
                std::cout << "touch: missing operand\n";
            } else {
                for (const auto& file : args) {
                    fs.create(root, pwd, file, '-');
                }
            }
        }
        else if (command == "rm") {
            if (args.empty()) {
                std::cout << "rm: missing operand\n";
            } else {
                for (const auto& target : args) {
                    fs.remove(root, pwd, target);
                }
            }
        }
        else if (command == "stat") {
            if (args.empty()) {
                std::cout << "stat: missing operand\n";
            } else {
                for (const auto& target : args) {
                    fs.print_stat(root, pwd, target);
                }
            }
        }
        else if (command == "chmod") {
            if (args.size() < 2) {
                std::cout << "chmod: missing operand\n";
            } else {
                std::string perm = args.front();
                args.pop_front();
                fs.chmod(root, pwd, args.front(), perm);
            }
        }
        else if (command == "tree") {
            if (args.empty()) {
                fs.print_tree(pwd->child);
            } else {
                for (const auto& dir : args) {
                    Inode* target = fs.find_node(root, pwd, dir);
                    if (target && target->type == 'd') {
                        std::cout << dir << ":\n";
                        fs.print_tree(target->child);
                    } else {
                        std::cout << "tree: " << dir << ": No such directory\n";
                    }
                }
            }
        }
        else if (command == "find") {
            if (args.empty()) {
                std::cout << "find: missing operand\n";
            } else {
                for (const auto& name : args) {
                    std::list<std::string> matches = fs.find_names(root, pwd, name);
                    if (matches.empty()) {
                        std::cout << "find: '" << name << "': No such file or directory\n";
                    } else {
                        for (const auto& match : matches) {
                            std::cout << match << std::endl;
                        }
                    }
                }
            }
        }
        else if (command == "exit") {
            break;  // Exit the program
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }

        std::cout << "\n" << fs.pwd_str(root, pwd) << ">> ";  // Show prompt again
    }

    std::cout << "Exiting...\n";
    return 0;
}
