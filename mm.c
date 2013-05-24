#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/system_properties.h>

#include "kallsyms.h"
#include "mm.h"

typedef struct _supported_device {
  const char *device;
  const char *build_id;
  unsigned long int remap_pfn_range_address;
} supported_device;

static supported_device supported_devices[] = {
  { "SH-04E", "01.00.02", 0xc00e458c },
};

static int n_supported_devices = sizeof(supported_devices) / sizeof(supported_devices[0]);

unsigned long int
_get_remap_pfn_range_address(void)
{
  int i;
  char device[PROP_VALUE_MAX];
  char build_id[PROP_VALUE_MAX];

  __system_property_get("ro.product.model", device);
  __system_property_get("ro.build.display.id", build_id);

  for (i = 0; i < n_supported_devices; i++) {
    if (!strcmp(device, supported_devices[i].device) &&
        !strcmp(build_id, supported_devices[i].build_id)) {
      return supported_devices[i].remap_pfn_range_address;
    }
  }

  printf("%s (%s) is not supported.\n", device, build_id);

  return 0;
}

void *
get_remap_pfn_range_address(void)
{
  if (kallsyms_exist()) {
    return kallsyms_get_symbol_address("remap_pfn_range_address");
  }
  return (void*)_get_remap_pfn_range_address();
}