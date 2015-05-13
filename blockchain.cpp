/*
 * blockchain.cpp
 * A bitcoin-inspired multithreaded blockchain implementation
 * Authors:
 * 	Nadav Geva <nadav.geva4@gmail.com>
 * 	Daniel Danon <danondaniel@gmail.com>
 */

#include <deque>
#include <pthread.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include "Block.h"
#include "blockchain.h"
#include "hash.h"

using namespace std;

//========================MACROS=================================

#define BLOCK_DOESNT_EXIST -2
#define NOT_STARTED -1
#define NOT_CLOSING -2
#define ERROR -1
#define BLOCK_IN_CHAIN 1
#define BLOCK_NOT_IN_CHAIN 0
#define SUCCESS 0

#define compare_and_swap __sync_bool_compare_and_swap
#define RUNNING() (gBlocksAdded != NOT_STARTED && ! gIsClosing)
// Tries locking the mutex, returns true iff no errors occured
#define LOCK_ALL() (pthread_mutex_lock(&lock) == 0)
// Tries unlocking the mutex, returns true iff no errors occured
#define UNLOCK_ALL() (pthread_mutex_unlock(&lock) == 0)

//========================DECLARATIONS===========================

int takeMinUnusedBlocknum(Block* block);
Block* getBlockByBlocknum(int blocknum);
Block* getDeepestBlock();
bool initDaemon();
void* runDaemon(void* arg);
inline void addToQueue(Block* toAdd);
void attachBlockByNum(int blocknum);
void addBlockAssumeMutex(Block* toAdd);
void handlePending();
void terminateDaemon();

//========================GLOBALS================================

// The queue of blocks waiting to be inserted
static deque<Block*> gQueueBlock;
// Counter of blockes added to the CHAIN (not CREATED). def: -1
static int gBlocksAdded = NOT_STARTED;
// The genesis block
static Block* gGenesis = nullptr;
/*
 * The vector containing all blocks allocated who own an ID.
 * The index of each block in the vector matches its ID.
 */
static vector<Block*> gBlockVector;
// The vector of all blocks at the depth Blocks::s_maxDepth
static vector<Block*> gDeepestBlocks;
static bool gIsClosing = false;
static pthread_t gDaemonThread = {0};
// The mutex to allow editing of the chain and blocks structure
pthread_mutex_t lock;

//========================IMPLEMENTATION==========================

//TODO remove on release
void print_chain()
{
	int i = 0;
	cout << endl << endl << "================================================" << endl << endl;
	for(auto &block : gBlockVector)
	{
		cout << "===Block " << i << "===" << endl;
		if(block == nullptr)
		{
			cout << "NULLPTR block" << endl;
			continue;
		}
		cout << "Id: " << block->getId() << ", depth: " << block->getDepth() << endl;
		cout << "AddInRealTime: " << block->toAddInRealTime() << ", Was added in RT: " << block->wasAddedInRealTime() << endl;
		if(block->getFather() == nullptr) {
			cout << "Orphan block" << endl;
		}
		else
		{
			cout << "Father blocknum: " << block->getFather()->getId() << endl;
		}
		++i;
	}
	cout << endl << endl << "=================================================" << endl << endl;
}

int init_blockchain()
{
	// TODO check flaging system - might not act as intended
	if(RUNNING())
	{
		return ERROR;
	}

	// Used for hash function	
	srand(time(0));

	Block::initMaxDepth();
	gGenesis = new Block();
	gGenesis->setFather(nullptr);
	gGenesis->setDepth(0);
	gGenesis->setId(0);
	gBlockVector.push_back(gGenesis);
	gDeepestBlocks.push_back(gGenesis);

	gBlocksAdded = 0;
	gIsClosing = false;
	
	if (! initDaemon())
	{
		delete gGenesis;
		return ERROR;
	}

	return 0;
}

int add_block(char *data , int length)
{
	if(! RUNNING())
	{
		return ERROR;
	}

	int blockNum = -1;
	Block* block = new Block();

	if(! block->setData(data, length))
	{
		delete block;
		return ERROR;
	}

	block->setFather(getDeepestBlock());
	blockNum = takeMinUnusedBlocknum(block);
    	block->setId(blockNum);
	addToQueue(block);

	return blockNum;
}

int to_longest(int block_num)
{
	if(! RUNNING())
	{
		return ERROR;
	}

	Block* block = getBlockByBlocknum(block_num);

	if(block == nullptr)
	{
		return BLOCK_DOESNT_EXIST;
	}

	if(block->inChain())
	{
		return BLOCK_IN_CHAIN;
	}

	block->setAddInRealTime();

	if(! block->inChain() || block->wasAddedInRealTime())
	{
		return SUCCESS;
	}
	else
	{
		return BLOCK_IN_CHAIN;
	}
}

