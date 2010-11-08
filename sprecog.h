#ifndef _sprecog_h_
#define _sprecog_h_

// depends on : julius-local package 
#include <julius/juliuslib.h>
#include "application.h"

void status_recready(Recog *recog, void *app_);
void status_recstart(Recog *recog, void *app_);
void output_result(Recog *recog, void *app_);
void result_pass1_current(Recog *recog, void *app_);
void add_callbacks(Recog *recog, Application *app);

class SpRecog {
 public:
  workorder_t *workorderp;
  Jconf *jconf;
  Recog *recog;
  FILE *srm_log_fp; 

  void openLogFile();
  bool loadConfigFile(Application *app);
  void close();
};

#endif


