#pragma once
#include <algorithm>
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/taskflow.h"
#include "taskflow/test/recmd_test/struct_define.h"

BeginFunc(ParseRequest) {
  RecmdRequest request = Input(RecmdRequest);
  std::cout << "request personid:" << request.personid << std::endl;
  WriteToOutput(ParseRequest, request.personid, std::string);
}

BeginFunc(BlackList) {
  std::string personid = ReadTaskOutput(ParseRequest, std::string);
  Blacklist blacklist;
  blacklist.black_feeds.emplace("aaaaa", 1);
  blacklist.black_feeds.emplace("bbbbb", 1);
  blacklist.black_posters.emplace("11111", 1);
  WriteToOutput(BlackList, blacklist, Blacklist);
}

BeginFunc(UU) {
  std::string personid = ReadTaskOutput(ParseRequest, std::string);
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
  WriteToOutput(UU, user, UserInfo);
}

BeginFunc(RecallCB) {
  UserInfo user = ReadTaskOutput(UU, UserInfo);
  Blacklist blacklist = ReadTaskOutput(BlackList, Blacklist);
  vector<string> feeds = {"aaaaa", "ccccc", "eeeee"};
  vector<string> posters = {"11111", "33333", "55555"};
  vector<Feed> recall_feeds;
  for (int i = 0; i < feeds.size(); i++) {
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
  WriteToOutput(RecallCB, cb_result, RecallResult);
}

BeginFunc(RecallEMB) {
  UserInfo user = ReadTaskOutput(UU, UserInfo);
  Blacklist blacklist = ReadTaskOutput(BlackList, Blacklist);
  vector<string> feeds = {"bbbbb", "ddddd", "fffff"};
  vector<string> posters = {"22222", "44444", "66666"};
  vector<Feed> recall_feeds;
  for (int i = 0; i < feeds.size(); i++) {
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
  WriteToOutput(RecallEMB, emb_result, RecallResult);
}

BeginFunc(RecallMerge) {
  RecallResult cb_result = ReadTaskOutput(RecallCB, RecallResult);
  RecallResult emb_result = ReadTaskOutput(RecallEMB, RecallResult);
  RecallResult merge_result;
  for (const auto& each : emb_result.recall_feeds) {
    merge_result.recall_feeds.emplace_back(each);
  }
  for (const auto& each : cb_result.recall_feeds) {
    merge_result.recall_feeds.emplace_back(each);
  }
  WriteToOutput(RecallMerge, merge_result, RecallResult);
}

BeginFunc(Rank) {
  RecallResult recall_result = ReadTaskOutput(RecallMerge, RecallResult);
  for (auto& each : recall_result.recall_feeds) {
    each.score_map.emplace("aa", random() % 10 / 10.0);
    each.score_map.emplace("bb", random() % 10 / 10.0);
    each.score_map.emplace("cc", random() % 10 / 10.0);
  }
  RankResult rank_result;
  rank_result.rank_feeds.swap(recall_result.recall_feeds);
  WriteToOutput(Rank, rank_result, RankResult);
}

BeginFunc(Policy) {
  RankResult rank_result = ReadTaskOutput(Rank, RankResult);
  std::sort(
      rank_result.rank_feeds.begin(), rank_result.rank_feeds.end(),
      [](Feed f1, Feed f2) { return f1.score_map["aa"] > f2.score_map["aa"]; });
  PolicyResult policy_result;
  policy_result.policy_feeds.swap(rank_result.rank_feeds);
  WriteToOutput(Policy, policy_result, PolicyResult);
}

BeginFunc(FillResponse) {
  PolicyResult policy_result = ReadTaskOutput(Policy, PolicyResult);
  RecmdResponse response;
  response.feeds_list.swap(policy_result.policy_feeds);
  WriteToFinalOutput(response, RecmdResponse);
}
