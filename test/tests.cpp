#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"
// Using a C library requires extern "C" to prevent function managling
extern "C" {
#include <dyn_array.h>
#include <processing_scheduling.h>
}


#define NUM_PCB 30
#define QUANTUM 4 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;

class GradeEnvironment : public testing::Environment {
    public:
        virtual void SetUp() {
            score = 0;
            total = 160;
        }
        virtual void TearDown() {
            ::testing::Test::RecordProperty("points_given", score);
            ::testing::Test::RecordProperty("points_total", total);
            std::cout << "SCORE: " << score << '/' << total << std::endl;
        }
};

/*
 * * ROUND ROBIN TEST CASES
 * */

TEST (round_robin, nullInputProcessControlBlockDynArray) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = round_robin (pcbs,sr,QUANTUM);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;

}

TEST (round_robin, nullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = round_robin (pcbs,sr,QUANTUM);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;

}

TEST (round_robin, goodInputA) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[3] = {
        [0] = {24,0,0,0},
        [1] = {3,0,0,0},
        [2] = {3,0,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = round_robin (pcbs,sr,QUANTUM);	
    ASSERT_EQ(true,res);
    float answers[3] = {15.666667,5.666667,30};
    ASSERT_FLOAT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_FLOAT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=10;
}

TEST (round_robin, goodInputB) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBS now
    ProcessControlBlock_t data[3] = {
        [0] = {20,0,0,0},
        [1] = {5,0,0,0},
        [2] = {6,0,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);		
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = round_robin (pcbs,sr,QUANTUM);	
    ASSERT_EQ(true,res);
    float answers[3] = {22.333334,12,31};
    ASSERT_FLOAT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=10;
}
/********** Additional Test Cases: Round Robin********/ 
TEST (round_robin, GoodInputSmallQuantum) {
	ScheduleResult_t *sr = new ScheduleResult_t;
	dyn_array_t* pcbs = dyn_array_create(0, sizeof(ProcessControlBlock_t),NULL);
	memset(sr,0,sizeof(ScheduleResult_t));
	ProcessControlBlock_t data[5] = {
       		[0] = {20,0,0,0},
        	[1] = {5,0,0,0},
        	[2] = {6,0,0,0},
			[3] = {4,0,0,0},
			[4] = {5,1,0,0}
    	};

	dyn_array_push_back(pcbs,&data[4]);	
	dyn_array_push_back(pcbs,&data[3]);
   	dyn_array_push_back(pcbs,&data[2]);
   	dyn_array_push_back(pcbs,&data[1]);
   	dyn_array_push_back(pcbs,&data[0]);
   	ASSERT_EQ(true,round_robin (pcbs,sr,1));
	dyn_array_destroy(pcbs);
	delete sr;
	score+=5;
}

TEST (round_robin, badInputZeroBurstTime) {
        ScheduleResult_t *sr = new ScheduleResult_t;
        dyn_array_t* pcbs = dyn_array_create(0, sizeof(ProcessControlBlock_t),NULL);
        memset(sr,0,sizeof(ScheduleResult_t));
        ProcessControlBlock_t data[3] = {
                [0] = {0,0,1,0},
                [1] = {0,0,0,0},
                [2] = {6,0,1,0}
        };

        dyn_array_push_back(pcbs,&data[2]);
        dyn_array_push_back(pcbs,&data[1]);
        dyn_array_push_back(pcbs,&data[0]);
        ASSERT_EQ(false,round_robin (pcbs,sr,QUANTUM));
		ASSERT_EQ(0,sr->average_wall_clock_time);
		ASSERT_EQ(0,sr->average_latency_time);
		ASSERT_EQ(0,sr->total_run_time);
	dyn_array_destroy(pcbs);
	delete sr;
	score+=5;
}

TEST (round_robin, TooLargeQuantum) {
        ScheduleResult_t *sr = new ScheduleResult_t;
        dyn_array_t* pcbs = dyn_array_create(0, sizeof(ProcessControlBlock_t),NULL);
        memset(sr,0,sizeof(ScheduleResult_t));
        ProcessControlBlock_t data[3] = {
                [0] = {20,0,0,0},
                [1] = {5,0,0,0},
                [2] = {6,0,0,0}
        };

        dyn_array_push_back(pcbs,&data[2]);
        dyn_array_push_back(pcbs,&data[1]);
        dyn_array_push_back(pcbs,&data[0]);
        ASSERT_EQ(false,round_robin (pcbs,sr,10000000000000000));
	dyn_array_destroy(pcbs);
	delete sr;
	score+=5;
}

TEST (round_robin, pcbBINFile){
	ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBS now
    ProcessControlBlock_t data[30] = {
        [0] = {14,0,0,0},
        [1] = {2,0,0,0},
        [2] = {13,0,0,0},
		[3] = {11,0,0,0},
        [4] = {9,0,0,0},
        [5] = {11,0,0,0},
		[6] = {2,0,0,0},
        [7] = {13,0,0,0},
        [8] = {10,0,0,0},
		[9] = {2,0,0,0},
        [10] = {3,0,0,0},
        [11] = {8,0,0,0},
		[12] = {6,0,0,0},
        [13] = {5,0,0,0},
        [14] = {9,0,0,0},
		[15] = {2,0,0,0},
        [16] = {1,0,0,0},
        [17] = {7,0,0,0},
		[18] = {8,0,0,0},
        [19] = {2,0,0,0},
        [20] = {12,0,0,0},
		[21] = {9,0,0,0},
        [22] = {13,0,0,0},
        [23] = {10,0,0,0},
		[24] = {3,0,0,0},
        [25] = {6,0,0,0},
        [26] = {3,0,0,0},
		[27] = {14,0,0,0},
        [28] = {8,0,0,0},
        [29] = {11,0,0,0},
    };
    // back loading dyn_array, pull from the back
	dyn_array_push_back(pcbs,&data[29]);
    dyn_array_push_back(pcbs,&data[28]);		
    dyn_array_push_back(pcbs,&data[27]);
	dyn_array_push_back(pcbs,&data[26]);
    dyn_array_push_back(pcbs,&data[25]);		
    dyn_array_push_back(pcbs,&data[24]);
	dyn_array_push_back(pcbs,&data[23]);
    dyn_array_push_back(pcbs,&data[22]);		
    dyn_array_push_back(pcbs,&data[21]);
	dyn_array_push_back(pcbs,&data[20]);
    dyn_array_push_back(pcbs,&data[19]);		
    dyn_array_push_back(pcbs,&data[18]);
	dyn_array_push_back(pcbs,&data[17]);
    dyn_array_push_back(pcbs,&data[16]);		
    dyn_array_push_back(pcbs,&data[15]);
	dyn_array_push_back(pcbs,&data[14]);
    dyn_array_push_back(pcbs,&data[13]);		
    dyn_array_push_back(pcbs,&data[12]);
	dyn_array_push_back(pcbs,&data[11]);
    dyn_array_push_back(pcbs,&data[10]);		
    dyn_array_push_back(pcbs,&data[9]);
	dyn_array_push_back(pcbs,&data[8]);
    dyn_array_push_back(pcbs,&data[7]);		
    dyn_array_push_back(pcbs,&data[6]);
	dyn_array_push_back(pcbs,&data[5]);
    dyn_array_push_back(pcbs,&data[4]);		
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);		
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = round_robin (pcbs,sr,QUANTUM);	
    ASSERT_EQ(true,res);
	/**/
    float answers[3] = {148.533340,140.966660,227};
    ASSERT_FLOAT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
	/*
	printf("\n%f", sr->average_wall_clock_time);
	printf("\n%lu", sr->total_run_time);
	printf("\n%f", sr->average_latency_time);
	*/
    dyn_array_destroy(pcbs);
    delete sr;

    score+=10;
	
}

