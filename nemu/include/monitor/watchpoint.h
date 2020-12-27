#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

  char * expr;
  uint32_t prev_value;
  bool just_init;
} WP;
void print_wps();
WP * new_wp(char * expr);
void rm_wp(int NO);
bool check_wps();
#endif
