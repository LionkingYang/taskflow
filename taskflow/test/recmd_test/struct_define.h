// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::vector;

struct RecmdRequest {
  std::string personid;
  int count;

  RecmdRequest() : count(0) {}
};

struct Feed {
  std::string feedid;
  std::string posterid;
  std::string recmd_reason;
  std::unordered_map<string, double> score_map;
};

struct RecmdResponse {
  vector<Feed> feeds_list;
};

struct Interest {
  string tag;
  double score;
};

struct UserInfo {
  int age;
  int sex;
  string address;
  string caree;
  vector<Interest> long_terem_interest;
  vector<Interest> short_term_interest;
};

struct Blacklist {
  std::unordered_map<string, int> black_feeds;
  std::unordered_map<string, int> black_posters;
};

struct RecallResult {
  vector<Feed> recall_feeds;
};

struct RankResult {
  vector<Feed> rank_feeds;
};

struct PolicyResult {
  vector<Feed> policy_feeds;
};
