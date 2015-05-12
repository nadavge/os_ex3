// TODO add file documentation header
#include <deque>
#include<pthread.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include "Block.h"
#include "hash.h"
// TODO \forall functions check when -1
// TODO Srand
// TODO think
/*
 * TODO daemon might be switched into after close while adding a new block
 * need to *not* do this
 */

using namespace std;

/*
 *       A multi threaded blockchain database manager
 *       Author: we!
 */

#define BLOCK_DOESNT_EXIST -2
#define NOT_STARTED -1
#define NOT_CLOSING -2
#define ERROR -1
#define BLOCK_IN_CHAIN 1
#define BLOCK_NOT_IN_CHAIN 0
#define SUCCESS 0

#define compare_and_swap __sync_bool_compare_and_swap
#define RUNNING() (gBlocksAdded != NOT_STARTED && ! gIsClosing)
#define LOCK_ALL() (pthread_mutex_lock(&lock) == 0)
#define UNLOCK_ALL() (pthread_mutex_lock(&lock) == 0)
//========================DECLARATIONS===========================
int init_blockchain();
int add_block(char *data , int length);
int to_longest(int block_num);
int attach_now(int block_num);
int was_added(int block_num);
int chain_size();
int prune_chain();
void close_chain();
int return_on_close();
int takeMinUnusedBlocknum(Block* block);
Block* getBlockByBlocknum(int blocknum);
Block* getDeepestBlock();
bool initDaemon();
void* runDaemon(void* arg);
inline void addToQueue(Block* toAdd);
void attachBlockByNum(int blocknum);
void addBlockAssumeMutex(Block* toAdd);
void daemonAddBlock(Block* toAdd);
void terminateDaemon();

//========================GLOBALS================================
static deque<Block*> gQueueBlock;
static int gBlocksAdded = NOT_STARTED;
static Block* gGenesis = nullptr;
static vector<Block*> gBlockVector;
static vector<Block*> gDeepestBlocks;
static bool gIsClosing = false;
static pthread_t gDaemonThread = {0};
pthread_mutex_t lock;

//========================IMPLEMENTATION==========================

/*
 * DESCRIPTION: This function initiates the Block chain, and creates the genesis Block.  The genesis Block does not hold any transaction data
 *      or hash. should be called prior to any other functions as a necessary precondition for their success (all other functions should
 *      return with an error otherwise).
 * RETURN VALUE: On success 0, otherwise -1.
 */

int init_blockchain()
{
	// TODO check flaging system - might not act as intended
    if(RUNNING())
    {
        return ERROR;
    }
	
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

/*
 * DESCRIPTION: Ultimately, the function adds the hash of the data to the Block chain.
 *      Since this is a non-blocking package, your implemented method should return as soon as possible, even before the Block was actually
 *      attached to the chain.
 *      Furthermore, the father Block should be determined before this function returns. The father Block should be the last Block of the
 *      current longest chain (arbitrary longest chain if there is more than onSTARTED()e).
 *      Notice that once this call returns, the original data may be freed by the caller.
 * RETURN VALUE: On success, the function returns the lowest available block_num (> 0),
 *      which is assigned from now on to this individual piece of data.
 *      On failure, -1 will be returned.
 */
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
    addToQueue(block);
    return blockNum;
}

/*
 * DESCRIPTION: Without blocking, enforce the policy that this block_num should be attached to the longest chain at the time of attachment of
 *      the Block. For clearance, this is opposed to the original add_block that adds the Block to the longest chain during the time that
 *      add_block was called.
 *      The block_num is the assigned value that was previously returned by add_block.
 * RETURN VALUE: If block_num doesn't exist, return -2; In case of other errors, return -1; In case of success return 0; In case block_num is
 *      already attached return 1.
*/
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

/*
 * DESCRIPTION: This function blocks all other Block attachments, until block_num is added to the chain. The block_num is the assigned value
 *      that was previously returned by add_block.
 * RETURN VALUE: If block_num doesn't exist, return -2;
 *      In case of other errors, return -1; In case of success or if it is already attached return 0.
*/
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
        return BLOCK_DOESNT_EXIST;
    }
    if(! block->inChain())
    {
        addBlockAssumeMutex(block);
    }

    return SUCCESS;
}
/*
 * DESCRIPTION: Without blocking, check whether block_num was added to the chain.
 *      The block_num is the assigned value that was previously returned by add_block.
 * RETURN VALUE: 1 if true and 0 if false. If the block_num doesn't exist, return -2; In case of other errors, return -1.
*/
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

/*
 * DESCRIPTION: Return how many Blocks were attached to the chain since init_blockchain.
 *      If the chain was closed (by using close_chain) and then initialized (init_blockchain) again this function should return
 *      the new chain size.
 * RETURN VALUE: On success, the number of Blocks, otherwise -1.
*/
int chain_size()
{
    if(! RUNNING())
    {
        return ERROR;
    }
    return gBlocksAdded;
}

