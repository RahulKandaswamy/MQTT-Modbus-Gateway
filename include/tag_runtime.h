#ifndef TAG_RUNTIME_H
#define TAG_RUNTIME_H

#include "tag_registry.h"
#include "system_err.h"

// Renamed from tag_data_t to tag_runtime_t
typedef struct {
  const tag_config_t* config;
  tag_value_t value;         
  uint32_t timestamp;
  bool valid;
  bool changed;
} tag_runtime_t;

sys_status_t tag_runtime_init();
sys_status_t tag_runtime_lock(void);
sys_status_t tag_runtime_unlock(void);

tag_runtime_t* tag_runtime_get(uint32_t tagId);
tag_runtime_t* tag_runtime_get_at(uint16_t idx);
void tag_runtime_print(const tag_runtime_t* tag);

#endif