#!/bin/bash

# variable
DIR=sp20140424_proj1

# code
make clean && \

mkdir $DIR && \

cp README.md $DIR/ && \
mv $DIR/README.md $DIR/README && \

cp *.c $DIR/ && \
cp *.h $DIR/ && \
cp *.docx $DIR/ && \
cp Makefile $DIR/  && \
cp opcode.txt $DIR/ && \

tar cvf $DIR.tar $DIR/ && \
rm -rf $DIR/ && \

echo done
