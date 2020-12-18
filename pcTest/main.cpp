#include <iostream>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "CommandQueue.h"

using namespace std;

std::mutex mtx;

void consume()
{
	for(;;)
	{
		int i = 0;
		CommandQueue::instance().pop(i);
		cout << "poped:" << i << endl;
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

void produce()
{
	for(;;)
	{
		int i = rand() % 1024;
		CommandQueue::instance().push(i);
		cout << "pushed:" << i << endl;
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}


int main(int argc, char** argv)
{
	srand(time(0));
	auto t1 = std::thread(consume);
	auto t2 = std::thread(produce);
	t1.join();
	t2.join();
	return 0;
}
