#include <stdio.h>
#include <stdlib.h>

#define NUM_JOBS 2001
#define FALSE 0
#define TRUE 1

struct job {
	int id;
	int time_submitted;
	int duration;
	int completion;
	int time_finished;
};

int read_file(struct job jobs[]);
void reset_data(struct job jobs[], int total_jobs);
void sim_fcfs(struct job jobs[], int total_jobs);
void sim_sjf(struct job jobs[], int total_jobs);
void sim_srtn(struct job jobs[], int total_jobs);
void sim_rr(struct job jobs[], int total_jobs);

int main() {
	struct job jobs[NUM_JOBS];
	int total_jobs;
	total_jobs = read_file(jobs);
	sim_fcfs(jobs, total_jobs);
	sim_sjf(jobs, total_jobs);
	sim_srtn(jobs, total_jobs);
	sim_rr(jobs, total_jobs);
	return 0;
}

int read_file(struct job jobs[]) {
	FILE *file;
	int i, end_file = FALSE, final_index;
	file = fopen("data.txt", "r");
	if (file == NULL) {
		printf("fopen err\n");
		exit(1);
	}
	for (i=0;i<NUM_JOBS && end_file == FALSE;i++) {
		if (fscanf(file, "%d %d %d", &jobs[i].id, &jobs[i].time_submitted, &jobs[i].duration) == EOF) {
			end_file = TRUE;
			final_index = i;
		}
		jobs[i].completion = 0; // initializing all values
		jobs[i].time_finished = -1;
	}
	//for (i=0;i<final_index;i++) { // debug code
	//	printf("%d %d %d\n%d %d\n", jobs[i].id, jobs[i].time_submitted, jobs[i].duration, jobs[i].completion, jobs[i].time_finished);
	//}
	fclose(file);
	return final_index;
}

void reset_data(struct job jobs[], int total_jobs) {
	int i;
	for (i=0;i<total_jobs;i++) {
		jobs[i].completion = 0;
		jobs[i].time_finished = -1;
		//printf("%d %d %d\n%d %d\n", jobs[i].id, jobs[i].time_submitted, jobs[i].duration, jobs[i].completion, jobs[i].time_finished);
	}
}

void sim_fcfs(struct job jobs[], int total_jobs) {
	reset_data(jobs, total_jobs);
	int context_switches = 1; // starting with 1 context switch from initializing jobs
	int time = 0;
	int i = 0, j;
	int sim_complete = FALSE;
	int curr_task_done = FALSE;
	int idling;
	while (sim_complete == FALSE) {
		curr_task_done = FALSE;
		while (curr_task_done == FALSE) {
			//printf("time %d: job %d %d %d/%d, %d\n", time, jobs[i].id, jobs[i].time_submitted, jobs[i].completion, jobs[i].duration, jobs[i].time_finished) // debug code;
			idling = TRUE;
			while (idling == TRUE) {
				for (j=0;j<total_jobs && idling == TRUE;j++) {
					if (jobs[j].time_submitted <= time) {
						idling = FALSE;
					}
				}
				if (idling == TRUE) {
					time++;
				}
			}
			jobs[i].completion++;
			time++;
			if (jobs[i].duration == jobs[i].completion) {
				curr_task_done = TRUE;
				jobs[i].time_finished = time;
			}
		}
		i++;
		if (i >= total_jobs) {
			sim_complete = TRUE;
		} else {
			context_switches++;
		}
	}
	int total_turnaround_time = 0;
	for (i=0;i<total_jobs;i++) {
		total_turnaround_time += jobs[i].time_finished - jobs[i].time_submitted;
	}
	float average_turnaround_time = (float)total_turnaround_time/(float)total_jobs;
	printf("FCFS turnaround time: %f\nFCFS context switches: %d\n", average_turnaround_time, context_switches);
}

void sim_sjf(struct job jobs[], int total_jobs) {
	reset_data(jobs, total_jobs);
	int context_switches = 1; // starting with 1 context switch from initializing jobs
	int time = 0;
	int i = 0, j;
	int sim_complete = FALSE;
	int curr_task_done = FALSE;
	int jobs_done = 0;
	struct job shortest;
	shortest = jobs[i];
	int idling;
	while (sim_complete == FALSE) {
		curr_task_done = FALSE;
		idling = TRUE;
		while (curr_task_done == FALSE) {
			//printf("time %d: job %d %d %d/%d, %d\n", time, jobs[i].id, jobs[i].time_submitted, jobs[i].completion, jobs[i].duration, jobs[i].time_finished); // debug code
			while (idling == TRUE) {
				for (j=0;j<total_jobs && idling == TRUE;j++) {
					if (jobs[j].time_submitted <= time) {
						idling = FALSE;
					}
				}
				if (idling == TRUE) {
					time++;
				}
			}
			jobs[i].completion++;
			time++;
			if (jobs[i].duration <= jobs[i].completion) {
				curr_task_done = TRUE;
				jobs[i].time_finished = time;
				jobs_done++;
			}
		}
		// now seeing what next shortest job is
		for (j=0;j<total_jobs;j++) { // setting shortest to an unfinished job
			if (jobs[j].completion == 0) {
				shortest = jobs[j];
			}
		}	
		for (j=0;j<total_jobs;j++) {
			if ((jobs[j].completion == 0) && (jobs[j].duration < shortest.duration) && (jobs[j].time_submitted <= time)) { // checks if job is unfinished and if duration is less than current shortest and if the job has been submitted yet
				shortest = jobs[j];
			}
		}
		i = shortest.id;
		if (jobs_done == total_jobs) {
			sim_complete = TRUE;
		} else {
			context_switches++;
		}
	}
	int total_turnaround_time = 0;
	for (i=0;i<total_jobs;i++) {
		total_turnaround_time += jobs[i].time_finished - jobs[i].time_submitted;
	}
	float average_turnaround_time = (float)total_turnaround_time/(float)total_jobs;
	printf("SJF turnaround time: %f\nSJF context switches: %d\n", average_turnaround_time, context_switches);
}

