from dataclasses import dataclass
from typing import ClassVar

from console.xcrc32 import Xcrc32

SSYMB = '['.encode('ascii')
ESYMB = ']'.encode('ascii')
END_OF_MSG = '\r\n'.encode('ascii')


def build_string_param(param: str) -> bytes:
    return SSYMB + param.encode('ascii') + ESYMB


def build_int_param(param: int) -> bytes:
    return SSYMB + param.to_bytes(4, 'little') + ESYMB


'''Scheme of proto
[size][NAME][type][Number of parameters][Param]...[xcrc32]\r\n
size -- binary number (uint32_t)
Name -- string
Numer of parameters -- dec number as string
Param -- optional string param
xcrc32 -- binary number (uint32_t)
\r\n -- end of message bytes
'''


@dataclass
class Request:
    NAME: ClassVar[str] = ''
    TYPE: ClassVar[str] = 'string'
    PARAMS: ClassVar[tuple] = ()

    def build(self) -> bytes:
        crc = build_int_param(0)
        cmd = build_string_param(self.NAME)
        cmd_type = build_string_param(self.TYPE)
        param_number = build_string_param(str(len(self.PARAMS)))
        params = b''
        for p in self.PARAMS:
            params += build_string_param(p)
        size = len(build_int_param(0) + cmd + cmd_type + param_number + crc + END_OF_MSG)
        if params:
            size += len(params)
        msg = build_int_param(size) + cmd + cmd_type + param_number + params
        crc = Xcrc32.calc(msg)
        return msg + build_int_param(crc) + END_OF_MSG


@dataclass
class GetName(Request):
    NAME: ClassVar[str] = "GET_NAME"


@dataclass
class GetFlashData(Request):
    NAME: ClassVar[str] = "GET_FLASH_DATA"

    Page: int
    Block: int
    Plane: int

    def __post_init__(self):
        self.PARAMS = tuple([  # type: ignore
            str(self.Page),
            str(self.Block),
            str(self.Page)
        ])
