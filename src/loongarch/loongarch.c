#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../common/global.h"
#include "../common/udev.h"
#include "udev.h"
#include "uarch.h"
#include "soc.h"

#define SET_ISA_EXT_MAP(name, bit)          \
  if(strncmp(isa_ext, name, isa_ext_len) == 0) { \
    ext->mask[bit] = true;                  \
    maskset = true;                         \
  }                                         \

struct frequency* get_frequency_info(uint32_t core) {
  struct frequency* freq = emalloc(sizeof(struct frequency));

  freq->measured = false;
  freq->base = UNKNOWN_DATA;
  freq->max = get_max_freq_from_file(core);

  return freq;
}

int64_t get_peak_performance(struct cpuInfo* cpu) {
  //First check we have consistent data
  if(get_freq(cpu->freq) == UNKNOWN_DATA) {
    return -1;
  }

  int64_t flops = cpu->topo->total_cores * (get_freq(cpu->freq) * 1000000);
  return flops;
}

// Parse LoongArch ISA extensions from /proc/cpuinfo
// LoongArch extensions are separated by spaces in the "isa" field
int parse_extension(struct extensions* ext, char* e) {
  bool maskset = false;
  int isa_ext_len = strlen(e);
  char* isa_ext = e;
  
  // This should be up-to-date with LoongArch Architecture Reference Manual
  SET_ISA_EXT_MAP("cpucfg",    LOONGARCH_ISA_EXT_LAM)     // Using LAM slot for cpucfg
  SET_ISA_EXT_MAP("lam",      LOONGARCH_ISA_EXT_LAM)
  SET_ISA_EXT_MAP("ual",      LOONGARCH_ISA_EXT_UAL)
  SET_ISA_EXT_MAP("fpu",      LOONGARCH_ISA_EXT_FLOAT)
  SET_ISA_EXT_MAP("lsx",      LOONGARCH_ISA_EXT_LSX)
  SET_ISA_EXT_MAP("lasx",     LOONGARCH_ISA_EXT_LASX)
  SET_ISA_EXT_MAP("crc32",    LOONGARCH_ISA_EXT_CRC32)
  SET_ISA_EXT_MAP("lspw",     LOONGARCH_ISA_EXT_VIRT)    // Using VIRT slot for lspw
  SET_ISA_EXT_MAP("lvz",      LOONGARCH_ISA_EXT_VIRT)    // Using VIRT slot for lvz
  SET_ISA_EXT_MAP("lbt_x86",  LOONGARCH_ISA_EXT_LBT_X86)
  SET_ISA_EXT_MAP("lbt_arm",  LOONGARCH_ISA_EXT_LBT_ARM)
  SET_ISA_EXT_MAP("lbt_mips", LOONGARCH_ISA_EXT_LBT_MIPS)

  if(!maskset) {
    // Unknown extension, but not necessarily an error
    // Just skip it
    return -1;
  }

  return isa_ext_len;
}

struct extensions* get_extensions_from_str(char* str) {
  struct extensions* ext = emalloc(sizeof(struct extensions));
  ext->mask = ecalloc(LOONGARCH_ISA_EXT_ID_MAX, sizeof(bool));
  ext->str = NULL;

  if(str == NULL) {
    return ext;
  }

  int len = strlen(str)+1;
  ext->str = emalloc(len * sizeof(char));
  strncpy(ext->str, str, sizeof(char) * len);

  // Parse extensions from the isa string
  // Extensions are space-separated
  char* isa = str;
  char* saveptr = NULL;
  char* token = strtok_r(isa, " ", &saveptr);
  
  while(token != NULL) {
    parse_extension(ext, token);
    token = strtok_r(NULL, " ", &saveptr);
  }

  return ext;
}

uint32_t get_num_extensions(bool* mask) {
  uint32_t num = 0;
  for (int i=0; i < LOONGARCH_ISA_EXT_ID_MAX; i++) {
    if (mask[i]) num++;
  }
  return num;
}

bool is_mask_empty(bool* mask) {
  return get_num_extensions(mask) == 0;
}

struct cpuInfo* get_cpu_info(void) {
  struct cpuInfo* cpu = malloc(sizeof(struct cpuInfo));
  struct topology* topo = emalloc(sizeof(struct topology));
  topo->total_cores = get_ncores_from_cpuinfo();
  topo->cach = NULL;
  cpu->topo = topo;

  char* ext_str = get_extensions_from_cpuinfo();
  cpu->hv = emalloc(sizeof(struct hypervisor));
  cpu->hv->present = false;
  cpu->ext = get_extensions_from_str(ext_str);
  cpu->arch = get_uarch(cpu);
  cpu->soc = get_soc(cpu);
  cpu->freq = get_frequency_info(0);
  cpu->peak_performance = get_peak_performance(cpu);

  return cpu;
}

//TODO: Might be worth refactoring with other archs
char* get_str_topology(struct cpuInfo* cpu, struct topology* topo) {
  uint32_t size = 3+7+1;
  char* string = emalloc(sizeof(char)*size);
  snprintf(string, size, "%d cores", topo->total_cores);

  return string;
}

char* get_str_extensions(struct cpuInfo* cpu) {
  if(cpu->ext != NULL) {
    return cpu->ext->str;
  }
  return NULL;
}

void print_debug(struct cpuInfo* cpu) {
  printf("- soc: ");
  if(cpu->soc->raw_name == NULL) {
    printf("NULL\n");
  }
  else {
    printf("'%s'\n", cpu->soc->raw_name);
  }

  printf("- uarch: ");
  char* arch_cpuinfo_str = get_arch_cpuinfo_str(cpu);
  if(arch_cpuinfo_str == NULL) {
    printf("NULL\n");
  }
  else {
    printf("'%s'\n", arch_cpuinfo_str);
  }
}
