#!/usr/bin/env python

import argparse
import sys
import os
import shutil

TARGET_NAME='dialectics'
DATA_DIR_NAME='data'
SCRIPTS_DIR_NAME='scripts'

parser = argparse.ArgumentParser(description=f'Utils for building and running {TARGET_NAME}.')
parser.add_argument('--run', '-r', action='store_true', help='run an already built binary')
parser.add_argument('--make', '-m', action='store_true', help='run make in the project')
parser.add_argument('--build', '-b', action='store_true', help='build dialectics')

def build(build_path):
    if os.path.isdir(build_path) == False:
        os.mkdir(build_path)
    os.chdir(build_path)
    return os.system('cmake ..')

def make(build_path):
    os.chdir(build_path)
    return os.system('make -j 4')

def run(target_path, scripts_path):
    build_data_path = f'{target_path}/{DATA_DIR_NAME}'
    build_scripts_path = f'{build_data_path}/{SCRIPTS_DIR_NAME}'

    if not os.path.exists(build_data_path):
        os.makedirs(build_data_path)

    if os.path.exists(build_scripts_path):
        shutil.rmtree(build_scripts_path)

    shutil.copytree(scripts_path, build_scripts_path)
    os.chdir(target_path)
    os.system(f'./{TARGET_NAME}')

def main():
    args = parser.parse_args()
    old_cwd = os.getcwd()
    scripts_path = f'{old_cwd}/{DATA_DIR_NAME}/{SCRIPTS_DIR_NAME}'
    build_path = f'{old_cwd}/build'
    target_path = f'{build_path}/bin'

    if args.build:
        status = build(build_path)
        if status != 0:
            sys.exit()
        status = make(build_path)
        if status != 0:
            sys.exit()

    if args.make:
        status = make(build_path)
        if status != 0:
            sys.exit()
    if args.run:
        run(target_path, scripts_path)

    if len(sys.argv) < 2:
        status = build(build_path)
        if status != 0:
            sys.exit()
        run(target_path, scripts_path)

    os.chdir(old_cwd)

if __name__ == '__main__':
    main()
