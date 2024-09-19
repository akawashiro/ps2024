#include <elf.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
  int hello_fd = open("./hello", O_RDONLY);
  size_t hello_size = lseek(hello_fd, 0, SEEK_END);
  size_t hello_mapped_size = (hello_size + 0xfff) & ~0xfff;
  char *hello_p =
      (char *)mmap(NULL, hello_mapped_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE, hello_fd, 0);

  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)hello_p;
  const int parasite_addr = 0x401030;
  ehdr->e_entry = parasite_addr;

  for (size_t pi = 0; pi < ehdr->e_phnum; pi++) {
    Elf64_Phdr *phdr = ((Elf64_Phdr *)(hello_p + ehdr->e_phoff)) + pi;
    if (phdr->p_type == PT_LOAD && phdr->p_flags == (PF_X | PF_R)) {
      phdr->p_filesz = 0x1000;
      phdr->p_memsz = 0x1000;
      break;
    }
  }

  const int parasite_dst_offset = 0x1030;

  int lifegame_fd = open("./lifegame", O_RDONLY);
  size_t lifegame_size = lseek(lifegame_fd, 0, SEEK_END);
  size_t lifegame_mapped_size = (lifegame_size + 0xfff) & ~0xfff;
  char *lifegame_p =
      (char *)mmap(NULL, lifegame_mapped_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE, lifegame_fd, 0);

  const int parasite_src_offset = 0x1149;
  const int parasite_size = 0x16f7 - 0x1149;

  for (int i = 0; i < parasite_size; i++) {
    if (0x1158 <= parasite_src_offset + i && parasite_src_offset + i < 0x1161) {
      // This part accessing fs:0x28.
      hello_p[parasite_dst_offset + i] = 0x90;
    } else {
      hello_p[parasite_dst_offset + i] =
          lifegame_p[parasite_src_offset + i];
    }
  }

  FILE *hello_with_parasite_fp =
      fopen("./hello_with_lifegame", "wb");
  fwrite(hello_p, hello_size, 1,
         hello_with_parasite_fp);
  chmod("./hello_with_lifegame", 0755);
  return 0;
}
