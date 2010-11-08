/*
 * application.cpp 
 */

#include "application.h"
#include <cstdlib>
#include <string>

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

uint64_t get_milli_time()
{
  struct timeb t;
  ftime(&t);
  return (uint64_t)(t.time) * 1000 + t.millitm;
}

void create_and_detach_thread(workorder_t *workorderp, void *(*worker)(void *))
{
  pthread_t *my_threadp;
  my_threadp = (pthread_t *) malloc(sizeof(pthread_t));
  pthread_create( my_threadp, NULL, worker, (void *) workorderp );
  pthread_detach( *my_threadp );
  free( my_threadp );
}

void Application::send(const char *msg)
{
  printf("%s\n", msg);
  fflush(stdout);
}

void Application::tell(const char *msg)
{
  printf("tell %s\n", msg);
  fflush(stdout);
}

void Application::agentSpeak(const char *sent)
{
  const int size = 1000;
  char buf[size];
  sprintf(buf, "to @AM-MCL set Speak = %s", sent);
  this->send(buf);
  this->agentSpeaking = true;
}

void Application::onSpeechRecognized(const char *word)
{
  std::string str;
  str = "あなたは";
  str += word;
  str += "と言いました。";
  this->agentSpeak(str.c_str());
}

void Application::openLogFile()
{
  this->dm_log_fp = fopen("_dm_log.txt", "w"); 
}

void Application::closeLogFile()
{
  fclose(this->dm_log_fp);
}

void Application::iteration()
{
  const int bufsize = 100000;
  static int n = 0;
  char ibuf[bufsize], obuf[bufsize];
  char *ret = fgets( ibuf, bufsize, stdin );
  chomp(ibuf);
  sprintf(obuf, "%d %s", n, ibuf);
  if (starts_with(ibuf, "From @SSM rep Speak.stat = IDLE")) { 
    fprintf(stderr, "%s\n", ibuf); fflush(stderr);
    this->agentSpeaking = false;
  }
  fprintf(this->dm_log_fp, "%s\n", obuf); 
  fflush(this->dm_log_fp);
  n++;
}

Application::Application()
{
  this->agentSpeaking = false;
}
