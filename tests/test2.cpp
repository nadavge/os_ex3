/*
 * test0.cpp
 *
 *  Created on: 5 ���� 2015
 *      Author: roigreenberg
 */

#include <unistd.h>
#include <iostream>
#include "../blockchain.h"
#include "../hash.h"

using namespace std;
void printChain()
{
	
}

void tester()
{
	cout << "********* testing init_blockchain **********" << endl;

	cout << "********* testing try to call all methods before init, should print -1 except maybe return_on_close **********" << endl;

	char l[] = "lll";
	char* pl = l;
	cout << "add block:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "to_longest:" << endl;
	cout << to_longest(1) << endl;

	cout << "attach_now:" << endl;
	cout << attach_now(1) << endl;

	cout << "was_added:" << endl;
	cout << was_added(1) << endl;

	cout << "chain_size:" << endl;
	cout << chain_size() << endl;

	cout << "prune_chain:" << endl;
	cout << prune_chain() << endl;

	cout << "return_on_close:" << endl;
	cout << return_on_close() << endl;

	cout << "********* calling to init twice, the second time should return -1 **********" << endl;

	init_hash_generator();

	cout << "init_blockchain:" << endl;
	cout << init_blockchain() << endl;

	cout << "init_blockchain:" << endl;
	cout << init_blockchain() << endl;

	cout << "********* testing add_block **********" << endl;

	cout << "add 1:" << endl;
	cout << add_block(pl, 4) << endl;
	cout << "add 2:" << endl;
	cout << add_block(pl, 4) << endl;
	cout << "add 3:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "chain should be empty now:" << endl;

	printChain();

	cout << "wait for adding to chain" << endl;;

	while (chain_size() == 0);

	cout << "somthing added!" << endl;;

	cout << "chain should contain one value:" << endl;

	printChain();

	cout << "********* testing to_longest **********" << endl;

	cout << "calling to longest on 1 which was already added, should print 1" << endl;

	cout <<to_longest(1)<< endl;

	cout << "calling to longest on 10 which don't exist , should print -2" << endl;

	cout <<to_longest(10)<< endl;


	cout << "calling to longest on 3 " << endl;

	to_longest(3);
	cout << "3 is now to_longest" << endl;

	while (chain_size() < 3);
	cout << "all blocks were added!" << endl;

	printChain();


	cout << "********* testing attach_now **********" << endl;

	cout << "calling attach_now on 1 which was already added, should print 1" << endl;

	cout <<attach_now(1)<< endl;

	cout << "calling attach_now on 10 which don't exist , should print -2" << endl;

	cout <<attach_now(10)<< endl;

	cout << "add 4:" << endl;
	cout << add_block(pl, 4) << endl;
	cout << "add 5:" << endl;
	cout << add_block(pl, 4) << endl;
	cout << "add 6:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "6 is being attach now" << endl;

	attach_now(6);

	cout << "waiting to add 6" << endl;

	while (chain_size() < 4);

	cout << "6 should be on your chain now" << endl;

	printChain();


	cout << "********* testing was_added **********" << endl;

	cout << "calling was_added on 1 which was already added, should print 1" << endl;

	cout <<was_added(1)<< endl;

	cout << "calling was_added on 10 which don't exist , should print -2" << endl;

	cout <<was_added(10)<< endl;

	cout << "calling was_added on 5  , should print 0" << endl;

	cout <<was_added(5)<< endl;

	cout << "********* testing chain_size **********" << endl;

	cout << "check if the size correspond to the next print, notice that you dont count genesis" << endl;

	printChain();

	cout << "size: " << chain_size()<< endl;

	cout << "********* testing prune_chain **********" << endl;

	printChain();
	cout << "calling prune" << endl;

	prune_chain();

	cout << "after prune" << endl;

	printChain();

}


int main() {

	cout << "********* start test **********" << endl;


	tester();

	cout << "********* testing close_chain **********" << endl;

	cout << "calling close chain" << endl;

	close_chain();

	cout << "running all testers again after closing chain" << endl;

	tester();

	cout << "********* testing return_on_close **********" << endl;

	cout << "try to call return on close before close chain , should return -2" << endl;

	return_on_close();

	cout << "calling close chain and after that call return_on_close " << endl;

	close_chain();
	return_on_close();


	cout << "********* test end **********" << endl;
	return 0;
}
