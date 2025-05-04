#include <iostream>
#include <filesystem>
#include <string>
#include <map>
#include <iomanip>

namespace fs = std::filesystem;

// Helper to format byte sizes
std::string format_size(uintmax_t size) {
    const char* sizes[] = { "B", "KB", "MB", "GB", "TB" };
    int order = 0;
    double readable_size = static_cast<double>(size);
    while (readable_size >= 1024 && order < 4) {
        order++;
        readable_size /= 1024;
    }
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << readable_size << " " << sizes[order];
    return out.str();
}

// Recursively calculate size of any directory
uintmax_t get_directory_size(const fs::path& path) {
    uintmax_t total_size = 0;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry)) {
                total_size += fs::file_size(entry);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing " << path << ": " << e.what() << '\n';
    }
    return total_size;
}

// Print subfolders and their sizes up to max_depth
void print_subfolder_sizes(const fs::path& path, int current_depth, int max_depth) {
    try {
        for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (fs::is_directory(entry)) {
                uintmax_t subfolder_size = get_directory_size(entry.path());
                std::cout << std::string(current_depth * 2, ' ')
                          << entry.path().string() << " -> " << format_size(subfolder_size) << '\n';

                if (max_depth == -1 || current_depth < max_depth) {
                    print_subfolder_sizes(entry.path(), current_depth + 1, max_depth);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing directory: " << e.what() << '\n';
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
        std::cerr << "Invalid directory path.\n";
        return 1;
    }

    // Calculate total size first
    uintmax_t total_size = get_directory_size(root);
    std::cout << "\nTotal size of " << root_path << ": " << format_size(total_size) << "\n\n";

    // Then list subfolder sizes
    std::cout << "Subfolder sizes:\n";
    print_subfolder_sizes(root, 1, max_depth);

    return 0;
}
