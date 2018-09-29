#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) {
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) {
	if(ready_queue!=NULL && result!=NULL){
		ProcessControlBlock_t current;
		int totalRunTime = 0;
		int waitTime=0;
		int numProcesses=0;
		int totalWallClockTime=0;
		while(!dyn_array_empty(ready_queue)){//ready_queue!=NULL){
			
		//for(int i=0; i<queueSize; i++){
			
			dyn_array_extract_back(ready_queue, (void *) &current);
			numProcesses++;
			waitTime+=totalRunTime;
			//Execute current Process until Burst time = 0
			if(current.remaining_burst_time>0 && !current.started){
				current.started=true;
				while(current.remaining_burst_time>0){
					//Actually running the Process Control Block, account for time & stats
					totalRunTime++;
					//waitTime += dyn_array_size(ready_queue);
					virtual_cpu(&current);
				}
				totalWallClockTime+=totalRunTime;
			}
		}
		result->total_run_time = (float)totalRunTime;
		result->average_latency_time = (float)waitTime/numProcesses;
		result->average_wall_clock_time =(float)totalWallClockTime/numProcesses;
		if(result->total_run_time>0)return true;
	}

	return false;

}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) {
	if(ready_queue!=NULL && result!=NULL){
			
	}
   return false;   
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) {
	if(ready_queue!=NULL && result!=NULL && quantum>0  /*Add case for quantum too large*/){
		
	}
    return false;
}

dyn_array_t *load_process_control_blocks(const char *input_file) {
    if(input_file!=NULL && *input_file!='\0'){
		
		int fileDesc = open(input_file, O_RDONLY);
		if(fileDesc<0){
			close(fileDesc);
			return NULL;
		}
		uint32_t numPCB;
		int fileSize = read(fileDesc, &numPCB, sizeof(uint32_t) );
		if(fileSize<=0) {
			close(fileDesc);
			return NULL;
		}
		ProcessControlBlock_t * pcb = malloc(numPCB * sizeof(ProcessControlBlock_t));
		uint32_t i=0;
		for(i=0; i<numPCB;i++){
			if(read(fileDesc,&(pcb[i].remaining_burst_time),sizeof(uint32_t))<=0) return NULL;
			if(read(fileDesc,&(pcb[i].priority),sizeof(uint32_t))<=0) return NULL;
		}
		
		dyn_array_t * array = dyn_array_import((void * )pcb, sizeof(numPCB), sizeof(ProcessControlBlock_t),NULL);
		free(pcb);
		close(fileDesc);
		return array;
	}
    return NULL;
}
