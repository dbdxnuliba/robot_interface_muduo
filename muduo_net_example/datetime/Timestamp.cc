// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "Timestamp.h"

#include <sys/time.h>
#include <stdio.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

using namespace muduo;

static_assert(sizeof(Timestamp) == sizeof(int),
              "Timestamp should be same size as int");

string Timestamp::toString() const
{
  char buf[32] = {0};
  int seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString(bool showMicroseconds) const
{
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds)
  {
    int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  }
  else
  {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

