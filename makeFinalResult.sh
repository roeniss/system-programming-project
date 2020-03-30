#!/bin/bash

mkdir tmp
cd tmp && \
rm -rf * && \
cd .. && \
cp *.c ./tmp/ && \
cp *.h ./tmp/ && \
cp *.docx ./tmp/ && \
cp opcode.txt ./tmp/ && \
cp Makefile ./tmp/ && \
cp README.md ./tmp/README && \
cd tmp && \
make clean && make && make clean && tar cvf sp20140424_proj1.tar * && \
ls | grep -v sp20140424_proj1.tar | xargs rm 