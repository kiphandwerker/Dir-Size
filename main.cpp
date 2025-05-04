#include <iostream>
#include <filesystem>
#include <string>
#include <map>
#include <iomanip>

namespace fs = std::filesystem;
using std::cout;

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

// Recursively calculate folder sizes with depth control
uintmax_t calculate_folder_sizes(const fs::path& path, int current_depth, int max_depth) {
    uintmax_t total_size = 0;

    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
                try {
                    if (fs::is_regular_file(entry)) {
                        total_size += fs::file_size(entry);
                    } else if (fs::is_directory(entry) && (max_depth == -1 || current_depth < max_depth)) {
                        uintmax_t subfolder_size = calculate_folder_sizes(entry.path(), current_depth + 1, max_depth);
                        total_size += subfolder_size;
                        std::cout << std::string(current_depth * 2, ' ') // indent
                                  << entry.path().string() << " -> " << format_size(subfolder_size) << '\n';
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error accessing entry: " << e.what() << '\n';
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing directory: " << e.what() << '\n';
    }

    return total_size;
}

int main() {
    std::string root_path;
    int max_depth;

    std::cout << "Enter the directory path to scan: ";
    std::getline(std::cin, root_path);

    std::cout << "Enter max depth (-1 for unlimited): ";
    std::cin >> max_depth;

    if (!fs::exists(root_path) || !fs::is_directory(root_path)) {
        std::cerr << "Invalid directory path.\n";
        return 1;
    }

    std::cout << "\nScanning folder sizes...\n\n";
    uintmax_t root_size = calculate_folder_sizes(fs::path(root_path), 0, max_depth);

    std::cout << "\nTotal size of " << root_path << ": " << format_size(root_size) << '\n';

    return 0;
}
