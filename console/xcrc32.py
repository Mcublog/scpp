from typing import ClassVar

from crc import Calculator, Configuration


class Xcrc32:
    CALC_XCRC32: ClassVar = Calculator(
        Configuration(width=32,
                      polynomial=0x04c11db7,
                      init_value=0xFFFFFFFF,
                      final_xor_value=0x00,
                      reverse_input=False,
                      reverse_output=False))

    @staticmethod
    def calc(data: bytes) -> int:
        return Xcrc32.CALC_XCRC32.checksum(data)
