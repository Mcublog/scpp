#include "proto_data_parse.h"

#include <stdlib.h>
#include <string.h>

#include "crc32.h"

//-------------------------Types and definition-------------------------------
#define CL_START_SYM ('[')
#define CL_STOP_SYM (']')

#define CL_SIZE_START_STOP_SYMB (2)
#define CL_SIZE_END_SYMB (2)

#define CL_START_CMD_NAME_IDX (6) //start comand index; for 0 to 5 components with size of all packet

#define CRC32_OFFSET_IDX (7)   //offset for getting crc32
#define CRC32_OFFSET_COUNT (8) //size of crc32 (subtracted from packet size)

//[Size of all packet in bytes][Cmd_name][Type][Number of parameters][P1]..[P20]
//[Command components]
//----------------------------------------------------------------------------

//-----------------------Local variables and function-------------------------
static uint8_t *_get_cmd_comp_to_buf(uint8_t *comp, uint8_t *buf);
static uint8_t *_set_cmd_comp(uint8_t *s, comp_t *comp, uint8_t *b);
static uint8_t *_set_cmd_comp_bin(comp_t *comp,
    uint8_t *data, uint32_t size,
    uint8_t *b);
static uint32_t _get_cmd_param_to_buff(uint8_t *p1, uint8_t *p2, uint8_t *p3, uint8_t *p4, uint8_t *p5,
    uint8_t *p6, uint8_t *p7, uint8_t *p8, uint8_t *p9, uint8_t *p10,
    uint8_t *p11, uint8_t *p12, uint8_t *p13, uint8_t *p14, uint8_t *p15,
    uint8_t *p16, uint8_t *p17, uint8_t *p18, uint8_t *p19, uint8_t *p20,
    uint32_t num_param, uint8_t *param);
static uint32_t _get_cmd_param(proto_cmd_t *cl_cmd,
    uint32_t num_param, uint8_t *param);
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Check []
/param: Pointer to buff with cmd
/return: 1 if all ok
-----------------------------------------------------------*/
uint32_t proto_check_start_stop_symb(const uint8_t *buf)
{
    if (buf[0] == '[' && buf[5] == ']')
        return 1;
    return 0;
}

uint32_t buffncmp(uint8_t *b1, uint8_t *b2, uint32_t num)
{
    uint32_t i = 0;
    for (i = 0; i < num; i++)
    {
        if (b1[i] != b2[i])
            return 1;
    }
    return 0;
}

/*-----------------------------------------------------------
/brief: Get crc32 from buf
/param: Pointer to buff with cmd and data
/param: Size of cmd
/return: 1 if crc32 equal
-----------------------------------------------------------*/
uint32_t proto_cmd_check_crc32(const uint8_t *buf, uint32_t size, uint32_t *crc32)
{
    uint32_t crc_pkt = 0, crc = 0;
    uint32_t i = 0;

    for (i = 0; i < 4; i++)
    {
        crc_pkt |= (buf[size - CRC32_OFFSET_IDX + i] << 8 * i);
    }

    crc = xcrc32(buf, size - CRC32_OFFSET_COUNT, 0xFFFFFFFF);
    if (crc == crc_pkt)
        return 1;
    return 0;
}

/*-----------------------------------------------------------
/brief: Copy command component to buf
/param: Pointer to buff with cmd
/return: Pointer to start next component
-----------------------------------------------------------*/
static uint8_t *_get_cmd_comp_to_buf(uint8_t *comp, uint8_t *buf)
{
    uint8_t *s; //start symbol
    uint8_t *e; //end symbol
    uint32_t n; //number symbols to copy
    uint8_t *b = buf;
    s = (uint8_t *)strchr((const char *)buf, CL_START_SYM);
    e = (uint8_t *)strchr((const char *)buf, CL_STOP_SYM);
    n = e - s;
    strncpy((char *)comp, (const char *)s + 1, n - 1); //copy
    b += (n + 1);                                      //jump to next components
    return b;
}

