/**
 * dm.cpp
 * by Takuya Nishimoto
 * based on julius-simple.c (julius 4.1.5) by Akinobu Lee
 */

/* include top Julius library header */
extern "C" {
#include <julius/juliuslib.h>
}

#include <time.h>

static void 
tell(const char *msg)
{
  printf("tell %s\n", msg);
  fflush(stdout);
}

static void
status_recready(Recog *recog, void *dummy)
{
  tell("recready");
}

static void
status_recstart(Recog *recog, void *dummy)
{
  tell("recstart");
}

static void
put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo)
{
  int i,j;
  WORD_ID w;
  static char buf[MAX_HMMNAME_LEN];

  if (seq != NULL) {
    for (i=0;i<n;i++) {
      if (i > 0) printf(" |");
      w = seq[i];
      for (j=0;j<winfo->wlen[w];j++) {
	center_name(winfo->wseq[w][j]->name, buf);
	printf(" %s", buf);
      }
    }
  }
  printf("\n");  
}

static void
output_result(Recog *recog, void *dummy)
{
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
	printf("<input rejected by power>\n");
	break;
      case J_RESULT_STATUS_TERMINATE:
	printf("<input teminated by request>\n");
	break;
      case J_RESULT_STATUS_ONLY_SILENCE:
	printf("<input rejected by decoder (silence input result)>\n");
	break;
      case J_RESULT_STATUS_REJECT_GMM:
	printf("<input rejected by GMM>\n");
	break;
      case J_RESULT_STATUS_REJECT_SHORT:
	printf("<input rejected by short input>\n");
	break;
      case J_RESULT_STATUS_FAIL:
	printf("<search failed>\n");
	break;
      }
      /* continue to next process instance */
      continue;
    }

    /* output results for all the obtained sentences */
    winfo = r->lm->winfo;

    for(n = 0; n < r->result.sentnum; n++) { /* for all sentences */

      s = &(r->result.sent[n]);
      seq = s->word;
      seqnum = s->word_num;

      /* output word sequence like Julius */
      printf("sentence%d:", n+1);
      for(i=0;i<seqnum;i++) printf(" %s", winfo->woutput[seq[i]]);
      printf("\n");
      /* LM entry sequence */
      printf("wseq%d:", n+1);
      for(i=0;i<seqnum;i++) printf(" %s", winfo->wname[seq[i]]);
      printf("\n");
      /* phoneme sequence */
      printf("phseq%d:", n+1);
      put_hypo_phoneme(seq, seqnum, winfo);
      printf("\n");
      /* confidence scores */
      printf("cmscore%d:", n+1);
      for (i=0;i<seqnum; i++) printf(" %5.3f", s->confidence[i]);
      printf("\n");
      /* AM and LM scores */
      printf("score%d: %f", n+1, s->score);
      if (r->lmtype == LM_PROB) { /* if this process uses N-gram */
	printf(" (AM: %f  LM: %f)", s->score_am, s->score_lm);
      }
      printf("\n");
      if (r->lmtype == LM_DFA) { /* if this process uses DFA grammar */
	/* output which grammar the hypothesis belongs to
	   when using multiple grammars */
	if (multigram_get_all_num(r->lm) > 1) {
	  printf("grammar%d: %d\n", n+1, s->gram_id);
	}
      }
      
      /* output alignment result if exist */
      for (align = s->align; align; align = align->next) {
	printf("=== begin forced alignment ===\n");
	switch(align->unittype) {
	case PER_WORD:
	  printf("-- word alignment --\n"); break;
	case PER_PHONEME:
	  printf("-- phoneme alignment --\n"); break;
	case PER_STATE:
	  printf("-- state alignment --\n"); break;
	}
	printf(" id: from  to    n_score    unit\n");
	printf(" ----------------------------------------\n");
	for(i=0;i<align->num;i++) {
	  printf("[%4d %4d]  %f  ", align->begin_frame[i], align->end_frame[i], align->avgscore[i]);
	  switch(align->unittype) {
	  case PER_WORD:
	    printf("%s\t[%s]\n", winfo->wname[align->w[i]], winfo->woutput[align->w[i]]);
	    break;
	  case PER_PHONEME:
	    p = align->ph[i];
	    if (p->is_pseudo) {
	      printf("{%s}\n", p->name);
	    } else if (strmatch(p->name, p->body.defined->name)) {
	      printf("%s\n", p->name);
	    } else {
	      printf("%s[%s]\n", p->name, p->body.defined->name);
	    }
	    break;
	  case PER_STATE:
	    p = align->ph[i];
	    if (p->is_pseudo) {
	      printf("{%s}", p->name);
	    } else if (strmatch(p->name, p->body.defined->name)) {
	      printf("%s", p->name);
	    } else {
	      printf("%s[%s]", p->name, p->body.defined->name);
	    }
	    if (r->am->hmminfo->multipath) {
	      if (align->is_iwsp[i]) {
		printf(" #%d (sp)\n", align->loc[i]);
	      } else {
		printf(" #%d\n", align->loc[i]);
	      }
	    } else {
	      printf(" #%d\n", align->loc[i]);
	    }
	    break;
	  }
	}
	
	printf("re-computed AM score: %f\n", align->allscore);

	printf("=== end forced alignment ===\n");
      }
    }
  }
  fflush(stdout);
}

