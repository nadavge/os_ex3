#include "Daemon.h"
static deque<Action> actions();
// @TODO Add in the end tell pthread we finished
static int gBlocksAdded = 0;

// @TODO add initDaemon and init pthread, gBlocksAdded
// @TODO receive genesis, gBlockVector
void runDaemon()
{
	gBlocksAdded = 0;
}
void addAction(Action action)
{
	actions.push_back(action);
}

/*
void attachBlockByNum(int blocknum)
{

}
*/

int actualBlocksAdded()
{
	return gBlocksAdded;
}


Block* getDeepestBlock()
{

}
