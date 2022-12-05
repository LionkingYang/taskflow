// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <algorithm>
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "taskflow/include/taskflow.h"
#include "test/recmd_test/struct_define.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

BeginFunc(ParseRequest) {
  GetGlobalInput(RecmdRequest, request);
  std::cout << "request personid:" << request.personid << std::endl;
  WriteToOutput(ParseRequest, std::string, request.personid);
}
EndFunc

BeginFunc(BlackList) {
  ReadTaskOutput(ParseRequest, std::string, personid);
  Blacklist blacklist;
  blacklist.black_feeds.emplace("aaaaa", 1);
  blacklist.black_feeds.emplace("bbbbb", 1);
  blacklist.black_posters.emplace("11111", 1);
  WriteToOutput(BlackList, Blacklist, blacklist);
}
EndFunc

BeginFunc(UU) {
  ReadTaskOutput(ParseRequest, std::string, personid);
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
  WriteToOutput(UU, UserInfo, user);
}
EndFunc

BeginFunc(RecallCB) {
  ReadTaskOutput(UU, UserInfo, user);
  ReadTaskOutput(BlackList, Blacklist, blacklist);
  vector<string> feeds = {"aaaaa", "ccccc", "eeeee"};
  vector<string> posters = {"11111", "33333", "55555"};
  vector<Feed> recall_feeds;
  for (uint64_t i = 0; i < feeds.size(); i++) {
    if (!blacklist.black_feeds.count(feeds[i]) &&
        !blacklist.black_posters.count(posters[i])) {
      Feed feed;
      feed.feedid = feeds[i];
      feed.posterid = posters[i];
      recall_feeds.emplace_back(std::move(feed));
    }
  }
  RecallResult cb_result;
  cb_result.recall_feeds.swap(recall_feeds);
  WriteToOutput(RecallCB, RecallResult, cb_result);
}
EndFunc

BeginFunc(RecallEMB) {
  ReadTaskOutput(UU, UserInfo, user);
  ReadTaskOutput(BlackList, Blacklist, blacklist);
  vector<string> feeds = {"bbbbb", "ddddd", "fffff"};
  vector<string> posters = {"22222", "44444", "66666"};
  vector<Feed> recall_feeds;
  for (uint64_t i = 0; i < feeds.size(); i++) {
    if (!blacklist.black_feeds.count(feeds[i]) &&
        !blacklist.black_posters.count(posters[i])) {
      Feed feed;
      feed.feedid = feeds[i];
      feed.posterid = posters[i];
      recall_feeds.emplace_back(std::move(feed));
    }
  }
  RecallResult emb_result;
  emb_result.recall_feeds.swap(recall_feeds);
  WriteToOutput(RecallEMB, RecallResult, emb_result);
}
EndFunc

BeginFunc(RecallMerge) {
  ReadTaskOutput(RecallCB, RecallResult, cb_result);
  ReadTaskOutput(RecallEMB, RecallResult, emb_result);
  RecallResult merge_result;
  for (const auto& each : emb_result.recall_feeds) {
    merge_result.recall_feeds.emplace_back(each);
  }
  for (const auto& each : cb_result.recall_feeds) {
    merge_result.recall_feeds.emplace_back(each);
  }
  WriteToOutput(RecallMerge, RecallResult, merge_result);
}
EndFunc

BeginFunc(Rank) {
  ReadTaskOutput(RecallMerge, RecallResult, recall_result);
  for (auto& each : recall_result.recall_feeds) {
    each.score_map.emplace("aa", random() % 10 / 10.0);
    each.score_map.emplace("bb", random() % 10 / 10.0);
    each.score_map.emplace("cc", random() % 10 / 10.0);
  }
  RankResult rank_result;
  rank_result.rank_feeds.swap(recall_result.recall_feeds);
  WriteToOutput(Rank, RankResult, rank_result);
}
EndFunc

BeginFunc(Policy) {
  ReadTaskOutput(Rank, RankResult, rank_result);
  std::sort(
      rank_result.rank_feeds.begin(), rank_result.rank_feeds.end(),
      [](Feed f1, Feed f2) { return f1.score_map["aa"] > f2.score_map["aa"]; });
  PolicyResult policy_result;
  policy_result.policy_feeds.swap(rank_result.rank_feeds);
  WriteToOutput(Policy, PolicyResult, policy_result);
}
EndFunc

BeginFunc(FillResponse) {
  ReadTaskOutput(Policy, PolicyResult, policy_result);
  RecmdResponse response;
  response.feeds_list.swap(policy_result.policy_feeds);
  WriteToFinalOutput(RecmdResponse, response);
}
EndFunc
