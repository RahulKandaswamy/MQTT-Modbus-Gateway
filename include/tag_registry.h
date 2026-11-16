#ifndef TAG_REGISTRY_H
#define TAG_REGISTRY_H

#include <Arduino.h>
#include <system_err.h>
#include "tag_types.h"


typedef struct {
  uint8_t slaveId;
  uint16_t address;
  uint16_t quantity;
  uint8_t functionCode;
  word_order_t wordOrder;
} tag_modbus_config_t;

typedef struct {
  uint8_t pin;
} tag_gpio_config_t;

typedef union {
  tag_modbus_config_t modbus;
  tag_gpio_config_t gpio;
} tag_source_config_t;

// LEAN TAG CONFIGURATION
typedef struct {
  uint32_t tagId;
  const char* name;
  const char* unit;
  const char* topic;
  tag_topic_type_t topicType;
  tag_source_t source;
  tag_access_t access;
  tag_type_t dataType; 
  tag_source_config_t sourceConfig;
} tag_config_t;

/* API */
sys_status_t tag_registry_init(uint16_t capacity);
sys_status_t tag_registry_add(const tag_config_t* tag);
const tag_config_t* tag_find_by_id(uint16_t tagId);
const tag_config_t* tag_find_by_name(const char* name);
const tag_config_t* tag_find_by_topic(const char* topic);
const tag_config_t* tag_get_at(uint16_t index);
uint16_t tag_count();

#endif