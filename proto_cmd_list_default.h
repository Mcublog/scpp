#ifdef ENABLE_DEFAULT_CMD_LIST

#ifndef CONSOLE_CMD_LIST_DEFAULT_H
#define CONSOLE_CMD_LIST_DEFAULT_H

typedef enum
{
    EMPTY = 0,
    Get_Name,
    //... add you code id ...//
    ID_LAST_CMD
} cmd_id_t; //command table

#endif //CONSOLE_CMD_LIST_H

#else
    #include "proto_cmd_list.h"
#endif // CONSOLE_CMD_LIST_DEFAULT_H
