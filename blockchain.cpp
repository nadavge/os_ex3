// TODO add file documentation header
#include <deque>
// @TODO \forall functions check when -1
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
 #define SUCCESS 0

#define STARTED() (gBlocksAdded != NOT_STARTED)
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
void runDaemon();
inline void addToQueue(Block* toAdd);
void attachBlockByNum(int blocknum);

//========================GLOBALS================================
static deque<Block*> queueBlock();
// @TODO Add in the end tell pthread we finished flag
static int gBlocksAdded = 0;
static Block* gGenesis = nullptr;
static vector<Block*> gBlockVector();
static vector<Block*> gDeepestBlocks();
static boolean gIsClosing = false;

//========================IMPLEMENTATION==========================

/*
 * DESCRIPTION: This function initiates the Block chain, and creates the genesis Block.  The genesis Block does not hold any transaction data
 *      or hash. should be called prior to any other functions as a necessary precondition for their success (all other functions should
 *      return with an error otherwise).
 * RETURN VALUE: On success 0, otherwise -1.
 */

// @TODO Add in the end tell pthread we finished
int init_blockchain()
{
	if(STARTED())
	{
		return ERROR;
	}
	gBlocksAdded = 0;
	gIsClosing = false;
	// @TODO Add more initialization?
	Block::initMaxDepth();
	gGenesis = new Block();
	gGenesis->setFather(nullptr);
	gGenesis->setId(0);
	gBlockVector.push_back(gGenesis);
	gDeepestBlocks().push_back(gGenesis);


	return 0;
}

/*
 * DESCRIPTION: Ultimately, the function adds the hash of the data to the Block chain.
 *      Since this is a non-blocking package, your implemented method should return as soon as possible, even before the Block was actually
 *      attached to the chain.
 *      Furthermore, the father Block should be determined before this function returns. The father Block should be the last Block of the
 *      current longest chain (arbitrary longest chain if there is more than one).
 *      Notice that once this call returns, the original data may be freed by the caller.
 * RETURN VALUE: On success, the function returns the lowest available block_num (> 0),
 *      which is assigned from now on to this individual piece of data.
 *      On failure, -1 will be returned.
 */
int add_block(char *data , int length)
{
	if(! STARTED())
	{
		return NOT_STARTED;
	}
    /*
     * @TODO allocates a block
     * @TODO adds data as string
     * @TODO set tentative father
     * @TODO assigns blocknum
     * @TODO adds to actions
     * @TODO return blocknum (or -1)
    */
	int blockNum = -1;
	Block block = new Block(data, length);
	block.setFather(getDeepestBlock());
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
	if(! STARTED())
	{
		return NOT_STARTED;
	}
    /*
     * @TODO Checks if the block num doesn't exists: return -2
     * @TODO Other errors(???): return -1
     * @TODO Checks if was already added(depth>0): return 1
     * @TODO Sets the block addInRealTime to true
     * @TODO Check if it's still in waiting: If yes, return 0
     *          If not, check if it was added in real time - and return 0/1
    */
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
    // even if set as real time, might've been added after we did the other check
}

