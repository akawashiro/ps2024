FROM ubuntu:22.04
COPY . /work
WORKDIR /work
RUN apt-get update
RUN apt-get -y install nasm gcc-11 python3 xxd

RUN nasm -f elf64 ./hello.asm
RUN ld -o hello ./hello.o

RUN gcc-11 -o lifegame ./lifegame.c
RUN gcc-11 -o ./embed_lifegame_in_hello ./embed_lifegame_in_hello.c
RUN ./embed_lifegame_in_hello

# RUN ./hello_with_lifegame

RUN python3 ./emit_hello_whitespace.py > ./whitespace_program_to_embed.ws
RUN xxd -include ./whitespace_program_to_embed.ws > ./whitespace_program_to_embed.h
RUN gcc-11 -fno-stack-protector -o ./whitespace_to_embed ./whitespace_to_embed.c

RUN gcc-11 -o ./embed_whitespace_in_hello ./embed_whitespace_in_hello.c
RUN ./embed_whitespace_in_hello

# RUN ./hello_with_whitespace
