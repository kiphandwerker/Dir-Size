#include <iostream>
#include <filesystem>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>

namespace fs = std::filesystem;

struct FolderInfo {
    int depth;
    std::string path;
    uintmax_t size;
};

// Format size as human-readable string
std::string format_size(uintmax_t size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int order = 0;
    double readable = static_cast<double>(size);
    while (readable >= 1024 && order < 4) {
        order++;
        readable /= 1024;
    }
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << readable << " " << units[order];
    return out.str();
}

// Get total size of a folder
uintmax_t get_directory_size(const fs::path &path) {
    uintmax_t size = 0;
    try {
        for (const auto &entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry)) {
                size += fs::file_size(entry);
            }
        }
    } catch (...) {}
    return size;
}

// Recursively collect folder info, sorting siblings by size
void collect_sorted_folders(const fs::path &path, int depth, int max_depth, std::vector<FolderInfo> &result) {
    std::vector<FolderInfo> children;

    try {
        for (const auto &entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (fs::is_directory(entry)) {
                uintmax_t size = get_directory_size(entry);
                children.push_back({depth, entry.path().string(), size});
            }
        }
    } catch (...) {}

    // Sort descending
    std::sort(children.begin(), children.end(), [](const FolderInfo &a, const FolderInfo &b) {
        return a.size > b.size;
    });

    // Add to result and continue 
    for (const auto &folder : children) {
        result.push_back(folder);
        if (max_depth == -1 || folder.depth < max_depth) {
            collect_sorted_folders(folder.path, folder.depth + 1, max_depth, result);
        }
    }
}


void print_table(const std::vector<FolderInfo> &info, const fs::path &root, uintmax_t root_size) {
    // Largest size for better looking table
    size_t max_path_length = root.string().length();
    for (const auto &entry : info) {
        size_t full_length = std::string(entry.depth * 2, ' ').length() + entry.path.length();
        if (full_length > max_path_length)
            max_path_length = full_length;
    }
    max_path_length = std::min(max_path_length, static_cast<size_t>(120)); // optional safety limit

    // Header
    std::cout << std::left << std::setw(7) << "Depth"
              << "| " << std::left << std::setw(max_path_length) << "Folder Path"
              << "| " << std::right << std::setw(10) << "Size" << '\n';

    std::cout << std::string(10 + max_path_length + 13, '-') << '\n';

    // Footer
    std::cout << std::left << std::setw(7) << 0
              << "| " << std::left << std::setw(max_path_length) << root.string()
              << "| " << std::right << std::setw(10) << format_size(root_size) << '\n';

    // Rows
    for (const auto &entry : info) {
        std::string indented_path = std::string(entry.depth * 2, ' ') + entry.path;
        std::cout << std::left << std::setw(7) << entry.depth
                  << "| " << std::left << std::setw(max_path_length) << indented_path
                  << "| " << std::right << std::setw(10) << format_size(entry.size) << '\n';
    }
}


int main() {
    std::string root_path;
    int max_depth;

    std::cout << "Enter the directory path to scan: ";
    std::getline(std::cin, root_path);

    std::cout << "Enter max depth (-1 for unlimited): ";
    std::cin >> max_depth;

    fs::path root(root_path);
    if (!fs::exists(root) || !fs::is_directory(root)) {
        std::cerr << "Invalid directory.\n";
        return 1;
    }

    uintmax_t root_size = get_directory_size(root);

    std::vector<FolderInfo> folder_data;
    collect_sorted_folders(root, 1, max_depth, folder_data);

    std::cout << "\nFolder Size Report (sorted by size within folders):\n\n";
    print_table(folder_data, root, root_size);

    return 0;
}

