# scpp

Simple communication protocol parser (SCPP)


## Scheme of proto

```bash
[Size][Name][Type][Number of parameters][Param]...[xcrc32]\r\n
```

Size -- binary number (uint32_t)
Name -- string
Type -- type of parameters string or bin,
for bin parameters allow only one parameter
Numer of parameters -- dec number as string
Param -- optional string param
xcrc32 -- binary number (uint32_t)
\r\n -- end of message bytes
