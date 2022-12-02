import json
import sys
import os


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
        dep_map[task["task_name"]] = task["dependencies"]
    return dep_map


def judge_if_legal(dep_map: map) -> str:
    for task in dep_map:
        for dep in dep_map[task]:
            if dep not in dep_map:
                return dep
    return ""


# 判断拓扑排序是否结束
def is_over(dep_map: map, deleted: list) -> int:
    count = 0
    ret = []
    has_non_zero = False
    for each in dep_map:
        if len(dep_map[each]) != 0:
            has_non_zero = True
        else:
            ret.append(each)
    # 如果存在还有依赖的文件，且依赖为0的文件已经都出列过了，说明有循环依赖
    if (has_non_zero and set(deleted) == set(ret)):
        return 1
    # 所有文件都没有依赖了，说明流程正常走完，没有循环依赖
    if not has_non_zero:
        return 2
    # 说明还有文件可以继续进行依赖剥除，继续操作
    return 0


def judge_circular_reference(dep_map: map) -> int:
    deleted = []
    while True:
        for each in dep_map:
            # 如果某个文件没有依赖，出列，并且所有依赖它的文件，依赖-1
            if len(dep_map[each]) == 0 and each not in deleted:
                for e in dep_map:
                    if each in dep_map[e]:
                        dep_map[e].remove(each)
                deleted.append(each)
                break
        res = is_over(dep_map, deleted)
        if res != 0:
            return res == 1


if __name__ == "__main__":
    path = ""
    if len(sys.argv) >= 2:
        path = sys.argv[1]
    else:
        path = input("输入json的绝对地址:")
    if not os.path.exists(path):
        raise Exception("json文件不存在!")

    tasks = parse_json_tasks(path)
    dep_map = build_depedency_map(tasks)
    # 检查是否有非法依赖
    legal = judge_if_legal(dep_map)
    if len(legal) > 0:
        raise Exception("存在非法依赖{}，检查一下".format(legal))
    if judge_circular_reference(dep_map):
        raise Exception("存在循环依赖")
    print("合法的依赖文件")
