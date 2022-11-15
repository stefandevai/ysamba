#!/usr/bin/env python

import argparse
import sys
import os

TARGET_NAME='dialectics'

parser = argparse.ArgumentParser(description=f'Utils for building and running {TARGET_NAME}.')
parser.add_argument('--run', '-r', action='store_true', help='run an already built binary')
parser.add_argument('--make', '-m', action='store_true', help='run make in the project')
parser.add_argument('--build', '-b', action='store_true', help='build dialectics')

def build(build_path):
    if os.path.isdir(build_path) == False:
        os.mkdir(build_path)
    os.chdir(build_path)
    os.system('cmake ..')

def make(build_path):
    os.chdir(build_path)
    os.system('make -j 4')

def run(target_path):
    os.chdir(target_path)
    os.system(f'./{TARGET_NAME}')

def main():
    args = parser.parse_args()
    old_cwd = os.getcwd()
    build_path = f'{old_cwd}/build'
    target_path = f'{build_path}/bin'

    if args.build:
        build(build_path)
        make(build_path)
    if args.make:
        make(build_path)
    if args.run:
        run(target_path)

    os.chdir(old_cwd)

if __name__ == '__main__':
    main()
