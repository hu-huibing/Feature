#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int get_self_filename(char* filename, int len)
{
    int fd = open("/proc/self/cmdline", O_RDONLY);
    if (-1 == fd) {
        perror("open /proc/self/cmdline failed\n");
        return -1;
    }

    if (-1 == read(fd, filename, len - 1)) {
        perror("read /proc/self/cmdline failed\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

void out_elf_header(Elf64_Ehdr *elf_header)
{
    printf("-----------------ELF头--------------\n");
    printf("Magic:\t");
    for (int i = 0; i < EI_NIDENT; ++i) printf("%02x", elf_header->e_ident[i]);
    printf("\n文件类型:\t%hx\n", elf_header->e_type);
    printf("运行平台:\t%hx\n", elf_header->e_machine);
    printf("入口虚拟地址:\t0x%x\n", elf_header->e_entry);
    printf("程序头表文件偏移:\t%d(bytes)\n", elf_header->e_phoff);
    printf("节头表文件偏移:\t%d(bytes)\n", elf_header->e_shoff);
    printf("文件头大小:\t%d(bytes)\n", elf_header->e_ehsize);
    printf("程序头表条目大小:\t%d(bytes)\n", elf_header->e_phentsize);
    printf("程序头表条目数量:\t%d\n", elf_header->e_phnum);
    printf("节头表条目大小:\t%d(bytes)\n", elf_header->e_shentsize);
    printf("节头表条目数量:\t%d\n", elf_header->e_shnum);
    printf("字符串节头在节头表索引:\t%d\n", elf_header->e_shstrndx);    
}

void out_elf_section(Elf64_Shdr* elf_shdr, uint16_t shent_num, char* shstrtab)
{
    printf("\t\t\t\t--------------------------------Section表-----------------------------------\n");
    printf("[编号]\t名称\t\t    类型\t\t属性\t虚拟地址\t\t偏移量\t\t大小\t\t索引值\t信息\t对齐长度\t表项大小\n");
    for (int i = 0; i < shent_num; ++i) {
        printf("[%02d]\t%s", i, &shstrtab[elf_shdr[i].sh_name]);
        for (int j = 0; j < 20 - strlen(&shstrtab[elf_shdr[i].sh_name]); ++j) {
            putchar(' ');
        }
        printf("0x%08x\t\t", elf_shdr[i].sh_type);
        printf("0x%x\t", elf_shdr[i].sh_flags);
        printf("0x%016x\t", elf_shdr[i].sh_addr);
        printf("0x%08x\t", elf_shdr[i].sh_offset);
        printf("%4lu bytes\t", elf_shdr[i].sh_size);
        printf("%u\t", elf_shdr[i].sh_link);
        printf("%u\t", elf_shdr[i].sh_info);
        printf("%2lu bytes\t", elf_shdr[i].sh_addralign);
        printf("%4x\n", elf_shdr[i].sh_entsize);
    }
}

int parse_self_elf64()
{
    char filename[128] = {0};
    if (0 == get_self_filename(filename, sizeof(filename))) {
        printf("filename: %s\n", filename);
    } else {
        printf("get self filename failed\n");
        return -1;
    }

    int fd = open(filename, O_RDONLY);
    if (-1 == fd) {
        perror("open self failed\n");
        return -1;
    }

    Elf64_Ehdr elf_header;
    if (sizeof(Elf64_Ehdr) != read(fd, (void *)&elf_header, sizeof(Elf64_Ehdr))) {
        printf("read self failed\n");
        close(fd);
        return -1;
    }
    out_elf_header(&elf_header);

    uint32_t elf_shdr_size = elf_header.e_shnum * sizeof(Elf64_Shdr);
    Elf64_Shdr* elf_shdr = (Elf64_Shdr *)malloc(elf_shdr_size);
    if (NULL != elf_shdr) {
        lseek(fd, elf_header.e_shoff, SEEK_SET);
        if (elf_shdr_size == read(fd, (void *)elf_shdr, elf_shdr_size)) {
            lseek(fd, elf_shdr[elf_header.e_shstrndx].sh_offset, SEEK_SET);
            uint32_t sh_strtab_size = elf_shdr[elf_header.e_shstrndx].sh_size;
            char shstrtab[sh_strtab_size];
            if (sh_strtab_size == read(fd, (void *)shstrtab, sh_strtab_size)) {
                out_elf_section(elf_shdr, elf_header.e_shnum, shstrtab);
            }
        }
        free(elf_shdr);
    }
    close(fd);
    return 0;
}

int main(void)
{
    parse_self_elf64();
    return 0;
}