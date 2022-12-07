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
        dep_map[task["task_name"]] = t
    return dep_map


def parse_op(op: str) -> list:
    op_name = re.findall(r"BeginTask\(([^(]*)\)", op)
    if len(op_name) == 0:
        raise Exception("op name dosen't found")
    op_name = op_name[0]
    op_read_input = re.findall(r"ReadTaskOutput\(([^(]*)\)", op)
    input = []
    for each in op_read_input:
        params = each.split(",")
        if (len(params) != 3):
            raise Exception(
                "{} has wrong number of read op input params".format(op_name))
        input.append([each.split(",")[1].strip(), each.split(",")[0].strip()])
    output = re.findall(r"WriteToOutput\((.*)\)", op)
    if len(output) == 0:
        if "WriteToFinalOutput" not in op:
            raise Exception("{} has no output?".format(op_name))
    elif len(output) > 1:
        raise Exception("{} has more than one output?".format(op_name))
    else:
        out = output[0].split(",")
        if len(out) != 3:
            raise Exception(
                "{} has wrong number of write op output params".format(op_name))
        if out[0] != op_name:
            raise Exception(
                "{} output op name is not equal to task op name".format(op_name))
        return [op_name, input]
    return [op_name, input]


def parse_ops(s: str):
    output = re.findall(r"WriteToFinalOutput\((.*)\)", s)
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
        if "BeginTask" in line:
            tmp = ""
            tmp += line
            begin = True
            continue
        if "EndTask" in line:
            tmp += line
            ops .append(tmp)
            begin = False
        if begin:
            tmp += line
            continue
    return ops


def check_if_legal(ops, dep_map):
    for op in ops:
        if op[0] not in dep_map:
            raise Exception("{}算子在json里面没有定义".format(op[0]))
        deps = dep_map[op[0]]
        for each in op[1]:
            if each[1] not in deps["depedencies"]:
                raise Exception(
                    "{}算子依赖的{}不在json设置的依赖里".format(op[0], each[1]))
            if len(each[0]) > 0 and "type" in dep_map[each[1]]:
                left = dep_map[each[1]]["type"]
                right = each[0]
                left = left.split("::")[-1] if "::" in left else left
                right = right.split("::")[-1] if "::" in right else right
                if left != right:
                    raise Exception(
                        "{}算子中使用{}算子的输入类型和json不一致".format(op[0], each[1]))


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
    op_deps = []
    for each in ops:
        op_deps.append(parse_op(each))
    check_if_legal(op_deps, dep_map)
    print("合法的算子构造")
