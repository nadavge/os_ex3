#include "Block.h"

Block::Block() : _id(-1), ), _data(""), _children(new vector<Block*>())
{
	//ctor

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
