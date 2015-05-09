#ifndef BLOCK_H
#define BLOCK_H

#define HASH_SIZE (16+1)
class Block
{
	public:
		/** Default constructor */
		Block(char* data, int length);
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
		String getData() { return _data; }
		/** Set _data
		 * \param val New value to set
		 */
		void getData(String val) { _data = val; }
		/** Access _children
		 * \return The current value of _children
		 */
		vector <Block*> getChildren() { return _children; }
		/** Set _children
		 * \param val New value to set
		 */
		void setChildren(vector <Block*> val) { _children = val; }
		/** Access _hash[HASH_LENGTH]
		 * \return The current value of _hash[HASH_LENGTH]
		 */
		char getHash[HASH_LENGTH]() { return _hash[HASH_LENGTH]; }
		/** Set _hash[HASH_LENGTH]
		 * \param val New value to set
		 */
		void setHash[HASH_LENGTH](char val) { _hash[HASH_LENGTH] = val; }
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
		// TODO free data
		vector <Block*> _children; //!< Member variable "_children"
		char _hash[HASH_LENGTH]; //!< Member variable "_hash[HASH_LENGTH]"
		Block* _father; //!< Member variable "_father"
		int _depth;
		static int s_maxDepth;
		bool _toAddInRealTime;
		bool _wasAddedInRealTime;
};

#endif // BLOCK_H
