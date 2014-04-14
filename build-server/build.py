#!/usr/bin/env python

import sys

from build import build_graph

if __name__ == '__main__':
    if not build_graph.main():
        sys.exit(1)