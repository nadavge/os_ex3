#ifndef BLOCK_H
#define BLOCK_H


class Block
{
	public:
		/** Default constructor */
		Block();
		/** Default destructor */
		virtual ~Block();
		/** Copy constructor
		 *  \param other Object to copy from
		 */
		Block(const Block& other);
		/** Assignment operator
		 *  \param other Object to assign from
		 *  \return A reference to this
		 */
		Block& operator=(const Block& other);
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
		// @TODO Decide whether _data should be char array (and length) or String
		String _data; //!< Member variable "_data"

		vector <Block*> _children; //!< Member variable "_children"
		char _hash[HASH_LENGTH]; //!< Member variable "_hash[HASH_LENGTH]"
		Block* _father; //!< Member variable "_father"
		int _depth;
		static int s_maxDepth;
		bool _toAddInRealTime;
		bool _wasAddedInRealTime;
};

#endif // BLOCK_H
