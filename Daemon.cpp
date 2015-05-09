#include "Daemon.h"
static deque<Action> actions();


void runDaemon()
{
	// @TODO start pthread
}
void addAction(Action action)
{
	actions.push_back(action);
}

void getBlockByNumFromActions(int blocknum)
{

}

void attachBlockByNum(int blocknum)
{

}

void markAsRealTime(int blocknum)
{

}
