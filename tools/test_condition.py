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


def build_map(tasks: map):
    succ_map = {}
    prede_map = {}
    for task in tasks["tasks"]:
        task_name = task["task_name"]
        deps = task["dependencies"]
        prede_map[task_name] = deps
        for dep in deps:
            if dep in succ_map:
                succ_map[dep].append(task_name)
            else:
                succ_map[dep] = [task_name]
    return prede_map, succ_map


def dfs(prede_map, succ_map, task, visited, res):
    for prede in prede_map[task]:
        if prede not in visited:
            return
    if task not in res:
        res.append(task)
    if task in succ_map:
        for succ in succ_map[task]:
            dfs(prede_map, succ_map, succ, visited, res)


def dfs(pred_map, succ_map, task, dest):
    if dest == task:
        return True
    if len(pred_map[task]) == 0:
        return False
    for each in pred_map[task]:
        if not dfs(pred_map, succ_map, each, dest):
            return False
    return True


def find_all_path(prede_map, succ_map, task):
    queue = []
    queue.append(task)
    visited = {}
    while len(queue) > 0:
        front = queue.pop(0)
        visited[front] = 1
        if front not in succ_map:
            continue
        succs = succ_map[front]
        # for succ in succs:
        queue.extend(succs)
    res = []
    for each in visited:
        if each != task and dfs(prede_map, succ_map, each, task):
            res.append(each)
    return res


if __name__ == "__main__":
    tasks = parse_json_tasks(
        "/home/lion/ops/taskflow/example/math_test/data/test_json")
    pred, succ = build_map(tasks)
    print(find_all_path(pred, succ, "a"))
