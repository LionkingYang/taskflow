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
        t["dependencies"] = task["dependencies"]
        if "type" in task:
            t["type"] = task["type"]
        else:
            t["type"] = ""
        if "final_output" in task and task["final_output"] == "1":
            t["output"] = "1"
        else:
            t["output"] = "0"
        t["use_input"] = "0"
        if "use_input" in task:
            t["use_input"] = task["use_input"]
        dep_map[task["op_name"]] = t
    return dep_map


def generate_one_op(op_name: str, dep_map: map, input_type: str, output_type: str):
    op_str = "\n\nBEGIN_OP({}) {{\n".format(op_name)
    if dep_map[op_name]["use_input"] == "1":
        op_str += "  GET_GLOBAL_INPUT({}, input_name);\n".format(input_type)
    i = 0
    for each in dep_map[op_name]["dependencies"]:
        op_str += "  GET_INPUT({}, {}, {});\n".format(i,
                                                      "$param_type", each+"_output")
        i += 1
    op_str += "  // write your code here\n"
    if dep_map[op_name]["output"] == "1":
        op_str += "  WRITE_TO_FINAL_OUTPUT({}, final_output);\n".format(
            output_type)

    op_str += "  RETURN_VAL({});\n".format("your_output")
    op_str += "}\nEND_OP;"
    return op_str


if __name__ == "__main__":
    op_path = ""
    js_path = ""
    if len(sys.argv) >= 3:
        op_path = sys.argv[1]
        js_path = sys.argv[2]
    else:
        raise Exception("在命令行输入算子文件地址和json地址")

    if not os.path.exists(js_path):
        raise Exception("json文件不存在!")

    tasks = parse_json_tasks(js_path)
    dep_map = build_depedency_map(tasks)
    input_type = "$input_type" if "input_type" not in tasks else tasks["input_type"]
    output_type = "$output_type" if "output_type" not in tasks else tasks["output_type"]
    op_content = """
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/logger/logger.h"
#include "taskflow/include/taskflow.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

extern "C" {
"""
    for dep in dep_map:
        op_content += generate_one_op(dep, dep_map, input_type, output_type)
    op_content += "\n}\n"
    with open(op_path, "w") as f:
        f.write(op_content)