int attach_now(int block_num)
{
	if(! RUNNING())
	{
		return ERROR;
	}

	if(! LOCK_ALL())
	{
		return ERROR;
	}

	Block* block = getBlockByBlocknum(block_num);

	if(block == nullptr)
	{

		if(! UNLOCK_ALL())
		{
			return ERROR;
		}
		return BLOCK_DOESNT_EXIST;
	}
   
	if(! block->inChain())
	{
		addBlockAssumeMutex(block);
	}

	if(! UNLOCK_ALL())
	{
		return ERROR;
	}

	return SUCCESS;
}

int was_added(int block_num)
{
	if(! RUNNING())
	{
		return ERROR;
	}

	Block* block = getBlockByBlocknum(block_num);

	if(block == nullptr)
	{
		return BLOCK_DOESNT_EXIST;
	}

	if(block->inChain())
	{
		return BLOCK_IN_CHAIN;
	}

	else
	{
		return BLOCK_NOT_IN_CHAIN; 
	}
}

int chain_size()
{
	if(! RUNNING())
	{
		return ERROR;
	}

	return gBlocksAdded;
}

int prune_chain()
{
	/*
	 * The mechanism is as such:
	 * 1. Get a block from the deepest depth, and build its route to genesis in an array
	 * 2. Iterate over all allocated blocks, and for each block that is in the block chain but
	 *	not in the longest chain chosen, delete it from the chain and free it
	 * 3. Iterate over all blocks waiting to be added, and for each of them whose father was
	 *	deleted, choose a new father who's the deepest block in the current chain
	 */

	int currDepth = -1;

	if(! RUNNING())
	{
		return ERROR;
	}

	if(! LOCK_ALL())
	{
		return ERROR;
	}

	Block* deepest = getDeepestBlock();
	Block* longestChain[Block::getMaxDepth() + 1];
	currDepth = 0;
	//TODO remove print	
	print_chain();

	//TODO remove currDepth check (used for infinite loop check)
	while(deepest != nullptr && currDepth <= Block::getMaxDepth()+10)
	{
		cout << "Currently at {" << deepest->getId() << "} - Depth: " << deepest->getDepth() << "/" << Block::getMaxDepth() << endl;
		++currDepth;

		longestChain[deepest->getDepth()] = deepest;
		deepest = deepest->getFather();
	}
   
	for(unsigned int i = 0; i < gBlockVector.size(); i++)
	{
		Block* currBlock;
		currBlock = gBlockVector.at(i);
		if(currBlock == nullptr || !currBlock->inChain())
		{
			continue;
		}

		if(currBlock != longestChain[currBlock->getDepth()])
		{
			gBlockVector.at(i) = nullptr;
			delete currBlock;
		}
	}

	for (auto &block : gQueueBlock)
	{
		/*
		 * Check if the blocks father is NOT in the longest chain => it was removed.
		 * Replace the blocks father with the currently deepest block
		 */ 
		bool fatherAlive = false;
		for (int i = 0; i < Block::getMaxDepth() + 1; ++i)
		{
			if (longestChain[i] == block)
			{
				fatherAlive = true;
				break;
			}
		}

		if (! fatherAlive)
		{
			block->setFather(longestChain[Block::getMaxDepth()]);
		}
	}

	gDeepestBlocks.clear();
	gDeepestBlocks.push_back(longestChain[Block::getMaxDepth()]);

	if (! UNLOCK_ALL())
	{
		return ERROR;
	}

	return SUCCESS;
}

void close_chain()
{
	if(! RUNNING())
	{
		exit(2);
	}

	gIsClosing = true;
}

int return_on_close()
{
	if( ! gIsClosing && gBlocksAdded != NOT_STARTED)
	{
		return NOT_CLOSING;
	}

	pthread_join(gDaemonThread, nullptr);
	
	return 0;
}


/**
* @brief Takes the minimum unused block num available
*
* @param block the block to attach to the min num
*
* @return the block num taken
*/
int takeMinUnusedBlocknum(Block* block)
{
	// Signals whether the function got the min value
	bool tookMin = false;
	unsigned int blocknum = 0;

	// Until took the min or found out that no free id's are, try and take the minimum
	while (! tookMin && blocknum < gBlockVector.size())
	{
		blocknum = 0;
   		for (auto &currBlock : gBlockVector)
		{
	   		if(currBlock == nullptr)
			{
				tookMin = compare_and_swap(&gBlockVector[blocknum], nullptr, block);
				break;
			}

			blocknum++;
   		}
	}

	/*
	 * If the no free slots were spotted in the vector, we need to push the item to the back.
	 * While doing so, another thread might race to push to, so to verify we return the correct
	 * block num we search for the value we just added.
	 * If it wasn't for the mutex limitation we would simply use a mutex here
	 */
	if (! tookMin)
	{

		blocknum = 0;
		gBlockVector.push_back(block);
		for (auto &currBlock : gBlockVector)
		{

			if(currBlock == block)
			{
				break;
			}
			++blocknum;
		}
	}

	return blocknum;
}

