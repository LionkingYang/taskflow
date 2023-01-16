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
    task_map = {}
    for each in tasks["tasks"]:
        task_map[each["task_name"]] = each
    template = """
graph LR
{}
"""
    body = ""
    if len(tasks["tasks"]) == 0:
        return template.format("a((NO_TASK))")
    template2 = "{}(({})) --> {}(({}))\n"
    template1 = "{}(({}))\n"
    template3 = "{}{{{}}} --> {}(({}))\n"
    template4 = "{}(({})) --> {}{{{}}}\n"
    i = 0
    for each in tasks["tasks"]:
        if "async" in each and each["async"]:
            body += "style {} rhombus stroke-width:2px,stroke-dasharray: 5, 5\n".format(
                each["task_name"])
        if len(each["dependencies"]) > 0:
            for dep in each["dependencies"]:
                if "condition" not in each or len(each["condition"]) == 0:
                    body += template2.format(dep, dep+":"+task_map[dep]["op_name"],
                                             each["task_name"], each["task_name"]+":"+each["op_name"])
                else:
                    body += "{}{{rhombus }}\n".format("condition{}".format(i))
                    body += template4.format(dep, dep+":"+task_map[dep]["op_name"],
                                             "condition{}".format(i), each["condition"].split("|")[:-1][0])
                    body += template3.format("condition{}".format(i), each["condition"].split("|")[:-1][0],
                                             each["task_name"], each["task_name"]+":"+each["op_name"])
                    i += 1
        else:
            body += template1.format(each["task_name"],
                                     each["task_name"]+":"+each["op_name"])
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
