/*
 * Block.h
 * A bitcoin-inspired block for a blockchain definition
 * Authors:
 * 	Nadav Geva <nadav.geva4@gmail.com>
 * 	Daniel Danon <danondaniel@gmail.com>
 */
#ifndef BLOCK_H
#define BLOCK_H

#define HASH_SIZE (16+1)

/*
 * The class representing a block in the blockchain for transactions
 */
class Block
{
public:
	// Default constructor
	Block();
	
	// Default destructor
	virtual ~Block();

	/*
	 * @brief Getter for the block ID
	 *
	 * @return The block id
	 */
	int getId()
	{
		return _id;
	}

	/*
	 * @brief Set the block ID
	 *
	 * @param id the id to set
	 */
	void setId(int id) 
	{
		_id = id;
	}

	/*
	 * @brief Get the block depth 
	 *
	 * @return The depth of the block
	 */
	int getDepth()
	{
		return _depth;
	}

	/*
	 * @brief Set the block depth
	 *
	 * @param depth the depth to set
	 */
	void setDepth(int depth)
	{
		_depth = depth;
	}

	/*
	 * @brief Get the block data pointer 
	 *
	 * @return the block data
	 */
	char* getData()
	{
		return _data;
	}

	/*
	 * @brief Get the block data length
	 *
	 * @return the data length
	 */
	int getDataLength()
	{
		return _dataLength;
	}
	
	/*
	 * @brief Set the block data
	 *
	 * @param data the data to copy 
	 * @param length the length of the data to copy in chars
	 *
	 * @return True iff the copy was successful
	 */
	bool setData(char* data, int length);

	/*
	 * @brief Check if the block is in the chain
	 *
	 * @return True iff the block is currently in the chain
	 */
	bool inChain()
	{
			return _depth >= 0;
	}

	/*
	 * @brief Get the block hash
	 *
	 * @return The block hash
	 */
	char* getHash()
	{
		return _hash;
	}

	/*
	 * @brief Set the block hash
	 *
	 * @param blockHash the hash to set
	 */
	void setHash(char* blockHash);

	/*
	 * @brief Check if the block's father was calculated in real-time
	 *
	 * @return true iff the block's father was calculated in real-time
	 */
	bool wasAddedInRealTime()
	{
		return _wasAddedInRealTime;
	}

	/*
	 * @brief Set the block flag to "father calculated in real-time"
	 */
	void setWasAddedInRealTime()
	{
		_wasAddedInRealTime = true;
	}

	/*
	 * @brief Check if the block's father is to be calculated in real-time
	 *
	 * @return True iff the father is to be calculated in real-time
	 */
	bool toAddInRealTime()
	{
		return _toAddInRealTime;
	}

	/*
	 * @brief Mark the block, so that on add its father would be caclulated in real-time
	 */
	void setAddInRealTime()
	{
			_toAddInRealTime = true;
	}

	/*
	 * @brief Get the blocks father as pointer
	 *
	 * @return Pointer to the block's father
	 */
	Block* getFather()
	{
		return _father;
	}
	
	/*
	 * @brief Set the block's father
	 *
	 * @param father the father's pointer
	 */
	void setFather(Block* father)
	{
		_father = father;
	}

	/*
	 * @brief Get the depth of the deepest block in the chain
	 *
	 * @return the depth of the chain
	 */
	static int getMaxDepth()
	{
		return s_maxDepth;
	}

	/*
	 * @brief Set the chain's depth
	 *
	 * @param depth The chain's new depth
	 */
	static void setMaxDepth(int depth)
	{
		s_maxDepth = depth;
	}

	/*
	 * @brief Init the max depth of the chain to 0
	 */
	static void initMaxDepth()
	{
		s_maxDepth = 0;
	}

private:
	int _id; // The blocknum
	char* _data; // The blocks data
	int _dataLength; // The length of the data in chars
	char* _hash; // The blocks hash
	Block* _father; // The blocks father
	int _depth; // The blocks depth
	bool _toAddInRealTime; // Whether to find the block's father in time of addition (to_longest)
	bool _wasAddedInRealTime; // Whether the father of the block was found in real-time
	
	static int s_maxDepth; // The maximal depth of the chain
};

#endif // BLOCK_H