/*
 * DESCRIPTION: Search throughout the tree for sub-chains that are not the longest chain,
 *      detach them from the tree, free the blocks, and reuse the block_nums.
 * RETURN VALUE: On success 0, otherwise -1.
*/
int prune_chain()
{
    if(! RUNNING())
    {
        return ERROR;
    }
    if(! LOCK_ALL())
    {
        return ERROR;
    }


    Block* deepest = getDeepestBlock();
    deque<Block*> longestChain;
    while(deepest != nullptr)
    {
        longestChain.push_front(deepest);
        deepest = deepest->getFather();
    }
    for(int i = 0; i < gBlockVector.size(); i++)
    {
        Block* currBlock;
        currBlock = gBlockVector.at(i);
        if(currBlock == nullptr)
        {
            continue;
        }

        if(currBlock != longestChain.at(currBlock->getDepth()))
        {
            delete gBlockVector.at(i);
            gBlockVector.at(i) = nullptr;
        }
    }

	for (auto &block : gQueueBlock)
	{
		/*
		 * Check if the blocks father is NOT in the longest chain => it was removed.
		 * Replace the blocks father with the currently deepest block
		 */ 
		if (find(longestChain.begin(), longestChain.end(), block->getFather()) == longestChain.end())
		{
			block->setFather(longestChain.back());
		}
	}

    if (! UNLOCK_ALL())
    {
        return ERROR;
    }
}

/*
 * DESCRIPTION: Close the recent blockchain and reset the system, so that it is possible to call init_blockchain again. Non-blocking.
 *      All pending Blocks should be hashed and printed to terminal (stdout).
 *      Calls to library methods which try to alter the state of the Blockchain are prohibited while closing the Blockchain. e.g.: Calling
 *      chain_size() is ok, a call to prune_chain() should fail.
 *      In case of a system error, the function should cause the process to exit.
*/
void close_chain()
{
    if(! RUNNING())
    {
        exit(1);
    }

    gIsClosing = true;

}

/*
 * DESCRIPTION: The function blocks and waits for close_chain to finish.
 * RETURN VALUE: If closing was successful, it returns 0.
 *      If close_chain was not called it should return -2. In case of other error, it should return -1.
*/ //========================GLOBALS================================


int return_on_close()
{
    if( ! gIsClosing && gBlocksAdded != NOT_STARTED)
    {
        return NOT_CLOSING;
    }
    while(gIsClosing)
    {
        // TODO Maybe let pthread know we're waiting?
    }

    return 0;
}


int takeMinUnusedBlocknum(Block* block)
{
	// Signals whether the function got the min value
	bool tookMin = false;
    int blocknum = 0;

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

    gBlockVector.push_back(block);
    return blocknum;
}
Block* getBlockByBlocknum(int blocknum)
{
    if(blocknum >= gBlockVector.size())
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
}

void* runDaemon(void* arg)
{
	Block* block = nullptr;
	while(! gIsClosing)
	{
		if (! gQueueBlock.empty())
		{
			block = gQueueBlock.front();
			gQueueBlock.pop_front();

			daemonAddBlock(block);
		}
	}

	terminateDaemon();

	// TODO set flag of (is daemon running maybe)
	return nullptr;
}

void terminateDaemon()
{
	char* hash = nullptr;
	Block* block = nullptr;

	// TODO Free all block items (from everywhere!!!!)
	if (! LOCK_ALL())
	{
		exit(1);
	}

	while (! gQueueBlock.empty())
	{
		block = gQueueBlock.front();
		gQueueBlock.pop_front();

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
		exit(1);
	}

	pthread_mutex_destroy(&lock);
	close_hash_generator();

}

inline void addToQueue(Block* toAdd)
{
    gQueueBlock.push_back(toAdd);
}


void daemonAddBlock(Block* toAdd)
{
	// TODO What if THIS function FAILS?
	int nonce = -1;
    if(! RUNNING())
    {
        exit(1);
    }
    if(! LOCK_ALL())
    {
		exit(1);
    }

	addBlockAssumeMutex(toAdd);

    if(! UNLOCK_ALL())
    {
 		exit(1);
    }
}
void addBlockAssumeMutex(Block* toAdd)
{
	// TODO What if THIS function FAILS?
	int nonce = -1;

    if(toAdd->toAddInRealTime())
    {
        toAdd->setFather(getDeepestBlock());
    }

	toAdd->setHash(generateHash(toAdd));
	toAdd->setDepth(toAdd->getFather()->getDepth()+1);

	if (toAdd->getDepth() > Block::getMaxDepth())
	{
		gDeepestBlocks.clear();
		Block::setMaxDepth(toAdd->getDepth());
	}

	if (toAdd->getDepth() == Block::getMaxDepth())
	{
		gDeepestBlocks.push_back(toAdd);
	}
	
	// Remove block from pending queue
	gQueueBlock.erase(remove(gQueueBlock.begin(), gQueueBlock.end(), toAdd), gQueueBlock.end());
}

