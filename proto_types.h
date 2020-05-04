#ifndef PROTO_TYPES_H
#define PROTO_TYPES_H

#include <stdint.h>

#include "proto_cmd_list.h"

//Errors code
#define CLI_CRC_ERROR   (0)//CRC ERROR
#define CLI_OK          (1)//ALL OK
#define CLI_CORRUPT     (2)//Packet corrupt, size is not correct

typedef struct
{
    uint8_t* data;//pointer to start componets
    uint32_t size;//size of components
}comp_t;

typedef struct
{
    comp_t p[20];
}proto_param_t;

typedef struct
{
    uint32_t size_all;//size of all packet with service symbol
    comp_t   name;//Command name
    comp_t   type;//type string/bin
    comp_t   size;//number of paramters
    proto_param_t param;//parameters
}proto_cmd_t;

typedef struct
{
    cmd_id_t id;
    char* s;
    void (*func_cmd)(proto_cmd_t* cl_cmd, uint8_t *buf);
}cmd_t;

#endif //PROTO_TYPES_H
