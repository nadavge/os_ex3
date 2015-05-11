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



Block::~Block()
{
    delete _hash;
    delete _data;
}
