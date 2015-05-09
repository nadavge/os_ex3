#include "Block.h"

Block::Block(char* data, int length) : _id(-1), _children(new vector<Block*>()), _depth(-1), _wasAddedInRealTime(false), _toAddInRealTime(false)
{
	// @TODO convert data to string

}

bool Block::inChain()
{
	return _depth >= 0;
}
bool Block::wasAddedInRealTime()
{
	return _wasAddedInRealTime;
}

void Block::setAddInRealTime()
{
	_toAddInRealTime = true;
}

static int Block::getMaxDepth()
{
	return s_maxDepth;
}
static int Block::initMaxDepth()
{
	s_maxDepth = -1;
}

Block::~Block()
{
	//dtor
}
/*
Block::Block(const Block& other)
{
	//copy ctor
}

Block& Block::operator=(const Block& rhs)
{
	if (this == &rhs) return *this; // handle self assignment
	//assignment operator
	return *this;
}
*/
