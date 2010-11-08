#include "sprecog.h"

void status_recready(Recog *recog, void *app_)
{
  Application *app = (Application *)app_;
  app->tell("recready");
}

void status_recstart(Recog *recog, void *app_)
{
  Application *app = (Application *)app_;
  app->tell("recstart");
}

void put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo)
{
  int i,j;
  WORD_ID w;
  static char buf[MAX_HMMNAME_LEN];

  if (seq != NULL) {
    for (i=0;i<n;i++) {
      if (i > 0) 
	fprintf(stderr, " |");
      w = seq[i];
      for (j=0;j<winfo->wlen[w];j++) {
	center_name(winfo->wseq[w][j]->name, buf);
	fprintf(stderr, " %s", buf);
      }
    }
  }
  fprintf(stderr, "\n");  
}

void output_result(Recog *recog, void *app_)
{
  Application *app = (Application *)app_;
  int i, j;
  int len;
  WORD_INFO *winfo;
  WORD_ID *seq;
  int seqnum;
  int n;
  Sentence *s;
  RecogProcess *r;
  HMM_Logical *p;
  SentenceAlign *align;
  boolean multi;

  if (recog->process_list->next != NULL) 
    multi = TRUE;
  else 
    multi = FALSE;

  /* all recognition results are stored at each recognition process
     instance */
  for(r=recog->process_list;r;r=r->next) {

    /* skip the process if the process is not alive */
    if (! r->live) continue;

    /* result are in r->result.  See recog.h for details */
    
    /* check result status */
    if (r->result.status < 0) {      /* no results obtained */
      /* outout message according to the status code */
      switch(r->result.status) {
      case J_RESULT_STATUS_REJECT_POWER:
	fprintf(stderr, "<input rejected by power>\n");
	break;
      case J_RESULT_STATUS_TERMINATE:
	fprintf(stderr, "<input teminated by request>\n");
	break;
      case J_RESULT_STATUS_ONLY_SILENCE:
	fprintf(stderr, "<input rejected by decoder (silence input result)>\n");
	break;
      case J_RESULT_STATUS_REJECT_GMM:
	fprintf(stderr, "<input rejected by GMM>\n");
	break;
      case J_RESULT_STATUS_REJECT_SHORT:
	fprintf(stderr, "<input rejected by short input>\n");
	break;
      case J_RESULT_STATUS_FAIL:
	fprintf(stderr, "<search failed>\n");
	break;
      }
      /* continue to next process instance */
      continue;
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "search id:%d name:%s\n", r->config->id, r->config->name);

    /* output results for all the obtained sentences */
    winfo = r->lm->winfo;

    for(n = 0; n < r->result.sentnum; n++) { /* for all sentences */

      s = &(r->result.sent[n]);
      seq = s->word;
      seqnum = s->word_num;

      /* output word sequence like Julius */
      fprintf(stderr, "sentence%d:", n+1);
      for(i=0;i<seqnum;i++) 
	fprintf(stderr, " %s", to_utf(winfo->woutput[seq[i]]));
      fprintf(stderr, "\n");

      if (n == 0 and seqnum == 3) {
#if 0
	const int size = 1000;
	char buf[size];
	sprintf(buf, "to @AM-MCL set Speak = %s", to_utf(winfo->woutput[seq[1]]));
	app->send(buf);
#endif
	app->onSpeechRecognized(to_utf(winfo->woutput[seq[1]]));
      }

      /* LM entry sequence */
      fprintf(stderr, "wseq%d:", n+1);
      for(i=0;i<seqnum;i++) 
	fprintf(stderr, " %s", to_utf(winfo->wname[seq[i]]));
      fprintf(stderr, "\n");
      /* phoneme sequence */
      fprintf(stderr, "phseq%d:", n+1);
      put_hypo_phoneme(seq, seqnum, winfo);
      //fprintf(stderr, "\n");
      /* confidence scores */
      fprintf(stderr, "cmscore%d:", n+1);
      for (i=0;i<seqnum; i++) 
	fprintf(stderr, " %5.3f", s->confidence[i]);
      fprintf(stderr, "\n");
      /* AM and LM scores */
      fprintf(stderr, "score%d: %f", n+1, s->score);
      if (r->lmtype == LM_PROB) { /* if this process uses N-gram */
	fprintf(stderr, " (AM: %f  LM: %f)", s->score_am, s->score_lm);
      }
      fprintf(stderr, "\n");
      if (r->lmtype == LM_DFA) { /* if this process uses DFA grammar */
	/* output which grammar the hypothesis belongs to
	   when using multiple grammars */
	if (multigram_get_all_num(r->lm) > 1) {
	  fprintf(stderr, "grammar%d: %d\n", n+1, s->gram_id);
	}
      }
    }
  }
  fflush(stderr);
}

