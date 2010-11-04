/*
 * util.cpp 
 */

void chomp(char *s)
{
  char *p;
  while (s != NULL && (p = strrchr(s, '\n')) != NULL) {
    *p = '\0';
  }
}

bool starts_with(const char *str, const char *pat)
{
  if (strncmp(str, pat, strlen(pat)) == 0) {
    return true;
  }
  return false;
}

#include <iconv.h>
#define ICONV_BUFSIZE 4096

char *to_utf(char *src)
{
  char orig_buf[ICONV_BUFSIZE];
  static char buf[ICONV_BUFSIZE];
  strcpy(orig_buf, src);
  iconv_t m_iconv = iconv_open("UTF-8", "EUC-JP"); // tocode, fromcode
  size_t in_size = (size_t)ICONV_BUFSIZE;
  size_t out_size = (size_t)ICONV_BUFSIZE;
  char *in = orig_buf;
  char *out = buf;
  iconv(m_iconv, &in, &in_size, &out, &out_size);
  iconv_close(m_iconv);
  return buf;
}

#include <sys/timeb.h>

uint64_t get_milli_time()
{
  struct timeb t;
  ftime(&t);
  return (uint64_t)(t.time) * 1000 + t.millitm;
}
