#!/usr/bin/env python

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

TARGET_NAME='ysamba'
DATA_DIR_NAME='data'
THIS_DIR = Path(__file__).parent  # Directory of this script

parser = argparse.ArgumentParser(description=f'Utils for building and running {TARGET_NAME}.')
parser.add_argument('--run', '-r', action='store_true', help='run an already built binary')
parser.add_argument('--make', '-m', action='store_true', help='build the project')
parser.add_argument('--build', '-b', action='store_true', help='configure/build ysamba')
parser.add_argument('--format', '-f', action='store_true', help='format code')
parser.add_argument('--check', '-c', action='store', help='run static analysis on a file')

def configure(build_path: Path) -> None:
    subprocess.run(["cmake", "-B", build_path, "-S", THIS_DIR], check=True)

def build(build_path: Path) -> None:
    subprocess.run(['cmake', '--build', build_path], check=True)

def run(target_path: Path, data_path: Path) -> None:
    subprocess.run([target_path / TARGET_NAME], check=True, cwd=THIS_DIR)

def check(filename: Path) -> None:
    subprocess.run([
        "clang-tidy",
        "-p",
        "./build/compile_commands.json",
        "-header-filter=.*",
        filename
    ], check=True, cwd=THIS_DIR)

def format() -> None:
    source_dir = THIS_DIR / "src"
    source_files = source_dir.glob("**/*.[ch]pp")
    subprocess.run(["clang-format", "--verbose", "-i", "-style=file", *source_files], check=True)

def main() -> None:
    args = parser.parse_args()
    data_path = Path(THIS_DIR, DATA_DIR_NAME)
    build_path = Path(THIS_DIR, 'build')
    target_path = Path(build_path, 'bin')

    if args.build:
        configure(build_path)
        build(build_path)

    if args.make:
        build(build_path)

    if args.run:
        run(target_path, data_path)

    if args.format:
        format()

    if args.check:
        check(args.check)

    if len(sys.argv) < 2:
        configure(build_path)
        run(target_path, data_path)

if __name__ == '__main__':
    main()
