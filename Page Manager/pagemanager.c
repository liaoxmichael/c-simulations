#include <stdio.h>
#include <stdlib.h>

#define NUM_PAGES 2001
#define FALSE 0
#define TRUE 1

struct page {
	int page_num;
	int time_accessed;
};

int read_file(struct page pages[], int *total_pages);
void reset_data(struct page pages[], int *total_pages);
void sim_optimal(struct page pages[], int total_frames, int *total_pages);
void sim_fifo(struct page pages[], int total_frames, int *total_pages);
void sim_lru(struct page pages[], int total_frames, int *total_pages);

int main() {
	struct page pages[NUM_PAGES];
	int total_frames;
	int *total_pages;
	int local_pages;
	total_pages = &local_pages;
	total_frames = read_file(pages, total_pages);
	sim_optimal(pages, total_frames, total_pages);
	sim_fifo(pages, total_frames, total_pages);
	sim_lru(pages, total_frames, total_pages);
	return 0;
}

int read_file(struct page pages[], int *total_pages) {
	FILE *file;
	int i, end_file = FALSE, frames_detected;
	file = fopen("data.txt", "r");
	if (file == NULL) {
		printf("fopen err\n");
		exit(1);
	}
	if (fscanf(file, "%d", &frames_detected) == EOF) {
		printf("fscanf err\n");
	}
	//printf("frames: %d\n", frames_detected); // debug code
	for (i=0;i<NUM_PAGES && end_file == FALSE;i++) {
		if (fscanf(file, "%d", &pages[i].page_num) == EOF) {
			end_file = TRUE;
			*total_pages = i;
		}
		pages[i].time_accessed = -1; // initializing all values
	}
	//for (i=0;i<*total_pages;i++) { // debug code
	//	printf("%d\n", pages[i].page_num);
	//}
	fclose(file);
	return frames_detected;
}

void reset_data(struct page pages[], int *total_pages) {
	int i;
	for (i=0;i<*total_pages;i++) {
		pages[i].time_accessed = -1;
	}
}

void sim_optimal(struct page pages[], int total_frames, int *total_pages) {
	reset_data(pages, total_pages);
	int page_faults = 0;
	int i = 0, j, k = 0;
	int sim_complete = FALSE;
	int page_found, another_page_found, no_more_instances;
	struct page frames[total_frames];
	for (j=0;j<total_frames;j++) { // initializing page nums for the computer's frames to be empty at beginning
		frames[j].page_num = -1;
		frames[j].time_accessed = -1;
	}
	struct page longest;
	while (sim_complete == FALSE) {
		page_found = FALSE;
		for (j=0;j<total_frames && page_found == FALSE;j++) {
			if (pages[i].page_num == frames[j].page_num) {
				page_found = TRUE;
			}
		}
		if (page_found == FALSE) {
			pages[i].time_accessed = i;
			if (page_faults >= total_frames) { // once all pages filled
				longest = frames[0]; // initializing
				longest.time_accessed = i;
				no_more_instances = FALSE;
				for (j=0;j<total_frames && no_more_instances == FALSE;j++) {
					another_page_found = FALSE;
					for (k=i;k<*total_pages && another_page_found == FALSE;k++) {
						if (frames[j].page_num == pages[k].page_num) { // finding the next use of this page
							//printf("frame: %d has page %d, next instance is %d away\n", j, frames[j].page_num, k-i); // debug code
							another_page_found = TRUE;
							//printf("k-i: %d-%d=%d; longest-i: %d-%d=%d\n", k, i, k-i, longest.time_accessed, i, longest.time_accessed-i); // debug code
							if ((k - i) > (longest.time_accessed-i)) { // if the index between this page and the next time it's used is larger than the current most distant page, set accordingly
							       longest = frames[j];
							       longest.time_accessed = k;
							}
						}
					}
					if (another_page_found == FALSE) { // exiting loop if a page does not exist farther down the queue
						longest = frames[j];
						no_more_instances = TRUE;
					}
				}
				for (j=0;j<total_frames;j++) { // setting k to the optimal page to swap out
					if (frames[j].page_num == longest.page_num) {
						k = j;
						//printf("changing k to %d\n", j); // debug code
					}
				}
			}
			//printf("page fault, replacing a page %d in frame %d with %d\n", frames[k].page_num, k, pages[i].page_num); // debug code
			frames[k] = pages[i]; // storing the current page in use
			k++; // moving along the current used pages while empty, gets rewritten if pages full
			page_faults++;
		}
		i++;
		if (i >= *total_pages) { // checking if all pages processed
			sim_complete = TRUE;
		}
	}
	printf("optimal page faults: %d\n", page_faults);
}

void sim_fifo(struct page pages[], int total_frames, int *total_pages) {
	reset_data(pages, total_pages);
	int page_faults = 0;
	int i = 0, j, k = 0;
	int sim_complete = FALSE;
	int page_found;
	struct page frames[total_frames];
	for (j=0;j<total_frames;j++) { // initializing page nums for the computer's frames to be empty at beginning
		frames[j].page_num = -1;
	}
	while (sim_complete == FALSE) {
		page_found = FALSE;
		for (j=0;j<total_frames && page_found == FALSE;j++) {
			if (pages[i].page_num == frames[j].page_num) {
				page_found = TRUE;
			}
		}
		if (page_found == FALSE) {
			pages[i].time_accessed = i;
			if (k >= total_frames) { // if we fill up all our allotted pages, we reset
				k = 0;
			}
			//printf("page fault, replacing a page %d in frame %d with %d\n", frames[k].page_num, k, pages[i].page_num); // debug code
			frames[k] = pages[i]; // storing the current page in use
			k++; // moving along the current used pages
			page_faults++;
		}
		i++;
		if (i >= *total_pages) {
			sim_complete = TRUE;
		}
	}
	printf("fifo page faults: %d\n", page_faults);
}

void sim_lru(struct page pages[], int total_frames, int *total_pages) {
	reset_data(pages, total_pages);
	int page_faults = 0;
	int i = 0, j, k = 0;
	int sim_complete = FALSE;
	int page_found; 
	struct page frames[total_frames];
	for (j=0;j<total_frames;j++) { // initializing page nums for the computer's frames to be empty at beginning
		frames[j].page_num = -1;
	}
	struct page oldest;
	while (sim_complete == FALSE) {
		page_found = FALSE;
		for (j=0;j<total_frames && page_found == FALSE;j++) {
			if (pages[i].page_num == frames[j].page_num) {
				page_found = TRUE;
				pages[i].time_accessed = i; // updating the time accessed to be most recent
				frames[j] = pages[i]; // keeping frames in sync
			}
		}
		if (page_found == FALSE) {
			pages[i].time_accessed = i;
			if (page_faults >= total_frames) { // once all pages filled
				oldest = frames[0]; // initializing
				for (j=0;j<total_frames;j++) {
					if (frames[j].time_accessed < oldest.time_accessed) { // finding the oldest frame
						oldest = frames[j];
					}
				}
				for (j=0;j<total_frames;j++) { // setting k to least recently used page to swap out
					if (frames[j].page_num == oldest.page_num) {
						k = j;
					}
				}
			}
			//printf("page fault, replacing a page %d in frame %d with %d\n", frames[k].page_num, k, pages[i].page_num); // debug code
			frames[k] = pages[i]; // storing the current page in use
			k++; // moving along the current used pages while empty, rewritten when pages full
			page_faults++;
		}
		i++;
		if (i >= *total_pages) {
			sim_complete = TRUE;
		}
	}
	printf("lru page faults: %d\n", page_faults);
}
