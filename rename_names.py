import os
import os.path
import re

match_c_file = re.compile(r"plasma\\(src|include)\\.*[\w_-]+\.(c|cpp|h)$", re.MULTILINE)
find_target_names = re.compile(r".*\w+\s[A-Z][A-Za-z0-9_]+\w+[;{]")


def rename_symbols(file_path: str) -> None:
    with open(file_path, "r") as file_object:
        contents = file_object.read()
    names_to_change = find_target_names.findall(contents)
    for name in names_to_change:
        print(names_to_change)


def main():
    for path, directories, files in os.walk(os.getcwd()):
        for file in files:
            full_file_path = os.path.join(path, file)
            if match_c_file.search(full_file_path):
                rename_symbols(full_file_path)


if __name__ == '__main__':
    main()