/* based on julius/output_module.c */
static void
msock_word_out1(WORD_ID w, RecogProcess *r)
{
  int j;
  static char buf[MAX_HMMNAME_LEN];
  WORD_INFO *winfo;

  winfo = r->lm->winfo;

  printf(" WORD=\"%s\"", winfo->woutput[w]);
  printf(" CLASSID=\"%s\"", winfo->wname[w]);
  printf(" PHONE=\"");
    for(j=0;j<winfo->wlen[w];j++) {
      /* libsent/src/hmminfo/cdhmm.c */ 
      center_name(winfo->wseq[w][j]->name, buf);
      if (j == 0) printf("%s", buf);
      else printf(" %s", buf);
    }
    printf("\"");
}

static void
result_pass1_current(Recog *recog, void *dummy)
{
  int i;
  WORD_INFO *winfo;
  WORD_ID *seq;
  int num;
  RecogProcess *r;
  boolean multi;

  if (recog->process_list->next != NULL) multi = TRUE;
  else multi = FALSE;

  for(r=recog->process_list;r;r=r->next) {
    if (! r->live) continue;
    if (! r->have_interim) continue;

    winfo = r->lm->winfo;
    seq = r->result.pass1.word;
    num = r->result.pass1.word_num;

    if (multi) {
      printf("<RECOGOUT ID=\"SR%02d\" NAME=\"%s\">\n", r->config->id, r->config->name);
    } else {
      printf("<RECOGOUT>\n");
    }
    printf("  <PHYPO PASS=\"1\" SCORE=\"%f\" FRAME=\"%d\" TIME=\"%ld\"/>\n", r->result.pass1.score, r->result.num_frame, time(NULL));
    for (i=0;i<num;i++) {
      printf("    <WHYPO");
      msock_word_out1(seq[i], r);
      printf("/>\n");
    }
    printf("  </PHYPO>\n</RECOGOUT>\n.\n");
  }
}

int
main(int argc, char *argv[])
{
  Jconf *jconf; // Julius configuration parameter holder
  Recog *recog; // Julius recognition instance

  FILE *srm_log_fp; 
  srm_log_fp = fopen("srm_log.txt", "w"); 
  jlog_set_output(srm_log_fp);

  char conf_file_name[] = "julius.conf";
  jconf = j_config_load_file_new(conf_file_name);
  if (jconf == NULL) {
    tell("error in j_config_load_file_new");
    return -1;
  }
  
  recog = j_create_instance_from_jconf(jconf);
  if (recog == NULL) {
    tell("error in j_create_instance_from_jconf");
    return -1;
  }

  callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, NULL);
  callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recstart, NULL);
  callback_add(recog, CALLBACK_RESULT, output_result, NULL);
  callback_add(recog, CALLBACK_RESULT_PASS1_INTERIM, result_pass1_current, NULL);

  if (j_adin_init(recog) == FALSE) {    
    tell("error in j_adin_init");
    return -1;
  }

  j_recog_info(recog); /* output system information to log */

  switch(j_open_stream(recog, NULL)) {
  case 0:			
    tell("ok j_open_stream");
    break;
  case -1:      		
    tell("error in input stream");
    return(-1);
  case -2:
    tell("error in beginning input");
    return(-2);
  }
    
  int ret = j_recognize_stream(recog);
  if (ret == -1) {
    tell("error j_recognize_stream");
    return -1;
  }

  j_close_stream(recog);
  j_recog_free(recog);
  return(0);
}
