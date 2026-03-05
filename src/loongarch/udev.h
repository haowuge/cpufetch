#ifndef __UDEV_LOONGARCH__
#define __UDEV_LOONGARCH__

#include "../common/udev.h"

#define UNKNOWN -1

// LoongArch CPU info structure
// Reference: Linux kernel arch/loongarch/kernel/cpu-probe.c
struct loongarch_cpuinfo {
  unsigned int cpuid;
  unsigned int prid;        // Processor Revision ID
  unsigned int config;      // Config register
  unsigned int config1;     // Config1 register
  unsigned int config2;     // Config2 register
};

char* get_hardware_from_devtree(void);
char* get_uarch_from_cpuinfo(void);
char* get_extensions_from_cpuinfo(void);
struct loongarch_cpuinfo *get_loongarch_cpuinfo(void);

#endif
