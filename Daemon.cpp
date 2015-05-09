#include "Daemon.h"
static deque<Action> actions();
// @TODO Add in the end tell pthread we finished

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

