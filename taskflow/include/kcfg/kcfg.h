// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.
#pragma once
#include <stdint.h>
#include <stdio.h>

#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "taskflow/include/macros/macros.h"

namespace kcfg {
inline const rapidjson::Value* getJsonFiledValue(const rapidjson::Value& json,
                                                 const char* name) {
  if (!name[0]) {
    return &json;
  }
  rapidjson::Value::ConstMemberIterator found = json.FindMember(name);
  if (found == json.MemberEnd()) {
    return NULL;
  }
  return &(found->value);
}

template <typename T>
inline bool Parse(const rapidjson::Value& json, const char* name, T& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsObject()) {
    return false;
  }
  return v.PraseFromJson(*val);
}

template <>
inline bool Parse(const rapidjson::Value& json, const char* name, double& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsNumber()) {
    return false;
  }
  v = val->GetDouble();
  return true;
}

template <>
inline bool Parse(const rapidjson::Value& json, const char* name, float& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsNumber()) {
    return false;
  }
  v = val->GetFloat();
  return true;
}
template <>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::string& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsString()) {
    return false;
  }
  v.assign(val->GetString(), val->GetStringLength());
  return true;
}

template <>
inline bool Parse(const rapidjson::Value& json, const char* name, uint64_t& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsUint64() && !val->IsUint()) {
    return false;
  }
  if (val->IsUint64()) {
    v = val->GetUint64();
  } else {
    v = val->GetUint();
  }
  return true;
}

template <>
inline bool Parse(const rapidjson::Value& json, const char* name, int64_t& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsInt64() && !val->IsInt()) {
    return false;
  }
  if (val->IsInt64()) {
    v = val->GetInt64();
  } else {
    v = val->GetInt();
  }
  return true;
}

template <>
inline bool Parse(const rapidjson::Value& json, const char* name, int32_t& v) {
  int64_t vv;
  if (!Parse(json, name, vv)) {
    return false;
  }
  if (vv > INT32_MAX || vv < INT32_MIN) {
    return false;
  }
  v = (int32_t)vv;
  return true;
}
template <>
inline bool Parse(const rapidjson::Value& json, const char* name, uint32_t& v) {
  uint64_t vv;
  if (!Parse(json, name, vv)) {
    return false;
  }
  if (vv > UINT32_MAX) {
    return false;
  }
  v = (uint32_t)vv;
  return true;
}
template <>
inline bool Parse(const rapidjson::Value& json, const char* name, int16_t& v) {
  int64_t vv;
  if (!Parse(json, name, vv)) {
    return false;
  }
  if (vv > INT16_MAX || vv < INT16_MIN) {
    return false;
  }
  v = (int16_t)vv;
  return true;
}
template <>
inline bool Parse(const rapidjson::Value& json, const char* name, uint16_t& v) {
  uint64_t vv;
  if (!Parse(json, name, vv)) {
    return false;
  }
  if (vv > UINT16_MAX) {
    return false;
  }
  v = (uint16_t)vv;
  return true;
}
template <>
inline bool Parse(const rapidjson::Value& json, const char* name, int8_t& v) {
  int64_t vv;
  if (!Parse(json, name, vv)) {
    return false;
  }
  if (vv > INT8_MAX || vv < INT8_MIN) {
    return false;
  }
  v = (int8_t)vv;
  return true;
}
template <>
inline bool Parse(const rapidjson::Value& json, const char* name, uint8_t& v) {
  int64_t vv;
  if (!Parse(json, name, vv)) {
    return false;
  }
  if (vv > UINT8_MAX) {
    return false;
  }
  v = (uint8_t)vv;
  return true;
}

template <>
inline bool Parse(const rapidjson::Value& json, const char* name, bool& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsBool()) {
    return false;
  }
  v = val->GetBool();
  return true;
}

template <typename K>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::vector<K>& v);

