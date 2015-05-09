#include "Block.h"
// TODO change const init order
Block::Block() :
    _id(-1),
    _children(new vector<Block*>()),
    _depth(-1),
    _wasAddedInRealTime(false),
    _toAddInRealTime(false),
    _dataLength(-1),
	_data(nullptr),
	_hash(nullptr)
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
void Block::setHash(char* blockHash)
{
	_hash = blockHash;
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
    delete _hash;
    delete _data;
}
