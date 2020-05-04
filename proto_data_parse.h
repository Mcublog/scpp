#ifndef CONSOLE_DATA_PARSE_H
#define CONSOLE_DATA_PARSE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#include "proto_types.h"

uint32_t proto_check_start_stop_symb(const uint8_t* buf);

uint32_t proto_cmd_get_size(const uint8_t* buf);
uint32_t proto_cmd_parse(const uint8_t* buf, proto_cmd_t* cl_cmd);

cmd_id_t get_cmd_id(comp_t* comp,const cmd_t* cmd_list);

void proto_form_comp(comp_t* comp, const uint8_t *data);
void proto_form_head(const char* name, const char* type, const char* size , proto_cmd_t* cl_cmd);

void proto_form_comp_bin(comp_t* comp, const uint8_t *data, const uint32_t size);
uint32_t proto_cmd_form(proto_cmd_t* cmd,
                          uint8_t *name, uint8_t *type, uint8_t *size,
                          comp_t *param,
                          uint8_t *b);
uint32_t proto_cmd_form_complete(proto_cmd_t* cmd, uint8_t *b);


#ifdef __cplusplus
}
#endif

#endif
