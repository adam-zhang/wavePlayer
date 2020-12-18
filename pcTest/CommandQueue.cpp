#include "CommandQueue.h"

//CommandQueue::CommandQueue()
//{
//}
//
//CommandQueue::~CommandQueue()
//{
//}
CommandQueue* CommandQueue::instance_ = 0;

std::mutex CommandQueue::mtx_;
