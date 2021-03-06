#ifndef _application_h_
#define _application_h_

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
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
  FILE *dm_log_fp;
  bool agentSpeaking;
  std::string cvresultMsg;
  //
  void send(const char *msg);
  void tell(const char *msg);
  void onSpeechPass1Recognized(const char *word, double t);
  void onSpeechRecognized(const char *word);
  void agentSpeak(const char *sent);
  void iteration();
  void openLogFile();
  void closeLogFile();
  void setCvresultMsg(std::string s) { cvresultMsg = s; }
  std::string getCvresultMsg() { return cvresultMsg; }
  Application();
};

#endif