/* based on julius/output_module.c */
void msock_word_out1(WORD_ID w, RecogProcess *r)
{
  int j;
  static char buf[MAX_HMMNAME_LEN];
  WORD_INFO *winfo;

  winfo = r->lm->winfo;

  fprintf(stderr, " WORD=\"%s\"", to_utf(winfo->woutput[w]));
  fprintf(stderr, " CLASSID=\"%s\"", winfo->wname[w]);
  fprintf(stderr, " PHONE=\"");
  for(j=0;j<winfo->wlen[w];j++) {
    /* libsent/src/hmminfo/cdhmm.c */ 
    center_name(winfo->wseq[w][j]->name, buf);
    if (j == 0) 
      fprintf(stderr, "%s", buf);
    else 
      fprintf(stderr, " %s", buf);
  }
  fprintf(stderr, "\"");
}

void result_pass1_current(Recog *recog, void *app_)
{
  Application *app = (Application *)app_;
  int i;
  WORD_INFO *winfo;
  WORD_ID *seq;
  int num;
  RecogProcess *r;
  boolean multi;

  if (recog->process_list->next != NULL) 
    multi = TRUE;
  else 
    multi = FALSE;

  for(r=recog->process_list;r;r=r->next) {
    if (! r->live) continue;
    if (! r->have_interim) continue;

    winfo = r->lm->winfo;
    seq = r->result.pass1.word;
    num = r->result.pass1.word_num;

    if (multi) {
      fprintf(stderr, "<RECOGOUT ID=\"SR%02d\" NAME=\"%s\">\n", r->config->id, r->config->name);
    } else {
      fprintf(stderr, "<RECOGOUT>\n");
    }
    fprintf(stderr, "  <PHYPO PASS=\"1\" SCORE=\"%f\" FRAME=\"%d\" TIME=\"%ld\">\n", r->result.pass1.score, r->result.num_frame, time(NULL));
    for (i=0;i<num;i++) {
      fprintf(stderr, "    <WHYPO");
      msock_word_out1(seq[i], r);
      fprintf(stderr, "/>\n");
    }
    fprintf(stderr, "  </PHYPO>\n</RECOGOUT>\n.\n");
  }
}

#if 0
void add_callbacks(Recog *recog, Application *app)
{
  callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, (void *)app);
  callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recstart, (void *)app);
  callback_add(recog, CALLBACK_RESULT, output_result, (void *)app);
  callback_add(recog, CALLBACK_RESULT_PASS1_INTERIM, result_pass1_current, (void *)app);
}
#else
void SpRecog::addCallbacks(Application *app)
{
  ::callback_add(this->recog, CALLBACK_EVENT_SPEECH_READY, status_recready, (void *)app);
  ::callback_add(this->recog, CALLBACK_EVENT_SPEECH_START, status_recstart, (void *)app);
  ::callback_add(this->recog, CALLBACK_RESULT, output_result, (void *)app);
  ::callback_add(this->recog, CALLBACK_RESULT_PASS1_INTERIM, result_pass1_current, (void *)app);
}
#endif

void SpRecog::openLogFile()
{
  this->srm_log_fp = fopen("_srm_log.txt", "w"); 
  jlog_set_output(this->srm_log_fp);
}

bool SpRecog::loadConfigFile(Application *app)
{
  char conf_file_name[] = "julius.conf";
  jconf = j_config_load_file_new(conf_file_name);
  if (jconf == NULL) {
    app->tell("error in j_config_load_file_new");
    fprintf(stderr, "error in j_config_load_file_new\n");
    return false;
  }
  recog = j_create_instance_from_jconf(jconf);
  if (recog == NULL) {
    app->tell("error in j_create_instance_from_jconf");
    return false;
  }
  return true;
}

bool SpRecog::start(Application *app)
{
  if (j_adin_init(this->recog) == FALSE) {    
    app->tell("error in j_adin_init");
    return false;
  }
  j_recog_info(this->recog); /* output system information to log */
  switch(j_open_stream(this->recog, NULL)) {
  case 0:			
    app->tell("ok j_open_stream");
    break;
  case -1:      		
    app->tell("error in input stream");
    return false;
  case -2:
    app->tell("error in beginning input");
    return false;
  }
  fflush(this->srm_log_fp);
  int ret = j_recognize_stream(this->recog);
  if (ret == -1) {
    app->tell("error j_recognize_stream");
    return false;
  }
  return true;
}

void SpRecog::close()
{
  j_close_stream(recog);
  j_recog_free(recog);
  // free(workorderp);
}
