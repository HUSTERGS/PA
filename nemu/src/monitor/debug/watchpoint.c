#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>
#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

// 在wp_pool中的节点，要么在head中，要么在free中，一开始全都在free中，所以其实不用管wp_pool，所有的操作基本上都在head和free之间进行
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP * new_wp(char * e) {
  // 如果还有空节点
  // 新的节点从链表的头部插入，可以避免对head的判断
  bool success = false;
  Log("新建的监视点的value为%s", e);
  uint32_t cur_val = expr(e, &success);
  if (!success) {
    Log("new_wp: 监视点对应的表达式有误，创建取消\n");
    return NULL;
  }
  if (free_) {
    // 如果之前有节点
    WP * temp = free_;
    free_ = free_->next;
    temp->next = head;
    head = temp;

    head->expr = (char *)malloc(sizeof(char) * (1 + strlen(e)));
    strcpyn(head->expr, e, (1+strlen(e)));
    head->prev_value = cur_val;
    head->just_init = true;
    return head;
    // if (head) {
    //   WP * temp = head;
    //   while (temp->next) {
    //     temp = temp->next;
    //   }
    //   temp->next = free_;
    //   free_ = free_->next;
    //   return temp->next;
    // } else {
    //   head = free_;
    //   free_ = free_->next;
    //   return head;
    // }
  }
  Log("new_wp: 监视点已满");
  return NULL;
}

void free_wp(WP *wp) {
  if (wp->expr) {
    free(wp->expr);
    wp->expr = NULL;
  }
  if (!head) {
    Log("free_wp: 重复释放监视点，操作已取消");
    return;
  }
  WP * temp = head;
  if (head == wp) {
    head = head->next;
    head->next = free_;
    free_ = temp;
    return;
  }
  WP * next = temp->next;
  while (next && next != wp) {
    temp = temp->next;
    next = next->next;
  }
  if (next == wp) {
    temp->next = next->next;
    next->next = free_;
    free_ = next;
    return;
  }
  Log("free_wp: 目标监视点不存在，操作已取消");
  return ;
}

void rm_wp(int NO) {
  if (NO >= 0 && NO < NR_WP) {
    free_wp(&wp_pool[NO]);
    return ;
  }
  Log("rm_wp: 目标监视点序号不存在");
  return ;
}

// 打印监视点相关信息
void print_wps() {
  WP * temp = head;
  while (temp) {
    printf("WP NO.%d \"%s\" value=%d  ", temp->NO, temp->expr, temp->prev_value);
    temp = temp->next;
  }
}
// 检查监视点对应的值是否发生变化
bool check_wps() {
  WP * temp = head;
  bool stop = false;
  bool success = false;
  while(temp) {
    uint32_t cur_val = expr(temp->expr, &success);
    Assert(success, "创建的时候应该就进行过判断，不应该会出错");
    if (success && cur_val != temp->prev_value) {
      temp->prev_value = cur_val;
      stop = true;
      printf("watchpoint No.%d value changed\n", temp->NO);
    }
    temp = temp->next;
  }
  return stop;
}

/* TODO: Implement the functionality of watchpoint */


