#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uarch.h"
#include "udev.h"
#include "../common/global.h"

typedef uint32_t MICROARCH;

struct uarch {
  MICROARCH uarch;
  char* uarch_str;
  char* cpuinfo_str;
  struct loongarch_cpuinfo* ci;
};

enum {
  UARCH_UNKNOWN,
  // LOONGSON - LA364 Core (3A5000/3B5000)
  UARCH_LA364,
  // LOONGSON - LA464 Core (3A6000/3B6000)
  UARCH_LA464,
  // LOONGSON - LA264 Core (2K2000)
  UARCH_LA264,
  // LOONGSON - LA364 Core (2K1000)
  UARCH_LA364_2K
};

#define UARCH_START if (false) {}
#define CHECK_UARCH(arch, cpu, cpuinfo_str, uarch_str, str, uarch, vendor) \
   else if (strcmp(cpuinfo_str, uarch_str) == 0) fill_uarch(arch, cpu, str, uarch, vendor);
#define UARCH_END else { printWarn("Unknown microarchitecture detected: uarch='%s'", cpuinfo_str); fill_uarch(arch, cpu, "Unknown", UARCH_UNKNOWN, CPU_VENDOR_UNKNOWN); }

void fill_uarch(struct uarch* arch, struct cpuInfo* cpu, char* str, MICROARCH u, VENDOR vendor) {
  arch->uarch = u;
  cpu->cpu_vendor = vendor;
  arch->uarch_str = emalloc(sizeof(char) * (strlen(str)+1));
  strcpy(arch->uarch_str, str);
}

// LoongArch microarchitectures reference:
// LA364: Used in 3A5000/3B5000 (LoongArch first generation)
// LA464: Used in 3A6000/3B6000 (LoongArch second generation)
// LA264: Used in 2K2000 (Embedded)
struct uarch* get_uarch_from_cpuinfo_str(char* cpuinfo_str, struct cpuInfo* cpu, struct uarch* arch) {
  arch->cpuinfo_str = cpuinfo_str;

  UARCH_START
  CHECK_UARCH(arch, cpu, cpuinfo_str, "Loongson-3A5000", "LA364", UARCH_LA364, CPU_VENDOR_LOONGSON)
  CHECK_UARCH(arch, cpu, cpuinfo_str, "Loongson-3A6000", "LA464", UARCH_LA464, CPU_VENDOR_LOONGSON)
  CHECK_UARCH(arch, cpu, cpuinfo_str, "Loongson-3B5000", "LA364", UARCH_LA364, CPU_VENDOR_LOONGSON)
  CHECK_UARCH(arch, cpu, cpuinfo_str, "Loongson-3B6000", "LA464", UARCH_LA464, CPU_VENDOR_LOONGSON)
  CHECK_UARCH(arch, cpu, cpuinfo_str, "Loongson-2K1000", "LA364", UARCH_LA364_2K, CPU_VENDOR_LOONGSON)
  CHECK_UARCH(arch, cpu, cpuinfo_str, "Loongson-2K2000", "LA264", UARCH_LA264, CPU_VENDOR_LOONGSON)
  UARCH_END

  return arch;
}

struct uarch* get_uarch(struct cpuInfo* cpu) {
  char* cpuinfo_str = get_uarch_from_cpuinfo();
  struct uarch* arch = emalloc(sizeof(struct uarch));
  arch->uarch = UARCH_UNKNOWN;
  arch->ci = NULL;

  if (cpuinfo_str == NULL) {
    printWarn("get_uarch_from_cpuinfo: Unable to detect microarchitecture using uarch: cpuinfo_str is NULL");
    arch->ci = get_loongarch_cpuinfo();

    if (arch->ci == NULL)
      printWarn("get_loongarch_cpuinfo: Unable to get cpuinfo from udev");
    else {
      // Use prid to detect microarchitecture if cpuinfo string is not available
      // PRID encoding: bits 23:16 contain processor ID
      unsigned int prid = arch->ci->prid;
      unsigned int proc_id = (prid >> 16) & 0xFF;
      
      // Loongson PRID values (approximate)
      if (proc_id >= 0x14 && proc_id < 0x20) {
        // LA364 core (3A5000/3B5000 series)
        fill_uarch(arch, cpu, "LA364", UARCH_LA364, CPU_VENDOR_LOONGSON);
      } else if (proc_id >= 0x20) {
        // LA464 core (3A6000/3B6000 series)
        fill_uarch(arch, cpu, "LA464", UARCH_LA464, CPU_VENDOR_LOONGSON);
      } else {
        printWarn("Unknown microarchitecture detected: prid=0x%.8X", prid);
        fill_uarch(arch, cpu, "Unknown", UARCH_UNKNOWN, CPU_VENDOR_UNKNOWN);
      }
    }
  }
  else {
    arch = get_uarch_from_cpuinfo_str(cpuinfo_str, cpu, arch);
  }

  if (arch->uarch == UARCH_UNKNOWN)
    fill_uarch(arch, cpu, "Unknown", UARCH_UNKNOWN, CPU_VENDOR_UNKNOWN);

  return arch;
}

char* get_str_uarch(struct cpuInfo* cpu) {
  return cpu->arch->uarch_str;
}

char* get_arch_cpuinfo_str(struct cpuInfo* cpu) {
  return cpu->arch->cpuinfo_str;
}

void free_uarch_struct(struct uarch* arch) {
  free(arch->uarch_str);
  free(arch);
}
