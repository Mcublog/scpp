#!/usr/bin/env python3
import argparse
import logging
from dataclasses import dataclass
from typing import ClassVar

import serial
import serial.tools.list_ports
from colorama import Fore as Clr
from prompt_toolkit import PromptSession
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.patch_stdout import patch_stdout
from pylogus import logger_init

from console.commands import GetName
from console.version import VERSION
from console.xcrc32 import Xcrc32

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

    data = bytes([0x11, 0x22, 0x33, 0x44])
    log.info(f"0x{Xcrc32.calc(data):x}")
    log.info(f"Available ports:")
    print_port_list()
    log.info(f"-----------------")
    msg = GetName().build()
    log.info(f"tx: {msg}")

    port = 'COM3'
    with serial.Serial(port, timeout=0.5) as ser:
        ser.write(msg)
        rx = ser.read(8192)
        log.info(f"rx: {rx}")


if __name__ == '__main__':
    main()