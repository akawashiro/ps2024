#include <elf.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
  int hello_fd = open("./hello", O_RDONLY);
  size_t hello_size = lseek(hello_fd, 0, SEEK_END);
  size_t hello_mapped_size = (hello_size + 0xfff) & ~0xfff;
  char *hello_p = (char *)mmap(NULL, hello_mapped_size, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE, hello_fd, 0);

  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)hello_p;
  const int parasite_addr = 0x401030;
  ehdr->e_entry = parasite_addr;

  for (size_t pi = 0; pi < ehdr->e_phnum; pi++) {
    Elf64_Phdr *phdr = ((Elf64_Phdr *)(hello_p + ehdr->e_phoff)) + pi;
    if (phdr->p_type == PT_LOAD) {
      phdr->p_filesz = 0x1000;
      phdr->p_memsz = 0x1000;
    }
  }

  const int parasite_dst_offset = 0x1030;

  int whitespace_to_embed_fd = open("./whitespace_to_embed", O_RDONLY);
  size_t whitespace_to_embed_size = lseek(whitespace_to_embed_fd, 0, SEEK_END);
  size_t whitespace_to_embed_mapped_size =
      (whitespace_to_embed_size + 0xfff) & ~0xfff;
  char *whitespace_to_embed_p = (char *)mmap(
      NULL, whitespace_to_embed_mapped_size, PROT_READ | PROT_WRITE,
      MAP_PRIVATE, whitespace_to_embed_fd, 0);

  const int parasite_src_offset = 0x1129;
  const int parasite_src_end = 0x1299;
  const int parasite_size = parasite_src_end - parasite_src_offset;

  for (int i = 0; i < parasite_size; i++) {
    hello_p[parasite_dst_offset + i] =
        whitespace_to_embed_p[parasite_src_offset + i];
  }

  int whitespace_program_fd =
      open("./whitespace_program_to_embed.ws", O_RDONLY);
  size_t whitespace_program_size = lseek(whitespace_program_fd, 0, SEEK_END);
  size_t whitespace_program_mapped_size =
      (whitespace_program_size + 0xfff) & ~0xfff;
  char *whitespace_program_p =
      (char *)mmap(NULL, whitespace_program_mapped_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE, whitespace_program_fd, 0);
  const int whitespace_program_dst_offset = 0x100;
  for (int i = 0; i < whitespace_program_size; i++) {
    hello_p[whitespace_program_dst_offset + i] = whitespace_program_p[i];
  }

  FILE *hello_with_parasite_fp = fopen("./hello_with_whitespace", "wb");
  fwrite(hello_p, hello_size, 1, hello_with_parasite_fp);
  chmod("./hello_with_whitespace", 0755);
  return 0;
}
