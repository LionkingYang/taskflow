import os
import sys

if __name__ == "__main__":
    project_path = ""
    js_path = ""
    if len(sys.argv) >= 3:
        project_path = sys.argv[1]
        js_path = sys.argv[2]
    else:
        raise Exception("在命令行输入项目地址和json地址")

    if not os.path.exists(js_path):
        raise Exception("json文件不存在!")

    os.system("/usr/bin/python3 ./check_json_file.py {}".format(js_path))

    os.system("/usr/bin/python3 ./generate_op.py {} {}".format(project_path +
              "/project_op.h", js_path))

    os.system("/usr/bin/python3 ./generate_main.py {} {}".format(project_path +
              "/project_main.cpp", js_path))

    os.system("/usr/bin/python3 ./check_ops.py {} {}".format(project_path +
              "/project_op.h", js_path))