template <typename V>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::map<std::string, V>& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsObject()) {
    return false;
  }
  v.clear();
  rapidjson::Value::ConstMemberIterator it = val->MemberBegin();
  while (it != val->MemberEnd()) {
    V tmp;
    std::string fieldName = it->name.GetString();
    if (Parse(it->value, "", tmp)) {
      v[fieldName] = tmp;
    } else {
      v.clear();
      return false;
    }
    it++;
  }
  return true;
}
template <typename K>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::vector<K>& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsArray()) {
    return false;
  }
  v.clear();
  for (rapidjson::Value::ConstValueIterator ait = val->Begin();
       ait != val->End(); ++ait) {
    K tmp;
    if (Parse(*ait, "", tmp)) {
      v.push_back(tmp);
    } else {
      v.clear();
      return false;
    }
  }
  return true;
}
template <typename K>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::deque<K>& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsArray()) {
    return false;
  }
  v.clear();
  for (rapidjson::Value::ConstValueIterator ait = val->Begin();
       ait != val->End(); ++ait) {
    K tmp;
    if (Parse(*ait, "", tmp)) {
      v.push_back(tmp);
    } else {
      v.clear();
      return false;
    }
  }
  return true;
}
template <typename K>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::list<K>& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsArray()) {
    return false;
  }
  v.clear();
  for (rapidjson::Value::ConstValueIterator ait = val->Begin();
       ait != val->End(); ++ait) {
    K tmp;
    if (Parse(*ait, "", tmp)) {
      v.push_back(tmp);
    } else {
      v.clear();
      return false;
    }
  }
  return true;
}

template <typename K>
inline bool Parse(const rapidjson::Value& json, const char* name,
                  std::set<K>& v) {
  const rapidjson::Value* val = getJsonFiledValue(json, name);
  if (NULL == val) {
    return false;
  }
  if (!val->IsArray()) {
    return false;
  }
  v.clear();
  for (rapidjson::Value::ConstValueIterator ait = val->Begin();
       ait != val->End(); ++ait) {
    K tmp;
    if (Parse(*ait, "", tmp)) {
      v.insert(tmp);
    } else {
      v.clear();
      return false;
    }
  }
  return true;
}

inline void addJsonMember(rapidjson::Value& json,
                          rapidjson::Value::AllocatorType& allocator,
                          const char* name, rapidjson::Value& json_value) {
  if (NULL == name) {
    json = json_value;
    return;
  }
  if (!name[0]) {
    json.PushBack(json_value, allocator);
  } else {
    rapidjson::Value field_name(name, strlen(name));
    json.AddMember(field_name, json_value, allocator);
  }
}

template <typename T>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const T& v) {
  rapidjson::Value json_value(rapidjson::kObjectType);
  v.WriteToJson(json_value, allocator);
  addJsonMember(json, allocator, name, json_value);
}

template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const double& v) {
  rapidjson::Value json_value(rapidjson::kNumberType);
  json_value.SetDouble(v);
  addJsonMember(json, allocator, name, json_value);
}

template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const float& v) {
  rapidjson::Value json_value(rapidjson::kNumberType);
  json_value.SetDouble(v);
  addJsonMember(json, allocator, name, json_value);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::string& v) {
  rapidjson::Value json_value(v.c_str(), v.size(), allocator);
  addJsonMember(json, allocator, name, json_value);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const bool& v) {
  rapidjson::Value json_value(rapidjson::kNumberType);
  json_value.SetBool(v);
  addJsonMember(json, allocator, name, json_value);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const uint64_t& v) {
  rapidjson::Value json_value(rapidjson::kNumberType);
  json_value.SetUint64(v);
  addJsonMember(json, allocator, name, json_value);
}

template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const int64_t& v) {
  rapidjson::Value json_value(rapidjson::kNumberType);
  json_value.SetInt64(v);
  addJsonMember(json, allocator, name, json_value);
}

template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const int32_t& v) {
  int64_t vv = v;
  Serialize(json, allocator, name, vv);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const uint32_t& v) {
  uint64_t vv = v;
  Serialize(json, allocator, name, vv);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const int16_t& v) {
  int64_t vv = v;
  Serialize(json, allocator, name, vv);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const uint16_t& v) {
  uint64_t vv = v;
  Serialize(json, allocator, name, vv);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const int8_t& v) {
  int64_t vv = v;
  Serialize(json, allocator, name, vv);
}
template <>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const uint8_t& v) {
  uint64_t vv = v;
  Serialize(json, allocator, name, vv);
}

template <typename K>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::vector<K>& v);