void sim_srtn(struct job jobs[], int total_jobs) {
	reset_data(jobs, total_jobs);
	int context_switches = 1; // starting with 1 context switch from initializing jobs
	int time = 0;
	int i = 0, j;
	int sim_complete = FALSE;
	int jobs_done = 0;
	struct job shortest;
	shortest = jobs[i];
	int idling;
	while (sim_complete == FALSE) {
		//printf("time %d: job %d %d %d/%d, %d\n", time, jobs[i].id, jobs[i].time_submitted, jobs[i].completion, jobs[i].duration, jobs[i].time_finished); // debug code
		idling = TRUE;
		while (idling == TRUE) {
			for (j=0;j<total_jobs && idling == TRUE;j++) {
				if (jobs[j].time_submitted <= time) {
					idling = FALSE;
				}
			}
			if (idling == TRUE) {
				time++;
			}
		}
		jobs[i].completion++;
		time++;
		
		if (jobs[i].duration <= jobs[i].completion) {
			jobs[i].time_finished = time;
			jobs_done++;
		}
		// now seeing if any other job has less time remaining
		shortest = jobs[i];
		for (j=0;j<total_jobs;j++) {
			if (jobs[j].duration-jobs[j].completion > 0) {
				shortest = jobs[j];
			}
		}
		for (j=0;j<total_jobs;j++) {
			//printf("job %d, %d\n shortest %d, %d\n", jobs[j].id, jobs[j].duration-jobs[j].completion, shortest.id, shortest.duration-shortest.completion); // debug code
			if ((jobs[j].time_submitted <= time) && (jobs[j].duration-jobs[j].completion > 0) && (jobs[j].duration-jobs[j].completion < shortest.duration-shortest.completion)) {
				shortest = jobs[j];
			}
		}
		if (shortest.id != i) {
			context_switches++;
		}
		i = shortest.id;
		

		if (jobs_done == total_jobs) {
			sim_complete = TRUE;
		}
	}
	int total_turnaround_time = 0;
	for (i=0;i<total_jobs;i++) {
		total_turnaround_time += jobs[i].time_finished - jobs[i].time_submitted;
	}
	float average_turnaround_time = (float)total_turnaround_time/(float)total_jobs;
	printf("SRTN turnaround time: %f\nSRTN context switches: %d\n", average_turnaround_time, context_switches);
}

void sim_rr(struct job jobs[], int total_jobs) {
	reset_data(jobs, total_jobs);
	int context_switches = 1; // starting with 1 context switch from initializing jobs
	int time = 0;
	int i = 0, j;
	int sim_complete = FALSE;
	int jobs_done = 0;
	int jobs_queued = 0;
	int idling, task_found, min_found;
	while (sim_complete == FALSE) {
		idling = TRUE;
		task_found = FALSE;
		min_found = FALSE;
		while (idling == TRUE) {
			for (j=0;j<total_jobs && idling == TRUE;j++) {
				if (jobs[j].time_submitted <= time) {
					idling = FALSE;
				}
			}
			if (idling == TRUE) {
				time++;
			}
		}
		if (i >= jobs_queued) {
			for (j=0;j<jobs_queued && min_found == FALSE;j++) {
				if ((jobs[j].completion < jobs[j].duration)) {
				       i = j;
				       context_switches++;
				       min_found = TRUE;
				}
			}
		} else {
			do {
				i++;
			} while (jobs[i].completion == jobs[i].duration);
			context_switches++;
		}
		
		jobs[i].completion++;
		time++;

		//printf("time %d: job %d %d %d/%d, %d\n", time, jobs[i].id, jobs[i].time_submitted, jobs[i].completion, jobs[i].duration, jobs[i].time_finished); // debug code
		
		// now seeing if any other job has been queued up
		for (j=jobs_queued+1;j<total_jobs && task_found == FALSE;j++) {
			if (jobs[j].time_submitted <= time) {
				jobs_queued++;
				task_found = TRUE;
			}
		}
		if (jobs[i].duration <= jobs[i].completion) {
			jobs[i].time_finished = time;
			jobs_done++;
		}
		
		if (jobs_done == total_jobs) {
			sim_complete = TRUE;
		}
	}
	int total_turnaround_time = 0;
	for (i=0;i<total_jobs;i++) {
		total_turnaround_time += jobs[i].time_finished - jobs[i].time_submitted;
	}
	float average_turnaround_time = (float)total_turnaround_time/(float)total_jobs;
	printf("RR turnaround time: %f\nRR context switches: %d\n", average_turnaround_time, context_switches);

}