/*-----------------------------------------------------------
/brief: Get components from components
/param: Pointer to componets
/param: Pointer to buff with cmd
/return: Pointer to start next component
-----------------------------------------------------------*/
static uint8_t *get_cmd_comp(comp_t *comp, uint8_t *buf)
{
    uint8_t *s; //ia?aeuiue neiaie
    uint8_t *e; //eiia?iue neiaie
    uint32_t n; //eie-ai neiaieia aey eiie?iaaiey
    uint8_t *b = buf;
    s = (uint8_t *)strchr((const char *)buf, CL_START_SYM);
    e = (uint8_t *)strchr((const char *)buf, CL_STOP_SYM);
    n = e - s;
    comp->size = n - 1;
    comp->data = s + 1;
    b += (n + 1); //ia?aoia e neaao?uaio eiiiiiaioo eiiaiau
    return b;
}

/*-----------------------------------------------------------
/brief: Set string components
/param: Pointer to string
/param: Pointer to componets
/param: Pointer to buff
/return: Pointer to start next component
-----------------------------------------------------------*/
static uint8_t *_set_cmd_comp(uint8_t *s, comp_t *comp, uint8_t *b)
{
    comp->size = strlen((char *)s);
    *b = CL_START_SYM;
    b++;
    comp->data = b;
    memcpy(b, s, comp->size);
    b += comp->size;
    *b = CL_STOP_SYM;
    b++;
    return (b++);
}

/*-----------------------------------------------------------
/brief: Set binary components
/param: Pointer to buf with data
/param: Size of data
/param: Pointer to buff
/return: Pointer to free place in buf
-----------------------------------------------------------*/
static uint8_t *_set_cmd_comp_bin(comp_t *comp,
    uint8_t *data, uint32_t size,
    uint8_t *b)
{
    comp->size = size;
    *b = CL_START_SYM;
    b++;
    comp->data = b;
    memcpy(b, data, comp->size);
    b += comp->size;
    *b = CL_STOP_SYM;
    b++;
    return (b++);
}

/*-----------------------------------------------------------
/brief: Get cmd parameters
/param: Pointer to buf with data
/param: Number of parametrs
/return: 1 if ok
-----------------------------------------------------------*/
uint32_t _get_cmd_param_to_buff(uint8_t *p1, uint8_t *p2, uint8_t *p3, uint8_t *p4, uint8_t *p5,
    uint8_t *p6, uint8_t *p7, uint8_t *p8, uint8_t *p9, uint8_t *p10,
    uint8_t *p11, uint8_t *p12, uint8_t *p13, uint8_t *p14, uint8_t *p15,
    uint8_t *p16, uint8_t *p17, uint8_t *p18, uint8_t *p19, uint8_t *p20,
    uint32_t num_param, uint8_t *param)
{
    uint32_t i = 0;
    for (i = 0; i < num_param; i++)
    {
        if (i == 0)
            param = _get_cmd_comp_to_buf(p1, param);
        else if (i == 1)
            param = _get_cmd_comp_to_buf(p2, param);
        else if (i == 2)
            param = _get_cmd_comp_to_buf(p3, param);
        else if (i == 3)
            param = _get_cmd_comp_to_buf(p4, param);
        else if (i == 4)
            param = _get_cmd_comp_to_buf(p5, param);
        else if (i == 5)
            param = _get_cmd_comp_to_buf(p6, param);
        else if (i == 6)
            param = _get_cmd_comp_to_buf(p7, param);
        else if (i == 7)
            param = _get_cmd_comp_to_buf(p8, param);
        else if (i == 8)
            param = _get_cmd_comp_to_buf(p9, param);
        else if (i == 9)
            param = _get_cmd_comp_to_buf(p10, param);
        else if (i == 10)
            param = _get_cmd_comp_to_buf(p11, param);
        else if (i == 11)
            param = _get_cmd_comp_to_buf(p12, param);
        else if (i == 12)
            param = _get_cmd_comp_to_buf(p13, param);
        else if (i == 13)
            param = _get_cmd_comp_to_buf(p14, param);
        else if (i == 14)
            param = _get_cmd_comp_to_buf(p15, param);
        else if (i == 15)
            param = _get_cmd_comp_to_buf(p16, param);
        else if (i == 16)
            param = _get_cmd_comp_to_buf(p17, param);
        else if (i == 17)
            param = _get_cmd_comp_to_buf(p18, param);
        else if (i == 18)
            param = _get_cmd_comp_to_buf(p19, param);
        else
            param = _get_cmd_comp_to_buf(p20, param);
    }
    return 1;
}

