/*
 * Block.cpp
 * A bitcoin-inspired block for blockchain implementation
 * Authors:
 * 	Nadav Geva <nadav.geva4@gmail.com>
 * 	Daniel Danon <danondaniel@gmail.com>
 */
#include "Block.h"
#include <cstring>

using namespace std;

int Block::s_maxDepth = 0;

Block::Block() :
	_id(-1),
	_data(nullptr),
	_dataLength(-1),
	_hash(nullptr),
	_father(nullptr),
	_depth(-1),
	_toAddInRealTime(false),
	_wasAddedInRealTime(false)
{

}

bool Block::setData(char* data, int length)
{
	_data = new char[length];
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
	delete[] _hash;
	delete[] _data;
}
