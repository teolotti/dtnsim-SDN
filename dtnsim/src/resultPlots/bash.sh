#!/bin/bash

# delete old generated files
rm -rf ../results/dtnsim.*.pdf

#generate new pdf files from .vec and .sca files
python3 main.py ../results ../results
