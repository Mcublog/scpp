from dataclasses import dataclass, field

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
    name: str = field(init=False, default='No_name')
    data_type: str = field(init=False, default='string')
    params: tuple = field(init=False, default_factory=tuple)

    def build(self) -> bytes:
        crc = build_int_param(0)
        cmd = build_string_param(self.name)
        cmd_type = build_string_param(self.data_type)
        param_number = build_string_param(str(len(self.params)))
        params = b''
        for p in self.params:
            params += build_string_param(p)
        size = len(build_int_param(0) + cmd + cmd_type + param_number + crc + END_OF_MSG)
        if params:
            size += len(params)
        msg = build_int_param(size) + cmd + cmd_type + param_number + params
        crc = Xcrc32.calc(msg)
        return msg + build_int_param(crc) + END_OF_MSG


@dataclass
class GetName(Request):
    name: str = "GET_NAME"


@dataclass
class GetFlashData(Request):
    name: str = field(init=False, default="GET_FLASH_DATA")

    Page: int = 0
    Block: int = 0
    Plane: int =0


    def __post_init__(self):
        self.params = (
            str(self.Page),
            str(self.Block),
            str(self.Plane)
        )


@dataclass
class GetConfig(Request):
    name: str = "GET_CFG"


@dataclass
class GetTime(Request):
    name: str = "GET_TIME"

@dataclass
class GetSensorData(Request):
    name: str = "GET_SENSOR_DATA"
