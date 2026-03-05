#ifndef __LOONGARCH__
#define __LOONGARCH__

#include "../common/cpu.h"

struct extension {
  int id;
  char* str;
};

// LoongArch extension IDs
// Reference: Linux kernel arch/loongarch/include/asm/cpucfg.h
// and /proc/cpuinfo output format
enum loongarch_isa_ext_id {
  LOONGARCH_ISA_EXT_BASE = 0,
  // Basic extensions from cpucfg
  LOONGARCH_ISA_EXT_LAM,      // LoongArch Multi-media Extensions
  LOONGARCH_ISA_EXT_UAL,      // Unified Assembly Language
  LOONGARCH_ISA_EXT_FLOAT,    // Floating Point Unit
  LOONGARCH_ISA_EXT_LSX,      // LoongArch SIMD eXtension (128-bit)
  LOONGARCH_ISA_EXT_LASX,     // LoongArch Advanced SIMD eXtension (256-bit)
  LOONGARCH_ISA_EXT_CRC32,    // CRC32 Instructions
  LOONGARCH_ISA_EXT_LBT,      // Binary Translation Extensions
  LOONGARCH_ISA_EXT_LBT_X86,  // X86 Binary Translation
  LOONGARCH_ISA_EXT_LBT_ARM,  // ARM Binary Translation
  LOONGARCH_ISA_EXT_LBT_MIPS, // MIPS Binary Translation
  LOONGARCH_ISA_EXT_VIRT,     // Virtualization Extensions
  LOONGARCH_ISA_EXT_CRYPTO,   // Crypto Extensions
  LOONGARCH_ISA_EXT_ID_MAX
};

// LoongArch extensions description
// Based on LoongArch Architecture Reference Manual
static const struct extension extension_list[] = {
  { LOONGARCH_ISA_EXT_LAM,      "(LAM) LoongArch Multi-media Extensions" },
  { LOONGARCH_ISA_EXT_UAL,      "(UAL) Unified Assembly Language" },
  { LOONGARCH_ISA_EXT_FLOAT,    "(FLOAT) Floating Point Unit" },
  { LOONGARCH_ISA_EXT_LSX,      "(LSX) LoongArch SIMD eXtension (128-bit)" },
  { LOONGARCH_ISA_EXT_LASX,     "(LASX) LoongArch Advanced SIMD eXtension (256-bit)" },
  { LOONGARCH_ISA_EXT_CRC32,    "(CRC32) Cyclic Redundancy Check Instructions" },
  { LOONGARCH_ISA_EXT_LBT,      "(LBT) LoongArch Binary Translation" },
  { LOONGARCH_ISA_EXT_LBT_X86,  "(LBT_X86) X86 Binary Translation" },
  { LOONGARCH_ISA_EXT_LBT_ARM,  "(LBT_ARM) ARM Binary Translation" },
  { LOONGARCH_ISA_EXT_LBT_MIPS, "(LBT_MIPS) MIPS Binary Translation" },
  { LOONGARCH_ISA_EXT_VIRT,     "(VIRT) Virtualization Extensions" },
  { LOONGARCH_ISA_EXT_CRYPTO,   "(CRYPTO) Crypto Extensions" },
};

struct cpuInfo* get_cpu_info(void);
char* get_str_topology(struct cpuInfo* cpu, struct topology* topo);
char* get_str_extensions(struct cpuInfo* cpu);
uint32_t get_num_extensions(bool* mask);
void print_debug(struct cpuInfo* cpu);

#endif
