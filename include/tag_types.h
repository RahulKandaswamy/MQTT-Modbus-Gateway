#ifndef TAG_TYPES_H
#define TAG_TYPES_H

/**
 * @file tag_types.h
 * @brief Plain data types shared by the tag registry, runtime and codecs.
 *
 * This header must stay free of Arduino / ESP-IDF includes so that code which
 * only depends on it (e.g. modbus_codec) can be unit-tested on the host
 * with `pio test -e native`.
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  WORD_ABCD = 10,
  WORD_CDAB = 11,
  WORD_BADC = 12,
  WORD_DCBA = 13
} word_order_t;

typedef enum {
  TAG_ACCESS_READ_ONLY = 20,
  TAG_ACCESS_WRITE_ONLY = 21,
  TAG_ACCESS_READ_WRITE = 22
} tag_access_t;

typedef enum {
  TAG_SOURCE_MODBUS = 30,
  TAG_SOURCE_CAN = 31,
  TAG_SOURCE_SYSTEM = 32,
} tag_source_t;

typedef enum {
  TAG_TOPIC_TYPE_SUBSCRIBE = 40,
  TAG_TOPIC_TYPE_PUBLISH = 41
} tag_topic_type_t;

typedef enum {
  TAG_BOOL = 50,
  TAG_UINT16 = 51,
  TAG_INT16 = 52,
  TAG_UINT32 = 53,
  TAG_INT32 = 54,
  TAG_FLOAT32 = 55,
  TAG_STRING = 56
} tag_type_t;

typedef union {
  bool bv;
  uint16_t u16v;
  int16_t i16v;
  uint32_t u32v;
  int32_t i32v;
  float f32v;
  char strv[64];
} tag_value_t;

#endif
