#include <iostream>
#include "multithread.h"
#include <vector>
#include <string>
#include <thread>

int count=0;

void print_t()
{

	std::chrono::milliseconds dura(2000);
	std::this_thread::sleep_for(dura);  
	std::cout<<"now this thread print the number "<<count<<" "<<std::endl;
	count++;

}

int main()
{
	size_t poolnum=20;
	fixed_thread_pool thpool(poolnum);
	for(int i=0;i<200;i++)
	{
		thpool.addtask(&print_t);
	}

	std::cin.get();
	return -1;

}
