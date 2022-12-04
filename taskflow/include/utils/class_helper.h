// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#define DISALLOW_COPY_AND_ASSIGN(clv)  \
  clv(const clv&) = delete;            \
  clv(clv&&) = delete;                 \
  clv& operator=(const clv&) = delete; \
  clv& operator=(clv&&) = delete;