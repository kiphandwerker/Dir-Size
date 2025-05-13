# 📁 Folder Size Analyzer (C++)

A command-line utility to recursively scan directories, calculate folder sizes, and display the results in a neatly formatted table. Useful for identifying large folders and cleaning up disk space.

## 🔧 Features

- Computes **total size** of a root directory (including all subfolders).
- Recursively calculates **subfolder sizes**.
- **Display depth** with a user-defined value.
- **Sorts folders by size** within each parent directory.
- Prints output as a **formatted table**, aligned by folder name length.

## 🖥️ Example Output

```bash
Enter the directory path to scan: /Users/you/Documents
Enter how many levels deep to show (e.g., 2): 2

Folder Size Report (sorted by size within folders):

Depth | Folder Path                           | Size
----------------------------------------------------
0     | /Users/you/Documents                  | 5.82 GB
1     |   /Users/you/Documents/Work           | 3.11 GB
2     |     /Users/you/Documents/Work/Reports | 1.45 GB
1     |   /Users/you/Documents/Photos         | 2.27 GB

```

## 🚀 Usage

```bash
./main.exe
```

You will be prompted to enter:
- The root directory path to scan.
- How many levels to display

## ⚙️ Options
- Max depth: Always goes to end of path.
- Sorting: Folders are sorted by size within each parent.
- Cross-platform: Works on Linux, macOS, and Windows with appropriate filesystem permissions.

## 🔒 Notes
- Files and folders with restricted permissions are skipped.
- Symlinks are not followed (to avoid infinite recursion).