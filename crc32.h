#ifndef CRC32_H
#define CRC32_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define CRC32_LENGHT (4)    

unsigned int
xcrc32 (const unsigned char *buf, int len, unsigned int init);

#ifdef __cplusplus
}
#endif

#endif