/*********************************************/

/*
 *  * * Priority  TEST CASES
 *   * */

TEST (priority, nullInputProcessControlBlockDynArray) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = priority (pcbs,sr);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;
}

TEST (priority, nullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = priority (pcbs,sr);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;
}
TEST (priority, badInputAllFinishedPCBS) {
   	ScheduleResult_t *sr = new ScheduleResult_t;
   	dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
   	memset(sr,0,sizeof(ScheduleResult_t));
	ProcessControlBlock_t data[4]={
		[0] = {15,8,1,0},
		[1] = {10,1, 1,0},
		[2] = {3, 3, 1,0},
		[3] = {2, 2, 1,0}
	};
	dyn_array_push_back(pcbs, &data[3]);
	dyn_array_push_back(pcbs, &data[2]);
	dyn_array_push_back(pcbs, &data[1]);
	dyn_array_push_back(pcbs, &data[0]);
	ASSERT_EQ(false, priority(pcbs,sr));
	ASSERT_EQ(0,sr->average_wall_clock_time);
	ASSERT_EQ(0,sr->average_latency_time);
	ASSERT_EQ(0,sr->total_run_time);
	delete sr;
	dyn_array_destroy(pcbs);
	score +=5;
}

TEST (priority, badInputZeroBurstTime) {
        ScheduleResult_t *sr = new ScheduleResult_t;
        dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
        memset(sr,0,sizeof(ScheduleResult_t));
        ProcessControlBlock_t data[2]={
                [0] = {0,0,0,0},
				[1] = {0,0,0,0}
        };
        dyn_array_push_back(pcbs, &data[0]);
		dyn_array_push_back(pcbs, &data[1]);
        ASSERT_EQ(false, priority(pcbs,sr));
		ASSERT_EQ(0,sr->average_wall_clock_time);
		ASSERT_EQ(0,sr->average_latency_time);
		ASSERT_EQ(0,sr->total_run_time);
        delete sr;
        dyn_array_destroy(pcbs);
        score +=5;
}