/*-----------------------------------------------------------
/brief: Get cmd parameters
/param: Pointer to buf with data
/param: Number of parametrs
/return: 1 if ok
-----------------------------------------------------------*/
static uint32_t _get_cmd_param(proto_cmd_t *cl_cmd,
    uint32_t num_param, uint8_t *param)
{
    uint32_t i = 0;
    for (i = 0; i < num_param; i++)
    {
        param = get_cmd_comp(&cl_cmd->param.p[i], param);
    }
    return 1;
}

/*-----------------------------------------------------------
/brief: Set size of all packet
/param: Pointer to buf with data and cmd
/return: Size of all packet
-----------------------------------------------------------*/
uint32_t proto_cmd_set_size_and_end(proto_cmd_t *cmd, uint8_t *b)
{
    uint32_t crc = 0;
    uint32_t crc_idx = 0;
    volatile uint32_t size_pkt = 0;
    uint32_t num = atoi((const char *)cmd->size.data);
    uint32_t size = cmd->name.size + CL_SIZE_START_STOP_SYMB;
    uint32_t i = 0;

    size = size + cmd->type.size + CL_SIZE_START_STOP_SYMB;
    size = size + cmd->size.size + CL_SIZE_START_STOP_SYMB;

    for (i = 0; i < num; i++)
    {
        size = size + cmd->param.p[i].size + CL_SIZE_START_STOP_SYMB;
    }

    size = size + CL_SIZE_START_STOP_SYMB + CL_SIZE_START_STOP_SYMB + CL_SIZE_END_SYMB;

    for (i = 0; i < CL_SIZE_START_STOP_SYMB + CRC32_LENGHT; i++)
    {
        if (i == 0)
            b[size + i] = CL_START_SYM;
        else if (i == 1)
        {
            b[size + i] = 0;
            crc_idx = size + i;
        }
        else if (i == 5)
            b[size + i] = CL_STOP_SYM;
        else
            b[size + i] = 0;
    }

    size = size + CL_SIZE_END_SYMB + CL_SIZE_START_STOP_SYMB + CRC32_LENGHT;
    b[0] = CL_START_SYM;
    *((uint32_t *)&b[1]) = size;
    b[5] = CL_STOP_SYM;

    *((uint32_t *)&b[crc_idx]) = 0;
    crc = xcrc32(b, size - CL_SIZE_START_STOP_SYMB - CL_SIZE_END_SYMB - CRC32_LENGHT, 0xffffffff); //crc without stop symol and themself
    *((uint32_t *)&b[crc_idx]) = crc;

    for (i = 0; i < CL_SIZE_END_SYMB; i++)
    {
        if (i == 0)
            b[size - 2] = '\r';
        else
            b[size - 1] = '\n';
    }

    cmd->size_all = size;
    return cmd->size_all;
}

/*-----------------------------------------------------------
/brief: Console command parse
/param: Pointer to buf with cmd
/return: 1 if ok
-----------------------------------------------------------*/
uint32_t proto_cmd_parse(const uint8_t *buf, proto_cmd_t *cl_cmd)
{
    uint32_t num = 0; //eie-ai ia?aiao?ia
    uint8_t *b = (uint8_t *)&buf[6];
    uint32_t size = proto_cmd_get_size(buf);
    uint32_t crc = 0;

    if (size != 0)
    {
        if (proto_cmd_check_crc32(buf, size, &crc))
        {
            cl_cmd->size_all = size;
            b = get_cmd_comp(&cl_cmd->name, b);
            b = get_cmd_comp(&cl_cmd->type, b);
            b = get_cmd_comp(&cl_cmd->size, b);

            num = atoi((const char *)cl_cmd->size.data);

            _get_cmd_param(cl_cmd, num, b);
            return CLI_OK;
        }
        else
            return CLI_CRC_ERROR;
    }

    return CLI_CORRUPT;
}

