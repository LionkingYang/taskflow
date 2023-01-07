import json
import sys
import os
import re


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


def build_depedency_map(tasks: map) -> map:
    dep_map = {}
    for task in tasks["tasks"]:
        t = {}
        t["depedencies"] = task["dependencies"]
        if "type" in task:
            t["type"] = task["type"]
        if "op_name" in task:
            t["op_name"] = task["op_name"]
        else:
            raise Exception("task {} has no op?".format(task["task_name"]))
        if "dependencies" in task:
            t["dependencies"] = task["dependencies"]
        dep_map[task["task_name"]] = t
    return dep_map


def parse_op(op: str) -> list:
    op_name = re.findall(r"BEGIN_OP\(([^(]*)\)", op)
    if len(op_name) == 0:
        raise Exception("op name dosen't found")
    op_name = op_name[0]
    op_read_input = re.findall(r"GET_INPUT\(([^(]*)\)", op)
    op_read_input_mutable = re.findall(
        r"GET_MUTABLE_INPUT\(([^(]*)\)", op)
    op_read_input.extend(op_read_input_mutable)
    input_s = 0
    if "GET_INPUT_TO_VEC" in op:
        input_s = -1
    else:
        input_s = len(op_read_input)
    for each in op_read_input:
        params = each.split(",")
        if (len(params) != 3):
            raise Exception(
                "{} has wrong number of read op input params".format(op_name))
    output = re.findall(r"RETURN_VAL\((.*)\)", op)
    if len(output) == 0:
        if "RETURN_VAL" not in op:
            raise Exception("{} has no output?".format(op_name))
    elif len(output) > 1:
        raise Exception("{} has more than one output?".format(op_name))
    return [op_name, input_s]


def parse_ops(s: str):
    output = re.findall(r"WRITE_TO_FINAL_OUTPUT\((.*)\)", s)
    if len(output) > 1:
        raise Exception("more than one final output?")
    ops = []
    tmp = ""
    begin = False
    lines = s.split("\n")
    for line in lines:
        # print(line)
        line = line.strip()
        if not (len(line) > 2 and line[:2] != "//" and line[:2] != "/*" and line[0] != "*"):
            continue
        if "BEGIN_OP" in line:
            tmp = ""
            tmp += line
            begin = True
            continue
        if "END_OP" in line:
            tmp += line
            ops .append(tmp)
            begin = False
        if begin:
            tmp += line
            continue
    return ops


def check_if_legal(op_map, dep_map):
    for each in dep_map:
        task_op = dep_map[each]["op_name"]
        if task_op not in op_map:
            raise Exception("json中{}任务定义的{}算子没有实现".format(
                each, task_op))
        dep_size = len(dep_map[each]["dependencies"])
        if dep_size < op_map[task_op]:
            raise Exception("json中{}任务定义的{}算子使用input数量存在越界".format(
                each, task_op))


if __name__ == "__main__":
    path = ""
    if len(sys.argv) >= 3:
        op_path = sys.argv[1]
        js_path = sys.argv[2]
    else:
        raise Exception("在命令行输入算子文件地址和json地址")
    if not os.path.exists(op_path):
        raise Exception("算子文件不存在!")
    if not os.path.exists(js_path):
        raise Exception("json文件不存在!")

    dep_map = build_depedency_map(parse_json_tasks(js_path))
    ops = ""
    with open(op_path, 'r') as f:
        ops = f.read()
    ops = parse_ops(ops)
    op_deps = {}
    for each in ops:
        op, size = parse_op(each)
        op_deps[op] = size
    check_if_legal(op_deps, dep_map)
    print("合法的算子构造")
