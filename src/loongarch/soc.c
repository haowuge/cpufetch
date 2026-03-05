#include "soc.h"
#include "socs.h"
#include "udev.h"
#include "../common/global.h"

#include <string.h>

#define SOC_START if (false) {}
#define SOC_END_LOONGARCH else { return false; }

bool match_loongson(char* soc_name, struct system_on_chip* soc) {
  char* tmp = soc_name;

  SOC_START
  SOC_EQ(tmp, "Loongson-3A5000", "Loongson 3A5000",  SOC_LOONGSON_3A5000, soc, 12)
  SOC_EQ(tmp, "Loongson-3A6000", "Loongson 3A6000",  SOC_LOONGSON_3A6000, soc, 12)
  SOC_EQ(tmp, "Loongson-3B5000", "Loongson 3B5000",  SOC_LOONGSON_3B5000, soc, 12)
  SOC_EQ(tmp, "Loongson-3B6000", "Loongson 3B6000",  SOC_LOONGSON_3B6000, soc, 12)
  SOC_EQ(tmp, "Loongson-2K1000", "Loongson 2K1000",  SOC_LOONGSON_2K1000, soc, 28)
  SOC_EQ(tmp, "Loongson-2K2000", "Loongson 2K2000",  SOC_LOONGSON_2K2000, soc, 12)
  SOC_END_LOONGARCH
}

struct system_on_chip* parse_soc_from_string(struct system_on_chip* soc) {
  char* raw_name = soc->raw_name;

  if(match_loongson(raw_name, soc))
    return soc;

  return soc;
}

struct system_on_chip* guess_soc_from_cpuinfo(struct system_on_chip* soc) {
  char* tmp = get_field_from_cpuinfo("Model Name\t\t: ");

  if(tmp != NULL) {
    soc->raw_name = tmp;
    return parse_soc_from_string(soc);
  }

  return soc;
}

struct system_on_chip* guess_soc_from_devtree(struct system_on_chip* soc) {
  char* tmp = get_hardware_from_devtree();

  if(tmp != NULL) {
    soc->raw_name = tmp;
    return parse_soc_from_string(soc);
  }

  return soc;
}

struct system_on_chip* get_soc(struct cpuInfo* cpu) {
  struct system_on_chip* soc = emalloc(sizeof(struct system_on_chip));
  soc->raw_name = NULL;
  soc->vendor = SOC_VENDOR_UNKNOWN;
  soc->model = SOC_MODEL_UNKNOWN;
  soc->process = UNKNOWN;

  soc = guess_soc_from_cpuinfo(soc);
  if(soc->vendor == SOC_VENDOR_UNKNOWN) {
    soc = guess_soc_from_devtree(soc);
  }
  if(soc->vendor == SOC_VENDOR_UNKNOWN) {
    if(soc->raw_name != NULL) {
      printWarn("SoC detection failed: Found '%s' string", soc->raw_name);
    }
    else {
      printWarn("SoC detection failed");
    }
  }

  if(soc->model == SOC_MODEL_UNKNOWN) {
    // raw_name might not be NULL, but if we were unable to find
    // the exact SoC, just print "Unknown"
    soc->raw_name = emalloc(sizeof(char) * (strlen(STRING_UNKNOWN)+1));
    snprintf(soc->raw_name, strlen(STRING_UNKNOWN)+1, STRING_UNKNOWN);
  }

  return soc;
}
