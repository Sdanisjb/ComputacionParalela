#include <iostream>
#include <random>
#include <time.h>
#include <cstdlib>
#include "mpi.h"

using namespace std;

int compare(const void* a, const void* b) {
	const int* _a = (int*)a;
	const int* _b = (int*)b;

	if (*_a > * _b) return 1;
	else if (*_a < *_b) return -1;
	return 0;
}

int* sort_locally(int* arr, int num_elements) {
	qsort(arr, num_elements, sizeof(int), compare);
	return arr;
}

int* create_rand_nums(int num_elements) {
	srand(time(NULL));
	int* rand_nums = new int[num_elements];
	for (int i = 0; i < num_elements; i++) {
		rand_nums[i] = rand();
	}
	return rand_nums;
}

int* keep_max(int* sub_num, int* neighbour_sub_num, int num_elements) {
	int* keep = new int[num_elements];
	int curr_sn = num_elements-1, curr_nsn = num_elements-1;
	for (int i = num_elements-1; i >= 0; i--) {
		if (sub_num[curr_sn] < neighbour_sub_num[curr_nsn]) {
			keep[i] = neighbour_sub_num[curr_nsn];
			curr_nsn--;
		}
		else {
			keep[i] = sub_num[curr_sn];
			curr_sn--;
		}
	}

	return keep;
}

int* keep_min(int* sub_num, int* neighbour_sub_num, int num_elements) {
	int* keep = new int[num_elements];
	int curr_sn = 0, curr_nsn = 0;
	for (int i = 0; i < num_elements; i++) {
		if (sub_num[curr_sn] > neighbour_sub_num[curr_nsn]) {
			keep[i] = neighbour_sub_num[curr_nsn];
			curr_nsn++;
		}
		else {
			keep[i] = sub_num[curr_sn];
			curr_sn++;
		}
	}

	return keep;
}


int main(int argc, char** argv) {
	MPI_Init(NULL, NULL);

	//Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//Get the rank of the proccess
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int* rand_nums = nullptr;
	int elements_per_proc = 40000;
	rand_nums = create_rand_nums(elements_per_proc * world_size);

	int* sub_rand_nums = new int[elements_per_proc];
	int* neighbour_sub_rand_nums = new int[elements_per_proc];

	float exec_time = 0.f;

	exec_time -= MPI_Wtime();


	MPI_Scatter(rand_nums, elements_per_proc, MPI_INT, sub_rand_nums, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	int* sorted_locally = sort_locally(sub_rand_nums, elements_per_proc);

	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < world_size; i++) {
		//Even Phase
		if (i % 2 == 0) {
			if (my_rank % 2 == 0) {
				if (my_rank + 1 > world_size - 1) continue;
				MPI_Send(sub_rand_nums, elements_per_proc, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				MPI_Recv(neighbour_sub_rand_nums, elements_per_proc, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				sub_rand_nums = keep_min(sub_rand_nums, neighbour_sub_rand_nums, elements_per_proc);
			}
			else {
				if (my_rank - 1 < 0) continue;
				MPI_Recv(neighbour_sub_rand_nums, elements_per_proc, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(sub_rand_nums, elements_per_proc, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
				sub_rand_nums = keep_max(sub_rand_nums, neighbour_sub_rand_nums, elements_per_proc);
			}
		}
		else {
			if (my_rank % 2 == 0) {
				if (my_rank - 1 < 0) continue;
				MPI_Recv(neighbour_sub_rand_nums, elements_per_proc, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(sub_rand_nums, elements_per_proc, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
				sub_rand_nums = keep_max(sub_rand_nums, neighbour_sub_rand_nums, elements_per_proc);
			}
			else {
				if (my_rank + 1 > world_size - 1) continue;
				MPI_Send(sub_rand_nums, elements_per_proc, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				MPI_Recv(neighbour_sub_rand_nums, elements_per_proc, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				sub_rand_nums = keep_min(sub_rand_nums, neighbour_sub_rand_nums, elements_per_proc);
			}
		}
	}

	int* ordered_array = NULL;
	if (my_rank == 0) {
		ordered_array = new int[elements_per_proc * world_size];
	}
	
	MPI_Gather(sub_rand_nums, elements_per_proc, MPI_INT, ordered_array, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
	
	exec_time += MPI_Wtime();
	
	if(my_rank == 0) cout << exec_time << endl;

	delete[] ordered_array;
	delete[] sub_rand_nums;
	delete[] neighbour_sub_rand_nums;
	
	MPI_Finalize();

	return 0;
}