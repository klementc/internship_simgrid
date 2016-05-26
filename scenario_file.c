/*
 * Example derived from cloud-two-tasks.
 */

#include "simgrid/msg.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(msg_test, "Messages specific for this msg example");

/*msg_task_t atask = NULL;*/
// START INTERESTING PART
msg_elastic_task_t atask = NULL;
typedef struct {
  long second_of_change;
  int visits_per_second;
} ratio_visit_change;

ratio_visit_change visits_task(long second_of_change, int visits_per_second) {
  ratio_visit_change res;
  res.second_of_change = second_of_change;
  res.visits_per_second = visits_per_second;
  return res;
}
// END INTERESTING PART

static int computation_fun(int argc, char *argv[])
{
  const char *pr_name = MSG_process_get_name(MSG_process_self());
  const char *host_name = MSG_host_get_name(MSG_host_self());
  double clock_sta, clock_end;
  /*atask = MSG_task_create("Task1", 1e9, 1e9, NULL);*/
  // START INTERESTING PART
  // MSG_task_create (const char *name, double time_visit, ratio_visit_change visits_per_second[], double message_size, void *data)
  atask = MSG_elastic_task_create("Task1", 1e9, [visits_task(0, 10), visits_task(100, 25)], 1e9, NULL);
  // END INTERESTING PART
  clock_sta = MSG_get_clock();
  XBT_INFO("%s:%s task 1 created %g", host_name, pr_name, clock_sta);
  MSG_task_execute(atask);
  /*clock_end = MSG_get_clock();*/

  /*XBT_INFO("%s:%s task 1 executed %g", host_name, pr_name, clock_end - clock_sta);*/

  /*MSG_task_destroy(atask);*/
  atask = NULL;

  MSG_process_sleep(1);

  /*atask = MSG_task_create("Task2", 1e10, 1e10, NULL);*/
  // START INTERESTING PART
  // MSG_task_create (const char *name, double time_visit, FILE* requests_log, double message_size, void *data)
  atask = MSG_elastic_task_create("Task2", 1e10, MY_FILE, 1e10, NULL);
  // END INTERESTING PART

  clock_sta = MSG_get_clock();
  XBT_INFO("%s:%s task 2 created %g", host_name, pr_name, clock_sta);
  MSG_task_execute(atask);
  /*clock_end = MSG_get_clock();*/

  /*XBT_INFO("%s:%s task 2 executed %g", host_name, pr_name, clock_end - clock_sta);*/

  /*MSG_task_destroy(atask);*/
  atask = NULL;

  return 0;
}

static void launch_computation_worker(msg_host_t host)
{
  const char *pr_name = "compute";
  char **argv = xbt_new(char *, 2);
  argv[0] = xbt_strdup(pr_name);
  argv[1] = NULL;

  MSG_process_create_with_arguments(pr_name, computation_fun, NULL, host, 1, argv);
}

static int master_main(int argc, char *argv[])
{
  xbt_dynar_t hosts_dynar = MSG_hosts_as_dynar();
  msg_host_t pm0 = xbt_dynar_get_as(hosts_dynar, 0, msg_host_t);
  xbt_dynar_free(&hosts_dynar);
  msg_vm_t vm0;
  vm0 = MSG_vm_create_core(pm0, "VM0");
  MSG_vm_start(vm0);
  //MSG_process_sleep(1);

  launch_computation_worker(vm0);

  while(MSG_get_clock()<100) {
  if (atask != NULL)
    XBT_INFO("aTask remaining duration: %g", MSG_task_get_flops_amount(atask));
  MSG_process_sleep(1);
  }

  MSG_process_sleep(10000);
  MSG_vm_destroy(vm0);
  xbt_dynar_free(&hosts_dynar);
  return 1;
}

static void launch_master(msg_host_t host)
{
  const char *pr_name = "master_";
  char **argv = xbt_new(char *, 2);
  argv[0] = xbt_strdup(pr_name);
  argv[1] = NULL;

  MSG_process_create_with_arguments(pr_name, master_main, NULL, host, 1, argv);
}

int main(int argc, char *argv[]){
  MSG_init(&argc, argv);

  xbt_assert(argc == 2);
  MSG_create_environment(argv[1]);

  xbt_dynar_t hosts_dynar = MSG_hosts_as_dynar();
  launch_master(xbt_dynar_get_as(hosts_dynar, 0, msg_host_t));
  xbt_dynar_free(&hosts_dynar);

  int res = MSG_main();
  XBT_INFO("Bye (simulation time %g)", MSG_get_clock());
  xbt_dynar_free(&hosts_dynar);

  return !(res == MSG_OK);
}
