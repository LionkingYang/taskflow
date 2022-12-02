# circle_reference_check.py

# 使用拓扑排序检测项目是否存在.h文件的循环依赖
# 需要python3.6.8及以上版本执行

# usage:
#  1. python3 circle_reference_check.py
#     输入项目在本地的绝对地址，如：/data/svn2/weishi_ips
#  2. python3 circle_reference_check.py /data/svn2/weishi_ips
# 输出存在两种情况：
#    1. 不存在循环依赖，打印: No circle reference
#    2. 存在循环依赖，  打印: Has circular reference, 并且打印出无法解依赖的几个.h依赖

# 脚本不足：
#   1. 由于成环引用，环上的每个依赖都可以作为环的起点
#      所以暂时无法精准的定位哪个.h文件为循环的起点，需要人为去解引用
#   2. 只支持项目内部的环引用检测，外部依赖的无法检测到

import os
import sys
import time


# 分析.h文件获取include, 只保留项目内依赖
def find_header_dependencies(file: str, path: str):
    res = []
    with open(file, "r", encoding="ISO-8859-1") as f:
        for each in f:
            if "#include" in each and each[0] == "#" and '"' in each:
                ref_file = each.split('"')[1]
                # 使用绝对路径的依赖
                if os.path.exists(path+"/"+ref_file):
                    res.append(os.path.abspath(path+"/"+ref_file))
                # 使用相对路径的依赖
                elif os.path.exists("/".join(file.split("/")[:-1])+"/"+ref_file):
                    res.append(os.path.abspath(
                        "/".join(file.split("/")[:-1])+"/"+ref_file))
    return res


# 简单迭代项目路径获取所有.h/.hpp文件
def find_all_header_files(path: str):
    res = []
    files = os.listdir(path)
    for file in files:
        f = path+"/"+file
        if not os.path.isdir(f):
            if ".h" in file:
                res.append(os.path.abspath(path+"/"+file))
        else:
            res.extend(find_all_header_files(path+"/"+file))
    return res


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
            return res


if __name__ == '__main__':
    path = ""
    if len(sys.argv) >= 2:
        path = sys.argv[1]
    else:
        path = input("输入项目的绝对地址:")
    if not os.path.exists(path):
        raise Exception("Project don't exist, check again please!")

    header_files = find_all_header_files(path)
    # 抽取依赖关系
    dependency_map = {}
    for header_file in header_files:
        dependency_map[header_file] = find_header_dependencies(
            header_file, path)
    # 拓扑排序判断是否存在环依赖
    if judge_circular_reference(dependency_map) == 1:
        print("Has circular reference")
        for each in dependency_map:
            if len(dependency_map[each]) != 0:
                print(each, len(dependency_map[each]), dependency_map[each])
    else:
        print("No circle reference")
