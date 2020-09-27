#!/usr/bin/env python3
import os
import glob

LICENSE_HEADER="""/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
"""

THISDIR=os.path.dirname(os.path.realpath(__file__))
ROOTDIR=os.path.join(THISDIR, "..")

SUFFICES = ["hpp", "cpp"]

def insert_license(path: str):
    with open(path, "r") as f:
        data = f.read()
    if not data.startswith(LICENSE_HEADER):
        with open(path, "w") as f:
            f.write(LICENSE_HEADER)
            f.write(data)

def main():
    files = []
    for suffix in SUFFICES:
        files += glob.glob(os.path.join(ROOTDIR, "**/*."+suffix), recursive=True)
    
    for f in files:
        insert_license(f)
    
if __name__ == "__main__":
    main()