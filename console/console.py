#!/usr/bin/env python3
import argparse
import logging

import serial
from colorama import Fore as Clr
from crc import Calculator, Configuration
from prompt_toolkit import PromptSession
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.patch_stdout import patch_stdout
from pylogus import logger_init
from version import VERSION

log = logger_init(__name__, logging.DEBUG)

DESCRIPTION = f'SCPP Console {Clr.GREEN}v{VERSION}{Clr.RESET}'

xcrc32_config = Configuration(
    width=32,
    polynomial=0x04c11db7,
    init_value=0xFFFFFFFF,
    final_xor_value=0x00,
    reverse_input=False,
    reverse_output=False,
)
data = bytes([0x11, 0x22, 0x33, 0x44])
xcrc32 = Calculator(xcrc32_config)


def main():
    parser = argparse.ArgumentParser(prog='console', description=f"{DESCRIPTION}")
    parser.add_argument(f'-p',
                        '--port',
                        type=str,
                        help=f'Target port name (/dev/ttyACM0)',
                        default='/dev/ttyACM0')

    log.info(f"0x{xcrc32.checksum(data):x}")


if __name__ == '__main__':
    main()