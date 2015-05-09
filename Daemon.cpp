#include "Daemon.h"
static deque<Action> actions();
// @TODO Add in the end tell pthread we finished
static int gBlocksAdded = 0;
// @TODO add initDaemon and init pthread, gBlocksAdded
void runDaemon()
{
	gBlocksAdded = 0;
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

int actualBlocksAdded() {
	return gBlocksAdded;
}