/**
* @brief Get the block owning the block num
*
* @param blocknum the block num
*
* @return the block who's block num is the one provided. nullptr if non-existent
*/
Block* getBlockByBlocknum(int blocknum)
{
	if((unsigned int) blocknum >= gBlockVector.size())
	{
		return nullptr;
	}

	return gBlockVector.at(blocknum);
}

/*
 * @brief Selects a random block from the gDeepestBlocks vector
 * @return Pointer to the random block
*/
Block* getDeepestBlock()
{

	auto it = gDeepestBlocks.begin();
	advance(it, rand() % gDeepestBlocks.size());

	return *it;
}

/**
* @brief Generate the hash for a block based on its data
*
* @param block the block to generate the hash for
*
* @return a pointer to the hash (need to free)
*/
char* generateHash(Block* block)
{
	int nonce = generate_nonce(block->getId(), block->getFather()->getId());
	return generate_hash(block->getData(), block->getDataLength(), nonce);
}

//========================DAEMON CODE============================

/**
* @brief Initialize the demon
*
* @return true iff the initialization was successful
*/
bool initDaemon()
{
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		return false;
	}
	
	init_hash_generator();
	
	if (pthread_create(&gDaemonThread, nullptr, runDaemon, nullptr) != 0)
	{
		close_hash_generator();
		pthread_mutex_destroy(&lock);
		return false;
	}

	return true;
}

/**
* @brief The daemon thread entry-point
*/
void* runDaemon(void* arg)
{
	while(! gIsClosing)
	{
		// If nothing to do, switch context
		if (gQueueBlock.empty())
		{
			pthread_yield();
		}
		else
		{
			handlePending();
		}
	}

	terminateDaemon();

	pthread_exit(nullptr);

	return nullptr;
}

/**
* @brief The daemon termination function
* Responsible for all the closings
*/
void terminateDaemon()
{
	char* hash = nullptr;
	Block* block = nullptr;

	if (! LOCK_ALL())
	{
		exit(3);
	}

	while (! gQueueBlock.empty())
	{
		block = gQueueBlock.front();
		gQueueBlock.pop_front();

		gBlockVector.at(block->getId()) = nullptr;

		hash = generateHash(block);
		cout << hash << endl;
		delete hash;
		delete block;
	}

	for (auto &block : gBlockVector)
	{
		if (block != nullptr)
		{
			delete block;
		}
	}

	gBlockVector.clear();
	gDeepestBlocks.clear();

	if (! UNLOCK_ALL())
	{
		exit(4);
	}

	pthread_mutex_destroy(&lock);
	close_hash_generator();
	//TODO remove when using states
	gBlocksAdded = NOT_STARTED;
	gIsClosing = false;
}

/**
* @brief Add the block to the pending queue for insertion
*
* @param toAdd the block to add
*/
inline void addToQueue(Block* toAdd)
{
	gQueueBlock.push_back(toAdd);
}


/**
* @brief Handles the pending queue for waiting blocks
*/
void handlePending()
{
	Block* toAdd = nullptr;

	if(! LOCK_ALL())
	{
		exit(6);
	}

	if (! gQueueBlock.empty())
	{
		toAdd = gQueueBlock.front();
		gQueueBlock.pop_front();

		addBlockAssumeMutex(toAdd);
	}

	if(! UNLOCK_ALL())
	{
 		exit(7);
	}
}

/**
* @brief The add block function for the daemon. Requires locking,
* but assumes to be called while the mutex is locked by the calling thread.
*
* @param toAdd the block to add
*/
void addBlockAssumeMutex(Block* toAdd)
{
	if(toAdd->toAddInRealTime())
	{
		toAdd->setFather(getDeepestBlock());
	}

	toAdd->setHash(generateHash(toAdd));
	toAdd->setDepth(toAdd->getFather()->getDepth()+1);

	if (toAdd->getDepth() > Block::getMaxDepth())
	{
		vector<Block*> tempDeepestBlocks = {toAdd};
		gDeepestBlocks = tempDeepestBlocks;
		Block::setMaxDepth(toAdd->getDepth());
	}
	else if (toAdd->getDepth() == Block::getMaxDepth())
	{
		gDeepestBlocks.push_back(toAdd);
	}
	
	++gBlocksAdded;	
	
	gQueueBlock.erase(remove(gQueueBlock.begin(), gQueueBlock.end(), toAdd), gQueueBlock.end());
}

