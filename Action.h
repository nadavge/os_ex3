#ifndef ACTION_H
#define ACTION_H


class Action
{
	public:
		/** Default constructor */
		Action();
		/** Default destructor */
		virtual ~Action();
		/** Access _block
		 * \return The current value of _block
		 */
		Block* getBlock() { return _block; }
		/** Set _block
		 * \param val New value to set
		 */
		void setBlock(Block* val) { _block = val; }
		/** Access _realTime
		 * \return The current value of _realTime
		 */
		boolean isRealTime() { return _realTime; }
		/** Set _realTime
		 * \param val New value to set
		 */
		void setRealTime(boolean val) { _realTime = val; }
	protected:
	private:
		Block* _block; //!< Member variable "_block"
		boolean _realTime; //!< Member variable "_realTime"
};

#endif // ACTION_H
