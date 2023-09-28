#!/usr/bin/env python3

import argparse
import logging

import serial
import serial.tools.list_ports
from colorama import Fore as Clr
from console.commands import GetFlashData, GetName
from console.logger_data_structures import AxesRaw_t, extract_binary_data, mem_mngr_h_t
from console.version import VERSION
from console.xcrc32 import Xcrc32
from pylogus import logger_init

log = logger_init(__name__, logging.DEBUG)

DESCRIPTION = f'SCPP Console {Clr.GREEN}v{VERSION}{Clr.RESET}'


def print_port_list() -> None:
    for port in serial.tools.list_ports.comports():
        print(port)


def main():
    parser = argparse.ArgumentParser(prog='console', description=f"{DESCRIPTION}")
    parser.add_argument(f'-p',
                        '--port',
                        type=str,
                        help=f'Target port name (/dev/ttyACM0)',
                        default='/dev/ttyACM0')

    args = parser.parse_args()
    data = bytes([0x11, 0x22, 0x33, 0x44])
    log.info(f"0x{Xcrc32.calc(data):x}")
    log.info(f"Available ports:")
    print_port_list()
    log.info(f"-----------------")
    msg = GetName().build()
    log.info(f"tx: {msg}")

    port = args.port

    with serial.Serial(port, timeout=0.5) as ser:
        for _ in range(10):
            # msg = GetFlashData(i, 0, 0).build()
            # log.info(f"tx: {msg}")
            ser.write(msg)
            if not (rx := ser.read(8192)):
                break
            log.info(f"rx: {rx}")
            # if not (data := extract_binary_data(rx)):
            #     continue
            # if len(data) < ctypes.sizeof(mem_mngr_h_t):
            #     log.info(f"rx: {rx}")
            #     continue
            # h = mem_mngr_h_t.from_buffer_copy(data)
            # # ACC report type
            # if h.type == 2:
            #     report_buffer = data[ctypes.sizeof(mem_mngr_h_t):]
            #     log.info(
            #         f'type: {h.type} part: {h.part} dsize: {h.dsize} crc32: {h.crc32:#x}')
            #     for i in range(0, h.dsize * ctypes.sizeof(AxesRaw_t),
            #                    ctypes.sizeof(AxesRaw_t)):
            #         report = AxesRaw_t.from_buffer_copy(report_buffer, i)
            #         log.info(f'nbr: {int(i/ctypes.sizeof(AxesRaw_t) + 1):04d} {report}')
            # log.info(f"rx: {rx}")


if __name__ == '__main__':
    main()
