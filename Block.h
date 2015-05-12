#ifndef BLOCK_H
#define BLOCK_H

#define HASH_SIZE (16+1)
class Block
{
	public:
		/** Default constructor */
		Block();
		/** Default destructor */
		virtual ~Block();
		/** Access _id
		 * \return The current value of _id
		 */
		int getId() { return _id; }
		/** Set _id
		 * \param val New value to set
		 */
		void setId(int val) { _id = val; }
		/** Access _data
		 * \return The current value of _data
		 */

		//TODO change documentation
		int getDepth()
		{
			return _depth;
		}

		void setDepth(int depth)
		{
			_depth = depth;
		}

		char* getData() { return _data; }
		int getDataLength() { return _dataLength; }
		/** Set _data
		 * \param val New value to set
		 */
		bool setData(char* data, int length);
bool inChain()
{
    return _depth >= 0;
}
		char* getHash() { return _hash; }
		/** Set _hash[HASH_LENGTH]
		 * \param val New value to set
		 */
		void setHash(char* blockHash);
bool wasAddedInRealTime()
{
    return _wasAddedInRealTime;
}

bool toAddInRealTime()
{
    return _toAddInRealTime;
}

void setAddInRealTime()
{
    _toAddInRealTime = true;
}

static int getMaxDepth()
{
    return s_maxDepth;
}

static void setMaxDepth(int depth)
{
	s_maxDepth = depth;
}

static int initMaxDepth()
{
    s_maxDepth = 0;
}
		/** Access _father
		 * \return The current value of _father
		 */
		Block* getFather() { return _father; }
		/** Set _father
		 * \param val New value to set
		 */
		void setFather(Block* val) { _father = val; }
	protected:
	private:
		int _id; //!< Member variable "_id"
		char* _data; //!< Member variable "_data"
		int _dataLength;
		char* _hash; //!< Member variable "_hash[HASH_LENGTH]"
		Block* _father; //!< Member variable "_father"
		int _depth;
		static int s_maxDepth;
		bool _toAddInRealTime;
		bool _wasAddedInRealTime;
};

#endif // BLOCK_H
