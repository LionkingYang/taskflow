// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include <queue>
#include <set>

#include "taskflow/include/taskflow.h"

using std::queue;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;

namespace taskflow {

void Graph::CircleCheck() {
  // 为了不影响后续正常执行，复制出一份依赖关系出来
  unordered_map<string, int> map_finish;
  auto tmp_predecessor_count = predecessor_count_;
  // 此处逻辑与执行类似，只是多了一层判断
  // 会判断依赖关系能否完全解耦，即所有任务都可以最终到达没有前置依赖的地步
  while (true) {
    bool found = false;
    for (const auto& node : nodes_) {
      if (tmp_predecessor_count[node->GetNodeName()] == 0 &&
          map_finish.count(node->GetNodeName()) == 0) {
        for (const auto& each : node->GetSuccessors()) {
          tmp_predecessor_count[each->GetNodeName()]--;
        }
        map_finish.emplace(node->GetNodeName(), 1);
        found = true;
      }
    }
    // 所有任务都能按照某种顺序执行完成，无环
    if (uint64_t(map_finish.size()) == nodes_.size()) {
      is_circle_ = false;
      return;
    }
    // 在某次循环发现每个未执行任务都有前置依赖了，存在环依赖，退出
    if (!found) {
      is_circle_ = true;
      return;
    }
  }
}

// 从json字符串中构建tasks
bool Graph::BuildFromJson(const string& json_path) {
  GraphConf graph;
  if (!kcfg::ParseFromJsonFile(json_path, graph)) {
    TASKFLOW_CRITICAL("error in parse json");
    return false;
  }
  // 遍历一遍，拿到Node列表
  for (const auto& node_conf : graph.tasks) {
    TaskPtr task = std::make_shared<Task>(node_conf.op_name, node_conf.config,
                                          node_conf.condition, node_conf.async);
    NodePtr node = std::make_shared<Node>(node_conf.task_name, task);

    nodes_.emplace_back(node);
    node_map_.emplace(node->GetNodeName(), node);
  }
  // 再遍历一遍，补充依赖关系
  for (const auto& node_conf : graph.tasks) {
    for (const auto& dep : node_conf.dependencies) {
      if (auto iter = node_map_.find(dep); iter != node_map_.end()) {
        node_map_[node_conf.task_name]->AddPredecessor(iter->second);
        node_map_[iter->second->GetNodeName()]->AddSuccessor(
            node_map_[node_conf.task_name]);
      }
    }
  }

  for (auto node : nodes_) {
    // 构建依赖map，即某个task如a依赖了哪些task
    // 当某个task的值为0时，说明task没有前置依赖，可以被执行了
    predecessor_count_[node->GetNodeName()] = node->GetPredecessorCount();
  }
  return true;
}

void Graph::DFS(const string& node_name,
                const unordered_map<string, string>& visited,
                unordered_set<string>& res) {
  for (auto node : node_map_[node_name]->GetPredecessors()) {
    if (visited.find(node->GetNodeName()) == visited.end()) {
      return;
    }
  }
  res.insert(node_name);
  for (auto node : node_map_[node_name]->GetSuccessors()) {
    DFS(node->GetNodeName(), visited, res);
  }
}

unordered_set<string> Graph::FindConditionInfer(const string& node_name) {
  unordered_map<string, string> visited;
  queue<NodePtr> q;
  q.emplace(node_map_[node_name]);
  visited.emplace(node_name, "1");
  while (!q.empty()) {
    auto front = q.front();
    q.pop();
    for (auto node : front->GetSuccessors()) {
      if (visited.find(node->GetNodeName()) == visited.end()) {
        visited.emplace(node->GetNodeName(), "1");
        q.emplace(node);
      }
    }
  }
  unordered_set<string> res;
  for (auto node : node_map_[node_name]->GetSuccessors())
    DFS(node->GetNodeName(), visited, res);
  return res;
}

void Graph::BuildConditionMap() {
  for (const auto& node : nodes_) {
    auto res = FindConditionInfer(node->GetNodeName());
    condition_map_[node->GetNodeName()] = move(res);
  }
}

std::string Graph::ToString() {
  std::string s;
  taskflow::StrAppend(&s, "predecessor map:\n");
  for (auto begin = predecessor_count_.begin();
       begin != predecessor_count_.end(); begin++) {
    taskflow::StrAppend(&s, begin->first, ":", begin->second, "\n");
  }
  taskflow::StrAppend(&s, "dependency map:\n");
  for (auto node : node_map_) {
    taskflow::StrAppend(&s, node.first + "|Succ:");
    for (auto succ : node.second->GetSuccessors()) {
      taskflow::StrAppend(&s, succ->GetNodeName());
    }
    taskflow::StrAppend(&s, "|Pred:");
    for (auto succ : node.second->GetPredecessors()) {
      taskflow::StrAppend(&s, succ->GetNodeName());
    }
    taskflow::StrAppend(&s, "\n");
  }
  taskflow::StrAppend(&s, "condition map:\n");
  for (const auto& cond : condition_map_) {
    taskflow::StrAppend(&s, cond.first + "|");
    for (const auto& each : cond.second) {
      taskflow::StrAppend(&s, each + ":");
    }
    taskflow::StrAppend(&s, "\n");
  }
  return s;
}
}  // namespace taskflow