template <typename V>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::map<std::string, V>& v) {
  rapidjson::Value json_value(rapidjson::kObjectType);
  typename std::map<std::string, V>::const_iterator it = v.begin();
  while (it != v.end()) {
    const V& vv = it->second;
    Serialize(json_value, allocator, it->first.c_str(), vv);
    it++;
  }
  addJsonMember(json, allocator, name, json_value);
}
template <typename K>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::deque<K>& v) {
  rapidjson::Value json_value(rapidjson::kArrayType);
  for (size_t i = 0; i < v.size(); i++) {
    const K& vv = v[i];
    Serialize(json_value, allocator, "", vv);
  }
  addJsonMember(json, allocator, name, json_value);
}
template <typename K>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::vector<K>& v) {
  rapidjson::Value json_value(rapidjson::kArrayType);
  for (size_t i = 0; i < v.size(); i++) {
    const K& vv = v[i];
    Serialize(json_value, allocator, "", vv);
  }
  addJsonMember(json, allocator, name, json_value);
}
template <typename K>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::list<K>& v) {
  rapidjson::Value json_value(rapidjson::kArrayType);
  typename std::list<K>::const_iterator cit = v.begin();
  while (cit != v.end()) {
    const K& vv = *cit;
    Serialize(json_value, allocator, "", vv);
    cit++;
  }
  addJsonMember(json, allocator, name, json_value);
}
template <typename K>
inline void Serialize(rapidjson::Value& json,
                      rapidjson::Value::AllocatorType& allocator,
                      const char* name, const std::set<K>& v) {
  rapidjson::Value json_value(rapidjson::kArrayType);
  typename std::set<K>::const_iterator cit = v.begin();
  while (cit != v.end()) {
    const K& vv = *cit;
    Serialize(json_value, allocator, "", vv);
    cit++;
  }
  addJsonMember(json, allocator, name, json_value);
}
template <typename T>
inline bool ParseFromJsonString(const std::string& content, T& v) {
  rapidjson::Document d;
  d.Parse<0>(content.c_str());
  if (d.HasParseError()) {
    return false;
  }
  return Parse(d, "", v);
}

template <typename T>
inline bool ParseFromJsonFile(const std::string& file, T& v) {
  FILE* fp;
  if ((fp = fopen(file.c_str(), "rb")) == NULL) {
    return false;
  }

  fseek(fp, 0, SEEK_END);
  long int fsize = ftell(fp);
  if (fsize >= 100 * 1024 * 1024) {
    // too large for a config file
    fclose(fp);
    return false;
  }
  rewind(fp);
  char* buffer = (char*)malloc(fsize);
  if (NULL == buffer) {
    fclose(fp);
    return false;
  }
  std::string content;
  size_t len = fread(buffer, 1, fsize, fp);
  content.assign(buffer, len);
  free(buffer);
  fclose(fp);
  return ParseFromJsonString(content, v);
}

template <typename T>
inline int WriteToJsonString(const T& v, std::string& content) {
  rapidjson::Value::AllocatorType allocator;
  rapidjson::Value doc(rapidjson::kObjectType);
  // v.WriteToJson(doc, allocator);
  Serialize(doc, allocator, NULL, v);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  content.assign(buffer.GetString(), buffer.GetSize());
  return 0;
}

template <typename T>
inline int WriteToJsonFile(const T& v, const std::string& file) {
  std::string content;
  int ret = WriteToJsonString(v, content);
  if (0 != ret) {
    return ret;
  }
  FILE* fp;
  if ((fp = fopen(file.c_str(), "wb")) == NULL) {
    return -1;
  }
  fwrite(content.c_str(), content.size(), 1, fp);
  fclose(fp);
  return 0;
}
}  // namespace kcfg

#define KCFG_PARSE_JSON(field) kcfg::Parse(json, #field, field)
#define KCFG_SERIALIZE_JSON(field) \
  kcfg::Serialize(json, allocator, #field, field)
#define KCFG_DEFINE_FIELDS(...)                                        \
  bool PraseFromJson(const rapidjson::Value& json) {                   \
    KCFG_FOR_EACH(KCFG_PARSE_JSON, __VA_ARGS__)                        \
    return true;                                                       \
  }                                                                    \
  void WriteToJson(rapidjson::Value& json,                             \
                   rapidjson::Value::AllocatorType& allocator) const { \
    KCFG_FOR_EACH(KCFG_SERIALIZE_JSON, __VA_ARGS__)                    \
  }
