#include <iostream>
#include <filesystem>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

namespace fs = std::filesystem;

// Keep this unchanged
struct FolderInfo {
    int depth;
    std::string path;
    uintmax_t size;
};

// No change to this function
uintmax_t get_directory_size(const fs::path& path) {
    uintmax_t size = 0;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry)) {
                size += fs::file_size(entry);
            }
        }
    } catch (...) {}
    return size;
}

std::string format_size(uintmax_t size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double display_size = static_cast<double>(size);
    while (display_size >= 1024 && unit_index < 4) {
        display_size /= 1024.0;
        ++unit_index;
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << display_size << ' ' << units[unit_index];
    return out.str();
}

// Recursively collect all folders (no max depth)
void collect_all_folders(const fs::path& path, int depth, std::vector<FolderInfo>& result) {
    std::vector<FolderInfo> children;

    try {
        for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (fs::is_directory(entry)) {
                uintmax_t size = get_directory_size(entry);
                children.push_back({depth, entry.path().string(), size});
            }
        }
    } catch (...) {}

    // Sort this level by size
    std::sort(children.begin(), children.end(), [](const FolderInfo& a, const FolderInfo& b) {
        return a.size > b.size;
    });

    for (const auto& folder : children) {
        result.push_back(folder);
        collect_all_folders(folder.path, folder.depth + 1, result);
    }
}

// Adjusted to filter display by max display depth
void print_table(const std::vector<FolderInfo>& info, const fs::path& root, uintmax_t root_size, int display_depth) {
    size_t max_path_length = root.string().length();
    for (const auto& entry : info) {
        size_t full_length = std::string(entry.depth * 2, ' ').length() + entry.path.length();
        if (full_length > max_path_length)
            max_path_length = full_length;
    }
    max_path_length = std::min(max_path_length, static_cast<size_t>(120));

    std::cout << std::left << std::setw(7) << "Depth"
              << "| " << std::left << std::setw(max_path_length) << "Folder Path"
              << "| " << std::right << std::setw(10) << "Size" << '\n';
    std::cout << std::string(10 + max_path_length + 13, '-') << '\n';

    std::cout << std::left << std::setw(7) << 0
              << "| " << std::left << std::setw(max_path_length) << root.string()
              << "| " << std::right << std::setw(10) << format_size(root_size) << '\n';

    for (const auto& entry : info) {
        if (entry.depth <= display_depth) {
            std::string indented_path = std::string(entry.depth * 2, ' ') + entry.path;
            std::cout << std::left << std::setw(7) << entry.depth
                      << "| " << std::left << std::setw(max_path_length) << indented_path
                      << "| " << std::right << std::setw(10) << format_size(entry.size) << '\n';
        }
    }
}

// Updated main function
int main() {
    std::string root_path;
    int display_depth;

    std::cout << "Enter the directory path to scan: ";
    std::getline(std::cin, root_path);

    std::cout << "Enter how many levels deep to show (e.g., 2): ";
    std::cin >> display_depth;

    fs::path root(root_path);
    if (!fs::exists(root) || !fs::is_directory(root)) {
        std::cerr << "Invalid directory.\n";
        return 1;
    }

    uintmax_t root_size = get_directory_size(root);

    std::vector<FolderInfo> folder_data;
    collect_all_folders(root, 1, folder_data);  // Always collects all depths

    std::cout << "\nFolder Size Report (showing up to depth " << display_depth << "):\n\n";
    print_table(folder_data, root, root_size, display_depth);

    return 0;
}
