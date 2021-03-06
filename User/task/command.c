/*
  命令接收任务。
  
  接收机器人的控制指令。
  
  从DR16中接收数据，转换为通用的CMD_RC_t控制信号。
  根据CMD_RC_t计算最终命令CMD_t。
  把计算好的CMD_t细分后放到对应的消息队列中。
  超时未收到则认为是丢控等特殊情况，把CMD_RC_t中的内容置零，
  在后续的CMD_Parse中会根据此内容发现错误，保证机器人不失控。
*/

/* Includes ----------------------------------------------------------------- */
#include <string.h>

#include "device\dr16.h"
#include "task\user_task.h"

/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
#ifdef DEBUG
DR16_t dr16;
CMD_RC_t rc;
CMD_t cmd;
#else
static DR16_t dr16;
static CMD_RC_t rc;
static CMD_t cmd;
#endif

/* Private function --------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
void Task_Command(void *argument) {
  Task_Param_t *task_param = (Task_Param_t *)argument;

  task_param->msgq.cmd.chassis =
      osMessageQueueNew(9u, sizeof(CMD_Chassis_Ctrl_t), NULL);
  task_param->msgq.cmd.gimbal =
      osMessageQueueNew(9u, sizeof(CMD_Gimbal_Ctrl_t), NULL);
  task_param->msgq.cmd.shoot =
      osMessageQueueNew(9u, sizeof(CMD_Shoot_Ctrl_t), NULL);

  /* Task Setup */
  osDelay(TASK_INIT_DELAY_COMMAND);

  DR16_Init(&dr16);
  CMD_Init(&cmd, &(task_param->config_pilot->param.cmd));

  while (1) {
#ifdef DEBUG
    task_param->stack_water_mark.command = osThreadGetStackSpace(NULL);
#endif
    /* Task body */
    DR16_StartDmaRecv(&dr16);

    if (DR16_WaitDmaCplt(200)) {
      DR16_ParseRC(&dr16, &rc);
    } else {
      memset(&rc, 0, sizeof(CMD_RC_t));
    }

    CMD_Parse(&rc, &cmd);
    osMessageQueuePut(task_param->msgq.cmd.chassis, &(cmd.chassis), 0, 0);
    osMessageQueuePut(task_param->msgq.cmd.gimbal, &(cmd.gimbal), 0, 0);
    osMessageQueuePut(task_param->msgq.cmd.shoot, &(cmd.shoot), 0, 0);
  }
}
