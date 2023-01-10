// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include <algorithm>
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "example/recmd_test/deps/struct_define.h"
#include "taskflow/include/logger/logger.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/include/utils/latency_guard.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

extern "C" {
BEGIN_OP(ParseRequest) {
  GET_GLOBAL_INPUT(RecmdRequest, request);
  TASKFLOW_INFO("request personid:{}, request count:{}", request.personid, request.count);
  RETURN_VAL(request.personid);
}
END_OP

BEGIN_OP(BlackList) {
  GET_INPUT(0, std::string, personid);
  TASKFLOW_INFO("{} op, personid:{}", "BlackList", personid);
  Blacklist blacklist;
  blacklist.black_feeds.emplace("aaaaa", 1);
  blacklist.black_feeds.emplace("bbbbb", 1);
  blacklist.black_posters.emplace("11111", 1);
  RETURN_VAL(blacklist);
}
END_OP

BEGIN_OP(UU) {
  GET_INPUT(0, std::string, personid);
  TASKFLOW_INFO("{} op, personid:{}", "UU", personid);
  UserInfo user;
  user.age = 18;
  user.sex = 1;
  user.address = "Beijing";
  user.caree = "IT";
  Interest interest;
  interest.score = 0.8;
  interest.tag = "beauty";
  user.long_terem_interest.emplace_back(std::move(interest));
  Interest interest2;
  interest2.score = 0.7;
  interest2.tag = "comedy";
  user.short_term_interest.emplace_back(std::move(interest));
  RETURN_VAL(user);
}
END_OP

BEGIN_OP(RecallOP) {
  GET_INPUT(0, UserInfo, user);
  GET_INPUT(1, Blacklist, blacklist);
  TASKFLOW_INFO("user info:sex({}), age({})", user.sex, user.age);
  vector<string> feeds = {"aaaaa", "ccccc", "eeeee"};
  vector<string> posters = {"11111", "33333", "55555"};
  vector<Feed> recall_feeds;
  for (uint64_t i = 0; i < feeds.size(); i++) {
    if (!blacklist.black_feeds.count(feeds[i]) && !blacklist.black_posters.count(posters[i])) {
      Feed feed;
      feed.feedid = feeds[i];
      feed.posterid = posters[i];
      recall_feeds.emplace_back(std::move(feed));
    }
  }
  RecallResult recall_result;
  recall_result.recall_feeds.swap(recall_feeds);
  RETURN_VAL(recall_result);
}
END_OP

BEGIN_OP(RecallMerge) {
  GET_INPUT_TO_VEC(RecallResult, recall_results);
  RecallResult merge_result;
  for (const auto& recall_result : recall_results) {
    for (const auto& feed : recall_result.recall_feeds) {
      merge_result.recall_feeds.emplace_back(feed);
    }
  }
  RETURN_VAL(merge_result);
}
END_OP

BEGIN_OP(Rank) {
  GET_MUTABLE_INPUT(0, RecallResult, recall_result);
  for (auto& each : recall_result.recall_feeds) {
    each.score_map.emplace("aa", random() % 10 / 10.0);
    each.score_map.emplace("bb", random() % 10 / 10.0);
    each.score_map.emplace("cc", random() % 10 / 10.0);
  }
  RankResult rank_result;
  rank_result.rank_feeds.swap(recall_result.recall_feeds);
  RETURN_VAL(rank_result);
}
END_OP

BEGIN_OP(Policy) {
  GET_MUTABLE_INPUT(0, RankResult, rank_result);
  std::sort(rank_result.rank_feeds.begin(), rank_result.rank_feeds.end(),
            [](Feed f1, Feed f2) { return f1.score_map["aa"] > f2.score_map["aa"]; });
  PolicyResult policy_result;
  policy_result.policy_feeds.swap(rank_result.rank_feeds);
  RETURN_VAL(policy_result);
}
END_OP

BEGIN_OP(FillResponse) {
  GET_MUTABLE_INPUT(0, PolicyResult, policy_result);
  RecmdResponse response;
  response.feeds_list.swap(policy_result.policy_feeds);
  WRITE_TO_FINAL_OUTPUT(RecmdResponse, response);
  RETURN_VAL(0);
}
END_OP
}