/*
 * DESCRIPTION: This function blocks all other Block attachments, until block_num is added to the chain. The block_num is the assigned value
 *      that was previously returned by add_block.
 * RETURN VALUE: If block_num doesn't exist, return -2;
 *      In case of other errors, return -1; In case of success or if it is already attached return 0.
*/
int attach_now(int block_num)
{
	if(! STARTED())
	{
		return NOT_STARTED;
	}
    /*
     * @TODO Block all other block attachments
     * @TODO Check if the block num exists, otherwise return -2
     * @TODO Calls the daemon attach function
     * @TODO Return 0 on success/was already added
    */
}
/*
 * DESCRIPTION: Without blocking, check whether block_num was added to the chain.
 *      The block_num is the assigned value that was previously returned by add_block.
 * RETURN VALUE: 1 if true and 0 if false. If the block_num doesn't exist, return -2; In case of other errors, return -1.
*/
int was_added(int block_num)
{
	if(! STARTED())
	{
		return NOT_STARTED;
	}
    /*
     * @TODO return depth>=0 if exists, otherwise -2
    */
    Block* block = getBlockByBlocknum(block_num);

	if(block == nullptr)
	{
		return BLOCK_DOESNT_EXIST;
	}

	if(block->inChain())
	{
		return 1;
	}
	else
	{
		return 0;
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
	if(! STARTED())
	{
		return NOT_STARTED;
	}
    // @TODO when -1???
    return gBlocksAdded;
}

/*
 * DESCRIPTION: Search throughout the tree for sub-chains that are not the longest chain,
 *      detach them from the tree, free the blocks, and reuse the block_nums.
 * RETURN VALUE: On success 0, otherwise -1.
*/
int prune_chain()
{
	if(! STARTED())
	{
		return NOT_STARTED;
	}
    /*
     * @TODO mutex lock
     * @TODO Get deepest block
     * @TODO Build root bottom-up
     * @TODO Delete irrelevant blocks from blockVector
     * @TODO Handle queued blocks
    */

    // @TODO Mutex lock - enable
    Block* deepest = getDeepestBlock();
    deque<Block*> longestChain;
    while(deepest->getFather() != nullptr) // @TODO maybe  != gGenesis?
	{
		longestChain.push_front(deepest);
		deepest = deepest.getFather();
	}
	for(int i = 0; i < gBlockVector.size(); i++)
	{
		Block* currBlock;
		currBlock = gBlockVector.at(i);
		if(currBlock == nullptr)
		{
			continue;
		}

		if(currBlock != longestChain.at(currBlock->getDepth())) // @TODO maybe depth +/- 1?
		{
			delete gBlockVector.at(i);
			gBlockVector.at(i) = nullptr;
		}
	}
    // @TODO Mutex lock - disable
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
	if(! STARTED())
	{
		return NOT_STARTED;
	}

	gIsClosing = true;
    /*
    	* @TODO Block all actions on the blockchain
    	* @TODO Print hashed pending
    	* @TODO Signal daemon to call pthread_exit()
    	* @TODO Clear all variables and free stuff
    	* @TODO Mark we finished for return_on_close
    */
}

/*
 * DESCRIPTION: The function blocks and waits for close_chain to finish.
 * RETURN VALUE: If closing was successful, it returns 0.
 *      If close_chain was not called it should return -2. In case of other error, it should return -1.
*/ //========================GLOBALS================================


int return_on_close()
{
    /*
    	* @TODO Check if close_chain was called, otherwise return -2
    	* @TODO while(true), return 0 if it was closed
    */
    if( ! gIsClosing)
	{
		return NOT_CLOSING;
	}
	while(gIsClosing)
	{
		// @TODO Add a real statement instead of CLOSING
		// @TODO Maybe let pthread know we're waiting?
	}

	return 0;
}


int takeMinUnusedBlocknum(Block* block)
{
    /*
    	* @TODO Iterate over vector, return first nullptr, sets the value as block
    	* @TODO If non found, add the block
    	* @TODO Return and set blocknum accordingly
    */
    int blocknum = 0;
    for (auto &i : gBlockVector) {
		if(i == nullptr)
		{
			gBlockVector.at(blocknum) = block;
			return blocknum;
		}
		blocknum++;
	}
	gBlockVector.push_back(block);
	return blocknum;
}
Block* getBlockByBlocknum(int blocknum)
{
    /*
    	* @TODO Returns the block from the vector if exists, otherwise nullptr
    */
    if(blocknum >= gBlockVector.size())
	{
		return nullptr;
	}
	return gBlockVector.at(blocknum);
}


Block* getDeepestBlock()
{

}

//========================DAEMON CODE============================


// @TODO add initDaemon and init pthread, gBlocksAdded
// @TODO receive genesis, gBlockVector
void runDaemon()
{
    gBlocksAdded = 0;
}
inline void addToQueue(Block* toAdd)
{
    queueBlock.push_back(toAdd);
}

/*
void attachBlockByNum(int blocknum)
{

}
*/