/*-----------------------------------------------------------
/brief: Get size of all packet
/param: Pointer to buf with cmd
/return: Return size of all packet
-----------------------------------------------------------*/
uint32_t proto_cmd_get_size(const uint8_t *buf)
{
    uint32_t size = 0;
    if (proto_check_start_stop_symb(buf))
    {
        size = *((uint32_t *)&buf[1]);
        if (size < 8192)
            return size;
    }
    return 0;
}

/*-----------------------------------------------------------
/brief: Set size of all packet
/param: Pointer to buf with cmd
/return: Return size of all packet
-----------------------------------------------------------*/
uint32_t proto_cmd_set_size(const uint8_t *buf)
{
    uint32_t size = 0;
    if (proto_check_start_stop_symb(buf))
    {
        size = (uint32_t)buf[1];
        if (size < 8192)
            return size;
    }
    return 0;
}

/*-----------------------------------------------------------
/brief: Parse cmd id
/param: Pointer to components
/param: Pointer to cmd_list
/return: Return cmd id
-----------------------------------------------------------*/
cmd_id_t get_cmd_id(comp_t *comp, const cmd_t *cmd_list)
{
    uint32_t i = 0;
    for (i = 0; i < ID_LAST_CMD; i++)
    {
        if (buffncmp(comp->data, (uint8_t *)cmd_list[i].s, comp->size) == 0)
        {
            return cmd_list[i].id;
        }
    }
    return EMPTY;
}

/*-----------------------------------------------------------
/brief: Component forming
/param: Pointer to components, string is not array
/return: Return cmd id
-----------------------------------------------------------*/
void proto_form_comp(comp_t *comp, const uint8_t *data)
{
    comp->data = (uint8_t *)data;
    comp->size = sizeof(data);
}

/*-----------------------------------------------------------
/brief: Create packet header (name, type, size)
/param: Pointer to strings name, type, size
/param: Pointer to command cmd
/return:
-----------------------------------------------------------*/
void proto_form_head(const char *name, const char *type, const char *size, proto_cmd_t *cl_cmd)
{
    proto_form_comp(&cl_cmd->name, (uint8_t *)name);
    proto_form_comp(&cl_cmd->type, (uint8_t *)type);
    proto_form_comp(&cl_cmd->size, (uint8_t *)size);
}

/*-----------------------------------------------------------
/brief: Create bin component
/param: Pointer to component
/param: Pointer to data (array or string)
/return:
-----------------------------------------------------------*/
void proto_form_comp_bin(comp_t *comp, const uint8_t *data, const uint32_t size)
{
    comp->data = (uint8_t *)data;
    comp->size = size;
}

/*-----------------------------------------------------------
/brief: Create command to tx
/param: Pointer to console command
/param: Pointer to strings name, type, size
/param: Array of parameters
/return: Size of all packet
-----------------------------------------------------------*/
uint32_t proto_cmd_form(proto_cmd_t *cmd,
    uint8_t *name, uint8_t *type, uint8_t *size,
    comp_t *param,
    uint8_t *b)
{

    uint32_t i = 0;
    uint32_t num = 0;
    uint8_t *p;

    p = &b[CL_START_CMD_NAME_IDX];

    p = _set_cmd_comp(name, &cmd->name, p);
    p = _set_cmd_comp(type, &cmd->type, p);
    p = _set_cmd_comp(size, &cmd->size, p);

    num = atoi((const char *)cmd->size.data);
    if (strncmp("string", (const char *)type, sizeof("string")) == 0)
    {
        for (i = 0; i < num; i++)
            p = _set_cmd_comp(param[i].data, &cmd->param.p[i], p);
    }
    else
    {
        for (i = 0; i < num; i++)
            p = _set_cmd_comp_bin(&cmd->param.p[i], param[i].data, param[i].size, p);
    }

    p = &b[0];
    proto_cmd_set_size_and_end(cmd, p);

    return 0;
}

/*-----------------------------------------------------------
/brief: Create comand to tx (all parametrs is ready)
/param: Pointer to console command
/param: Pointer to data
/return: Size of all packet
-----------------------------------------------------------*/
uint32_t proto_cmd_form_complete(proto_cmd_t *cmd, uint8_t *b)
{
    return proto_cmd_form(cmd,
        cmd->name.data, cmd->type.data, cmd->size.data,
        &cmd->param.p[0], b);
}