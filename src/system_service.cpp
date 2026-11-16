#include "system_service.h"
#include "logger.h"
#include "acquisition.h"

#define MODULE "SYSTEM-SVC"

/**
 * @brief Serves read requests for tags whose source is SYSTEM (internal gateway values).
 *
 * No system tags are defined yet, so every request is answered with
 * SYS_ERR_NOT_FOUND. Add new internal values here by matching on the tag name.
 */
sys_status_t system_service_read_request(const protocol_request_t* request){
  if(request == NULL || request->tag == NULL) return SYS_ERR_INVALID_PARAM;

  // Ensure we are only handling internal system requests
  if(request->tag->source != TAG_SOURCE_SYSTEM) return SYS_ERR_INVALID_PARAM;

  protocol_response_t p_resp = {};
  p_resp.tagId = request->tag->tagId;

  LOG_WARN(MODULE, "Unknown system tag: %s", request->tag->name);
  p_resp.status = SYS_ERR_NOT_FOUND;

  return acquisition_post_response(&p_resp);
}
