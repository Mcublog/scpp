/**
 * @file crc_test.c
 * @author Viacheslav (slava.k@ks2corp.com)
 * @brief
 * @version 0.1
 * @date 2023-04-08
 *
 * @copyright KS2 Copyright (c) 2023
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "crc32.h"

int main(void)
{
    const uint32_t CORRECT_CRC32 = 0xb14257cc;
    const uint8_t buffer[] = {0x11, 0x22, 0x33, 0x44};
    uint32_t crc = xcrc32(buffer, sizeof(buffer), 0xFFFFFFFF);
    if (crc != CORRECT_CRC32)
    {
        printf("crc != CORRECT_CRC32: 0x%x != 0x%x\n", crc, CORRECT_CRC32);
        exit(1);
    }
    printf("crc: 0x%x is correct\n", crc);
}