TEST (priority, goodInputNonZeroResults) {
        ScheduleResult_t *sr = new ScheduleResult_t;
        dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
        memset(sr,0,sizeof(ScheduleResult_t));
        ProcessControlBlock_t data[4]={
                [0] = {6,8,0,0},
                [1] = {4,1, 0,0},
                [2] = {10, 3, 0,0},
                [3] = {2, 2, 0,0}
        };
        dyn_array_push_back(pcbs, &data[3]);
        dyn_array_push_back(pcbs, &data[2]);
        dyn_array_push_back(pcbs, &data[1]);
        dyn_array_push_back(pcbs, &data[0]);
        ASSERT_EQ(true, priority(pcbs,sr));
		ASSERT_NE(0,sr->average_wall_clock_time);
		ASSERT_NE(0,sr->average_latency_time);
		ASSERT_NE(0,sr->total_run_time);
        delete sr;
        dyn_array_destroy(pcbs);
        score +=5;
}



TEST (priority, goodInputA) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[3] = {
        [0] = {24,3,0,0},
        [1] = {3,1,0,0},
        [2] = {3,2,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);
    bool res = priority (pcbs,sr);
    ASSERT_EQ(true,res);
    float answers[3] = {13,3,30};
    ASSERT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}

TEST (priority, goodInputB) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[5] = {
        [0] = {10,3,0,0},
        [1] = {1,1,0,0},
        [2] = {2,4,0,0},
        [3] = {1,5,0,0},
        [4] = {5,2,0,0},
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[4]);
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);
    bool res = priority (pcbs,sr);
    ASSERT_EQ(true,res);
    float answers[3] = {12,8.2,19};
    ASSERT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}


/*
 * * First Come First Serve  TEST CASES
 * */

TEST (first_come_first_serve, nullInputProcessControlBlockDynArray) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = NULL;
    bool res = first_come_first_serve (pcbs,sr);
    ASSERT_EQ(false,res);
    delete sr;

    score+=5;
}

TEST (first_come_first_serve, nullScheduleResult) {
    ScheduleResult_t *sr = NULL;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    bool res = first_come_first_serve (pcbs,sr);
    ASSERT_EQ(false,res);
    dyn_array_destroy(pcbs);

    score+=5;
}

TEST (first_come_first_serve, goodInputA) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[3] = {
        [0] = {24,0,0,0},
        [1] = {3,0,0,0},
        [2] = {3,0,0,0}
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = first_come_first_serve (pcbs,sr);	
    ASSERT_EQ(true,res);
    float answers[3] = {27,17,30};
    ASSERT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}

