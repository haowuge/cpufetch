#include "soc.h"
#ifdef ARCH_ARM
  #include "../arm/socs.h"
#elif ARCH_RISCV
  #include "../riscv/socs.h"
#elif ARCH_LOONGARCH
  #include "../loongarch/socs.h"
#endif
#include "udev.h"
#include "../common/global.h"

#include <string.h>

static char* soc_trademark_string[] = {
  // ARM
  [SOC_VENDOR_SNAPDRAGON] = "Snapdragon ",
  [SOC_VENDOR_MEDIATEK]   = "MediaTek ",
  [SOC_VENDOR_EXYNOS]     = "Exynos ",
  [SOC_VENDOR_KIRIN]      = "Kirin ",
  [SOC_VENDOR_KUNPENG]    = "Kunpeng ",
  [SOC_VENDOR_BROADCOM]   = "Broadcom ",
  [SOC_VENDOR_APPLE]      = "Apple ",
  [SOC_VENDOR_ROCKCHIP]   = "Rockchip ",
  [SOC_VENDOR_GOOGLE]     = "Google ",
  [SOC_VENDOR_NVIDIA]     = "NVIDIA ",
  [SOC_VENDOR_AMPERE]     = "Ampere ",
  [SOC_VENDOR_NXP]        = "NXP ",
  [SOC_VENDOR_AMLOGIC]    = "Amlogic ",
  [SOC_VENDOR_MARVELL]    = "Marvell",
  // RISC-V
  [SOC_VENDOR_SIFIVE]     = "SiFive ",
  [SOC_VENDOR_STARFIVE]   = "StarFive ",
  [SOC_VENDOR_SIPEED]     = "Sipeed ",
  [SOC_VENDOR_SPACEMIT]   = "SpacemiT ",
  // ARM & RISC-V
  [SOC_VENDOR_ALLWINNER]  = "Allwinner ",
  // LOONGARCH
  [SOC_VENDOR_LOONGSON]   = ""
};

VENDOR get_soc_vendor_from_soc(SOC soc_model) {
  // ARM vendors
  if(soc_model >= 0 && soc_model <= 100) return SOC_VENDOR_SNAPDRAGON;  // Approximate range
  if(soc_model >= 101 && soc_model <= 200) return SOC_VENDOR_MEDIATEK;
  if(soc_model >= 201 && soc_model <= 300) return SOC_VENDOR_EXYNOS;
  if(soc_model >= 301 && soc_model <= 400) return SOC_VENDOR_KIRIN;
  if(soc_model >= 401 && soc_model <= 500) return SOC_VENDOR_KUNPENG;
  if(soc_model >= 501 && soc_model <= 600) return SOC_VENDOR_BROADCOM;
  if(soc_model >= 601 && soc_model <= 700) return SOC_VENDOR_APPLE;
  if(soc_model >= 701 && soc_model <= 800) return SOC_VENDOR_ROCKCHIP;
  if(soc_model >= 801 && soc_model <= 900) return SOC_VENDOR_GOOGLE;
  if(soc_model >= 901 && soc_model <= 1000) return SOC_VENDOR_NVIDIA;
  if(soc_model >= 1001 && soc_model <= 1100) return SOC_VENDOR_AMPERE;
  if(soc_model >= 1101 && soc_model <= 1200) return SOC_VENDOR_NXP;
  if(soc_model >= 1201 && soc_model <= 1300) return SOC_VENDOR_AMLOGIC;
  if(soc_model >= 1301 && soc_model <= 1400) return SOC_VENDOR_MARVELL;
  // RISC-V vendors
  if(soc_model >= 1401 && soc_model <= 1500) return SOC_VENDOR_SIFIVE;
  if(soc_model >= 1501 && soc_model <= 1600) return SOC_VENDOR_STARFIVE;
  if(soc_model >= 1601 && soc_model <= 1700) return SOC_VENDOR_SIPEED;
  if(soc_model >= 1701 && soc_model <= 1800) return SOC_VENDOR_SPACEMIT;
  // ARM & RISC-V
  if(soc_model >= 1801 && soc_model <= 1900) return SOC_VENDOR_ALLWINNER;
  // LOONGARCH
  if(soc_model >= 1901 && soc_model <= 2000) return SOC_VENDOR_LOONGSON;
  
  return SOC_VENDOR_UNKNOWN;
}

VENDOR get_soc_vendor(struct system_on_chip* soc) {
  return soc->vendor;
}

char* get_str_process(struct system_on_chip* soc) {
  char* str;

  if(soc->process == UNKNOWN) {
    str = emalloc(sizeof(char) * (strlen(STRING_UNKNOWN)+1));
    snprintf(str, strlen(STRING_UNKNOWN)+1, STRING_UNKNOWN);
  }
  else {
    int max_process_len = 5 + 1;
    str = ecalloc(max_process_len, sizeof(char));
    snprintf(str, max_process_len, "%dnm", soc->process);
  }
  return str;
}

char* get_soc_name(struct system_on_chip* soc) {
  if(soc->model == SOC_MODEL_UNKNOWN)
    return soc->raw_name;
  return soc->name;
}

void fill_soc(struct system_on_chip* soc, char* soc_name, SOC soc_model, int32_t process) {
  soc->model = soc_model;
  soc->vendor = get_soc_vendor_from_soc(soc_model);
  soc->process = process;
  if(soc->vendor == SOC_VENDOR_UNKNOWN) {
    printBug("fill_soc: soc->vendor == SOC_VENDOR_UNKOWN");
    // If we fall here there is a bug in socs.h
    // Reset everything to avoid segfault
    soc->vendor = SOC_VENDOR_UNKNOWN;
    soc->model = SOC_MODEL_UNKNOWN;
    soc->process = UNKNOWN;
    soc->raw_name = emalloc(sizeof(char) * (strlen(STRING_UNKNOWN)+1));
    snprintf(soc->raw_name, strlen(STRING_UNKNOWN)+1, STRING_UNKNOWN);
  }
  else {
    int len = strlen(soc_name) + strlen(soc_trademark_string[soc->vendor]) + 1;
    soc->name = emalloc(sizeof(char) * len);
    sprintf(soc->name, "%s%s", soc_trademark_string[soc->vendor], soc_name);
  }
}

void fill_soc_raw(struct system_on_chip* soc, char* soc_name, VENDOR vendor) {
  soc->model = SOC_MODEL_UNKNOWN;
  soc->vendor = vendor;
  soc->process = UNKNOWN;

  int len = strlen(soc_name) + strlen(soc_trademark_string[soc->vendor]) + 1;
  soc->raw_name = emalloc(sizeof(char) * len);
  sprintf(soc->raw_name, "%s%s", soc_trademark_string[soc->vendor], soc_name);
}

#ifdef _WIN32
VENDOR try_match_soc_vendor_name(char* vendor_name)
{
  for(size_t i=1; i < sizeof(soc_trademark_string)/sizeof(soc_trademark_string[0]); i++) {
    if(strstr(vendor_name, soc_trademark_string[i]) != NULL) {
      return i;
    }
  }
  return SOC_VENDOR_UNKNOWN;
}
#endif

bool match_soc(struct system_on_chip* soc, char* raw_name, char* expected_name, char* soc_name, SOC soc_model, int32_t process) {
  int len1 = strlen(raw_name);
  int len2 = strlen(expected_name);
  int len = min(len1, len2);

  if(strncmp(raw_name, expected_name, len) != 0) {
    return false;
  }
  else {
    fill_soc(soc, soc_name, soc_model, process);
    return true;
  }
}
