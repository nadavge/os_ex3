#include "Block.h"

Block::Block() :
    _id(-1),
    _children(new vector<Block*>()),
    _depth(-1),
    _wasAddedInRealTime(false),
    _toAddInRealTime(false),
    _dataLength(-1),
	_data(nullptr)
{

}
bool Block::setData(char* data, int length)
{
	_data = malloc(length*sizeof(char));
	if(_data == nullptr)
	{
		_dataLength = -1;
		return false;
	}

	memcpy(_data, data, length*sizeof(char));

	return true;
}
char* Block::getData()
{
	return _data;
}
int Block::getDataLength()
{
	return _dataLength;
}
bool Block::inChain()
{
    return _depth >= 0;
}
bool Block::wasAddedInRealTime()
{
    return _wasAddedInRealTime;
}

bool Block::toAddInRealTime()
{
    return _toAddInRealTime;
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