TEST (first_come_first_serve, goodInputB) {
    ScheduleResult_t *sr = new ScheduleResult_t;
    dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    memset(sr,0,sizeof(ScheduleResult_t));
    // add PCBs now
    ProcessControlBlock_t data[4] = {
        [0] = {6,0,0,0},
        [1] = {8,0,0,0},
        [2] = {7,0,0,0},
        [3] = {3,0,0,0},
    };
    // back loading dyn_array, pull from the back
    dyn_array_push_back(pcbs,&data[3]);
    dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);		
    dyn_array_push_back(pcbs,&data[0]);	
    bool res = first_come_first_serve (pcbs,sr);	
    ASSERT_EQ(true,res);
    float answers[3] = {16.25,10.25,24};
    ASSERT_EQ(answers[0],sr->average_wall_clock_time);
    ASSERT_EQ(answers[1],sr->average_latency_time);
    ASSERT_EQ(answers[2],sr->total_run_time);
    dyn_array_destroy(pcbs);
    delete sr;

    score+=20;
}
/*************** Additional Test Cases: First Come First Serve ************/
TEST (first_come_first_serve, goodInputNonZeroResults){
	ScheduleResult_t *sr = new ScheduleResult_t;
	dyn_array_t* pcbs = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
	memset(sr,0,sizeof(ScheduleResult_t));
	
	ProcessControlBlock_t data[4] = {
	   [0]={8,0,0,0},
	   [1]={4,0,0,0},
	   [2]={3,0,0,0},
	   [3]={7,0,0,0}
	};
        dyn_array_push_back(pcbs,&data[3]);
   	dyn_array_push_back(pcbs,&data[2]);
  	dyn_array_push_back(pcbs,&data[1]);
   	dyn_array_push_back(pcbs,&data[0]);
	ASSERT_EQ(true, first_come_first_serve(pcbs,sr));
	ASSERT_NE(0,sr->average_wall_clock_time);
    ASSERT_NE(0,sr->average_latency_time);
    ASSERT_NE(0,sr->total_run_time);
	dyn_array_destroy(pcbs);
	delete sr;
	score +=20;
}
TEST (first_come_first_serve, badInputZeroBurstTime){
	ScheduleResult_t *sr = new ScheduleResult_t;
        dyn_array_t* pcbs = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
        memset(sr,0,sizeof(ScheduleResult_t));
	ProcessControlBlock_t data[1]={
		[0] = {0,0,0,0}
	};
	dyn_array_push_back(pcbs, &data[0]);
	ASSERT_EQ(false, first_come_first_serve(pcbs, sr));
	ASSERT_EQ(0, sr->average_wall_clock_time);
	ASSERT_EQ(0, sr->average_latency_time);
	ASSERT_EQ(0, sr->total_run_time);
	dyn_array_destroy(pcbs);
	delete sr;
	score +=20;
}
TEST (first_come_first_serve, badInputAllFinishedPCBS){
	ScheduleResult_t *sr = new ScheduleResult_t;
    	dyn_array_t* pcbs = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
    	memset(sr,0,sizeof(ScheduleResult_t));
   	 // add PCBs now
	ProcessControlBlock_t data[4] = {
    		[0] = {1,0,1,0},
       		[1] = {4,8,1,0},
       		[2] = {8,2,1,0},
       		[3] = {5,4,1,0},
	};
 	dyn_array_push_back(pcbs,&data[3]);
	dyn_array_push_back(pcbs,&data[2]);
    dyn_array_push_back(pcbs,&data[1]);
  	dyn_array_push_back(pcbs,&data[0]);
    ASSERT_EQ(false, first_come_first_serve (pcbs,sr));
	ASSERT_EQ(0, sr->average_wall_clock_time);
	ASSERT_EQ(0, sr->average_latency_time);
	ASSERT_EQ(0, sr->total_run_time);
	dyn_array_destroy(pcbs);
	delete sr;
}
/**********************************************/

/*
 * LOAD PROCESS CONTROL BLOCKS TEST CASES
 */
TEST (load_process_control_blocks, nullFilePath) {
    dyn_array_t* da = load_process_control_blocks (NULL);
    ASSERT_EQ(da,(dyn_array_t*) NULL);

    score+=5;
}

TEST (load_process_control_blocks, notFoundFile) {

    dyn_array_t* da = load_process_control_blocks ("NotARealFile.Awesome");
    ASSERT_EQ(da,(dyn_array_t*)NULL);

    score+=5;
}

TEST (load_process_control_blocks, emptyFoundFile) {
    const char* fname = "EMPTYFILE.DARN";
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int fd = open(fname, flags, mode);
    close(fd);
    dyn_array_t* da = load_process_control_blocks (fname);
    ASSERT_EQ(da,(dyn_array_t*)NULL);

    score+=5;
}

TEST (load_process_control_blocks, incorrectPCBFoundFile) {
    const char* fname = "CANYOUHANDLETHE.TRUTH";
    uint32_t pcb_num = 10;
    uint32_t pcbs[5][2] = {{1,1},{2,2},{3,3},{4,4},{5,5}};
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int fd = open(fname, flags, mode);
    write(fd,&pcb_num,sizeof(uint32_t));
    write(fd,pcbs,5 * sizeof(uint32_t)*2);
    close(fd);
    dyn_array_t* da = load_process_control_blocks (fname);
    ASSERT_EQ((dyn_array_t*)NULL,da);

    score+=5;
}

TEST (load_process_control_blocks, fullFoundFile) {
    const char* fname = "../test/PCBs.bin";
    uint32_t pcb_num = NUM_PCB;
    uint32_t pcbs[NUM_PCB][2];
    for (uint32_t p = 0; p < pcb_num; ++p) {
        pcbs[p][0] = rand() % 15 + 1;
        pcbs[p][1] = p;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int fd = open(fname, flags, mode);
    write(fd,&pcb_num,sizeof(uint32_t));
    write(fd,pcbs,pcb_num * sizeof(uint32_t)*2);
    close(fd);
    dyn_array_t* da = load_process_control_blocks (fname);
    ASSERT_NE(da, (dyn_array_t*) NULL);
    for (size_t i = 0; i < dyn_array_size(da); ++i) {
        ProcessControlBlock_t * pPCB = (ProcessControlBlock_t *)dyn_array_at(da, i);
        ASSERT_EQ(pPCB->remaining_burst_time, pcbs[i][0]);
        ASSERT_EQ(pPCB->priority, pcbs[i][1]);
    }
    dyn_array_destroy(da);

    score+=10;
}

/******** Additional Test Cases: Load Process Control Blocks ****/



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
    return RUN_ALL_TESTS();

}


