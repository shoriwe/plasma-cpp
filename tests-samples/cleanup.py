import os
import os.path


def main():
    for path, directories, files in os.walk("."):
        for file in files:
            script_path = os.path.join(path, file)
            with open(script_path, "rb") as file_object:
                content = file_object.read()
            if b'\r' in content:
                print("[-] With return!")
                content = content.replace(b"\r\n", b'\n').replace(b"\n\r", b'\n')
                with open(script_path, "wb") as file_object:
                    file_object.write(content)


if __name__ == "__main__":
    main()
