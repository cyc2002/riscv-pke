#ifndef _ELF_H_
#define _ELF_H_

#include "util/types.h"
#include "process.h"

#define MAX_CMDLINE_ARGS 64

// elf header structure
typedef struct elf_header_t {
  uint32 magic;
  uint8 elf[12];
  uint16 type;      /* Object file type */
  uint16 machine;   /* Architecture */
  uint32 version;   /* Object file version */
  uint64 entry;     /* Entry point virtual address */
  uint64 phoff;     /* Program header table file offset */
  uint64 shoff;     /* Section header table file offset */
  uint32 flags;     /* Processor-specific flags */
  uint16 ehsize;    /* ELF header size in bytes */
  uint16 phentsize; /* Program header table entry size */
  uint16 phnum;     /* Program header table entry count */
  uint16 shentsize; /* Section header table entry size */
  uint16 shnum;     /* Section header table entry count */
  uint16 shstrndx;  /* Section header string table index */
} elf_header;


// Program segment header.
typedef struct elf_prog_header_t {
  uint32 type;   /* Segment type */
  uint32 flags;  /* Segment flags */
  uint64 off;    /* Segment file offset */
  uint64 vaddr;  /* Segment virtual address */
  uint64 paddr;  /* Segment physical address */
  uint64 filesz; /* Segment size in file */
  uint64 memsz;  /* Segment size in memory */
  uint64 align;  /* Segment alignment */
} elf_prog_header;

// elf section header structure
typedef struct section_header_t {
  uint32 name;      /* Section name (string table index) */
  uint32 type;      /* Section type */
  uint64 flags;     /* Section flags */
  uint64 addr;      /* Section virtual address */
  uint64 offset;    /* Section file offset */
  uint64 size;      /* Section size in bytes */
  uint32 link;      /* Link to another section */
  uint32 info;      /* Additional section information */
  uint64 addralign; /* Section alignment */
  uint64 entsize;   /* Entry size if section holds table */
} section_header;

// elf symbol table
typedef struct symbol_table_t {
  uint32 name;      /* Symbol name (string table index) */
  uint8 info;       /* Symbol type and binding attributes */
  uint8 reserved;   /* Reserved */
  uint16 shndx;     /* Section table index */
  uint64 value;     /* Symbol value */
  uint64 size;      /* Symbol size */
} symbol_table;

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian
#define ELF_PROG_LOAD 1

typedef enum elf_status_t {
  EL_OK = 0,

  EL_EIO,
  EL_ENOMEM,
  EL_NOTELF,
  EL_ERR,

} elf_status;

typedef struct elf_ctx_t {
  void *info;
  elf_header ehdr;
} elf_ctx;

elf_ctx elfloader;
section_header *section_table;
// uint64 elf_fpread(elf_ctx *ctx, void *dest, uint64 nb, uint64 offset);
elf_status elf_init(elf_ctx *ctx, void *info);
elf_status elf_load(elf_ctx *ctx);
char* strtab;
symbol_table *symtab;
void load_bincode_from_host_elf(process *p);

#endif
