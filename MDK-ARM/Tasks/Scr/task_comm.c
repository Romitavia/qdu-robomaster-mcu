#include "task_comm.h"
#include "main.h"
#include "cmsis_os.h"

#include "bsp_io.h"

#include "tool_protocol.h"

#define COMM_TASK_FREQ_HZ (20)
#define COMM_TASK_STATUS_LED LED2

Protocol_PcInfoDownTypeDef pc_down_info;
Protocol_PcInfoUpTypeDef pc_up_info;
Protocol_JudgeInfoTypeDef judge_info;
Protocol_RemoteInfoTypeDef remote_info;

void CommTask(const void* argument) {
	uint32_t last_tick = osKernelSysTick();
	LED_Set(COMM_TASK_STATUS_LED, LED_ON);
	
	while(1) {
		
		LED_Set(COMM_TASK_STATUS_LED, LED_TAGGLE);
		osDelayUntil(&last_tick, (1000 / COMM_TASK_FREQ_HZ));
	}
}