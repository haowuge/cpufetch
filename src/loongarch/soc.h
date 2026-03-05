#ifndef __SOC_LOONGARCH__
#define __SOC_LOONGARCH__

#include "../common/soc.h"

struct system_on_chip* get_soc(struct cpuInfo* cpu);
char* get_soc_name(struct system_on_chip* soc);
VENDOR get_soc_vendor(struct system_on_chip* soc);

#endif
