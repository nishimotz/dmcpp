#ifndef _application_h_
#define _application_h_

#include <iostream>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <pthread.h>
#include <iconv.h>
#include <sys/timeb.h>

typedef struct workorder {
  void *dummy;
} workorder_t;

typedef struct {
  int dummy;
} thread_info_t;

#define ICONV_BUFSIZE 4096

void chomp(char *s);
bool starts_with(const char *str, const char *pat);
char *to_utf(char *src);
uint64_t get_milli_time();
void create_and_detach_thread(workorder_t *workorderp, void *(*worker)(void *));

class Application {
 public:
  thread_info_t m_thread_info;
  void send(const char *msg);
  void tell(const char *msg);
};

#endif
