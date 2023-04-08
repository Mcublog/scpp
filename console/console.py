#!/usr/bin/env python3
import argparse
import logging

import serial
from colorama import Fore as Clr
from prompt_toolkit import PromptSession
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.patch_stdout import patch_stdout
from pylogus import logger_init
from version import VERSION

log = logger_init(__name__, logging.DEBUG)

DESCRIPTION = f'SCPP Console {Clr.GREEN}v{VERSION}{Clr.RESET}'


def main():
    parser = argparse.ArgumentParser(prog='console', description=f"{DESCRIPTION}")
    parser.add_argument(f'-p',
                        '--port',
                        type=str,
                        help=f'Target port name (/dev/ttyACM0)',
                        default='/dev/ttyACM0')

    log.info(DESCRIPTION)


if __name__ == '__main__':
    main()