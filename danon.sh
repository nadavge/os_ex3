cd ~/safe/os/ex3;
make remake;
mv -f ./libblockchain.a ./tests
cd tests;
make clean;
make test1;
echo Done!
