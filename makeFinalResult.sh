#!/bin/bash

cd sp20140424_proj1  && \
rm -rf *  && \
cd ..  && \
cp *.c ./sp20140424_proj1/  && \
cp *.h ./sp20140424_proj1/  && \
cp *.docx ./sp20140424_proj1/  && \
cp opcode.txt ./sp20140424_proj1/  && \
cp Makefile ./sp20140424_proj1/  && \
cp README.md ./sp20140424_proj1/README  && \
cd sp20140424_proj1  && \
make clean && make && make clean && tar cvf sp20140424_proj1.tar * && \
ls | grep -v sp20140424_proj1.tar | xargs rm 