/*
 * adinpost.cpp for dmcpp
 */
#include <julius/juliuslib.h>
#include <sys/timeb.h>

int
get_plugin_info(int opcode, char *buf, int buflen)
{
  switch(opcode) {
  case 0:
    strncpy(buf, "adinpost for dmcpp", buflen);
    break;
  }
  return 0;
}

int
adin_postprocess(SP16 *buf, int sampnum)
// or adin_postprocess_triggered()
{
  struct timeb timeb_data;
  ftime(&timeb_data);
  return(sampnum);
}
