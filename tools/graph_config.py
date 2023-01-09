import json

tasks = {"tasks": []}
ops = {
    "add": 2,
    "mult": 3,
    "add_num": 1,
    "mult_num": 1,
    "accum_add": 0,
    "accum_mult": 0,
    "fetch_input": 0,
    "write_output": 1
}

op_list = ["add", "mult", "add_num", "mult_num",
           "accum_add", "accum_mult", "fetch_input", "write_output"]

dep_map = {}

op_string = "0:add,1:mult,2:add_num,3:mult_num,4:accum_add,5:accum_mult,6:fetch_input,7:write_output"


def input_to_task():
    curr_task = []
    for each in tasks["tasks"]:
        curr_task.append(each["task_name"])
    task_name = ""
    while len(task_name) == 0 or task_name in curr_task:
        task_name = input("输入你的任务名:")
    deps = []
    if len(curr_task) > 0:
        while True:
            succ = False
            deps_str = input("你的任务依赖，逗号分隔。目前已知有任务:({}):".format(
                ",".join(curr_task)))
            if len(deps_str.strip()) == 0:
                break
            deps = deps_str.split(",")
            for each in deps:
                if each not in curr_task:
                    succ = True
                    break
            if not succ:
                break

    op_name = input("你的执行算子，输入0-7。目前有这些算子:({}):".format(
        op_string))
    while len(op_name.strip()) == 0 or op_name not in "01234567":
        op_name = input("你的算子没有被定义，请重新输入:({}):".format(
            ",".join(list(ops.keys()))))

    while len(deps) < ops[op_list[int(op_name)]]:
        op_name = input("你的算子依赖不足，请重新输入:({}):".format(
            op_string))
    op_name = op_list[int(op_name)]
    config = ""
    if op_name in ["add_num", "mult_num"]:
        num = input("需要乘以或者加的数，默认为0:")
        config = "num={}".format(num)
    task = {}
    task["task_name"] = task_name
    task["dependencies"] = deps
    task["op_name"] = op_name
    if len(config) > 0:
        task["config"] = config
    tasks["tasks"].append(task)
    dep_map[task_name] = deps


if __name__ == "__main__":
    while True:
        if len(dep_map) > 0:
            print("现在的任务依赖关系:{}\n".format(str(dep_map)))
        input_to_task()
        con = input("继续输入任务?输入回车继续，输入q退出:")
        if con == "q":
            break
        print("\n")
    print(json.dumps(tasks))
