#!/usr/bin/env python

import sys
import os

def main():
    old_cwd = os.getcwd()

    build_path = f'{old_cwd}/build'
    target_path = f'{build_path}/bin'

    if os.path.isdir(build_path) == False:
        os.mkdir(build_path)

    os.chdir(build_path)
    # os.system('cmake ..')
    os.system('make -j 4')

    os.chdir(target_path)
    os.system(f'./dialectics')
    os.chdir(old_cwd)

if __name__ == '__main__':
    main()
