// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <any>
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/task.h"
#include "taskflow/include/work_manager/work_manager.h"

using std::unordered_map;
using std::unordered_set;
using std::vector;
using taskflow::Node;
using taskflow::NodePtr;

namespace taskflow {
class Graph {
 public:
  Graph() {}
  bool Init(const string& graph_path) {
    is_circle_ = false;
    predecessor_count_.clear();
    condition_map_.clear();
    nodes_.clear();
    node_map_.clear();
    if (!BuildFromJson(graph_path)) {
      return false;
    }
    CircleCheck();
    BuildConditionMap();
    worker_ = std::make_shared<WorkManagerWithNum>(GetNodes().size());
    return true;
  }

  const std::unordered_map<string, int>& GetPredecessorCount() {
    return predecessor_count_;
  }
  const vector<NodePtr>& GetNodes() { return nodes_; }
  bool GetCircle() { return is_circle_; }
  std::string ToString();
  unordered_map<string, unordered_set<string>>& GetConditionMap() {
    return condition_map_;
  }
  auto GetWorker() { return worker_; }

 private:
  bool BuildFromJson(const string& graph_path);
  void CircleCheck();
  unordered_set<string> FindConditionInfer(const string& node_name);
  void DFS(const string& node_name,
           const unordered_map<string, string>& visited,
           unordered_set<string>& res);
  void BuildConditionMap();

 private:
  unordered_map<string, int> predecessor_count_;
  vector<NodePtr> nodes_;
  unordered_map<string, NodePtr> node_map_;
  bool is_circle_ = false;
  unordered_map<string, unordered_set<string>> condition_map_;
  std::shared_ptr<WorkManagerWithNum> worker_ = nullptr;
};

}  // namespace taskflow
