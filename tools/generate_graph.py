import os
import sys
import json


def parse_json_tasks(path: str) -> map:
    content = ""
    with open(path, 'r') as f:
        content = f.read()
    tasks = {}
    try:
        tasks = json.loads(content)
    except:
        raise Exception("wrong format of json, check again")
    return tasks


def generate_code(tasks: map) -> str:
    template = """```mermaid
graph LR
   {}
```"""
    body = ""
    template2 = "{}(({})) --> {}(({}))\n"
    for each in tasks["tasks"]:
        for dep in each["dependencies"]:
            body += template2.format(dep, dep,
                                     each["task_name"], each["task_name"])
    return template.format(body)


if __name__ == "__main__":
    path = ""
    if len(sys.argv) >= 2:
        path = sys.argv[1]
    else:
        path = input("输入json的绝对地址:")
    if not os.path.exists(path):
        raise Exception("json文件不存在!")
    tasks = parse_json_tasks(path)
    print(generate_code(tasks))
