#!/usr/bin/env python3

import ctypes
from ctypes import Structure, c_int16, c_uint16, c_uint32

from console.test_data import TEST_FLASH_DATA_ACK_ACC_MORE_SIZE


# yapf: disable
class time_stamp_t(Structure):
    _fields_ = [
        ('time',    c_uint32),
        ('t_ms',    c_uint16),
        ('day',     c_uint16),
        ('date',    c_uint16)
        ]


class acc_header_t(Structure):
    _fields_ = [
        ('t', time_stamp_t),
        ('freq', c_uint32),
        ('rezolution', c_uint32),
        ('max_size', c_uint32),
        ('acc_i', c_uint32)
        ]

class acc_report_t(Structure):
    _fields_ = [
        ('h', acc_header_t),
        ('acc_buf', c_uint32)
        ]

class mem_mngr_h_t(Structure):
    _fields_ = [
        ('type', c_uint16),
        ('dsize', c_uint16),
        ('part', c_uint16),
        ('crc32', c_uint32),
        ('acc_report', acc_report_t)
        ]


class AxesRaw_t(Structure):
    _fields_ = [
    ('AXIS_X', c_int16),
    ('AXIS_Y', c_int16),
    ('AXIS_Z', c_int16)]

    def __str__(self):
        return f'x: {self.AXIS_X:05d} y: {self.AXIS_Y:05d} z: {self.AXIS_Z:05d}'
# yapf: enable


def extract_binary_data(buffer: bytes) -> bytes:
    PREFIX_SIZE = len(b'[(\x08\x00\x00][GET_FLASH_DATA][bin][1][')
    END_SIZE = -9
    if len(buffer) < PREFIX_SIZE:
        return b''
    return buffer[PREFIX_SIZE:END_SIZE]


def main():
    # print(TEST_FLASH_DATA_ACK)
    data = extract_binary_data(TEST_FLASH_DATA_ACK_ACC_MORE_SIZE)
    # print(data)
    header = data[:ctypes.sizeof(mem_mngr_h_t)]
    h = mem_mngr_h_t.from_buffer_copy(header) # type: ignore
    report_buffer = data[ctypes.sizeof(mem_mngr_h_t):]
    # print(h)
    print(f'type: {h.type} part: {h.part} dsize: {h.dsize} crc32: {h.crc32:#x}')
    # print(report)
    for i in range(0, h.dsize * ctypes.sizeof(AxesRaw_t), ctypes.sizeof(AxesRaw_t)):
        report = AxesRaw_t.from_buffer_copy(report_buffer, i) # type: ignore
        print(f'nbr: {int(i/ctypes.sizeof(AxesRaw_t) + 1):04d} {report}')


if __name__ == '__main__':
    main()