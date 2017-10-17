#! /bin/sh

./fdopen_t abc.txt r r
sleep 1
./fdopen_t abc.txt r r+
sleep 1
./fdopen_t abc.txt r w
sleep 1
./fdopen_t abc.txt r w+
sleep 1
./fdopen_t abc.txt r a
sleep 1
./fdopen_t abc.txt r a+
sleep 1
./fdopen_t abc.txt w r
sleep 1
./fdopen_t abc.txt w r+
sleep 1
./fdopen_t abc.txt w w
sleep 1
./fdopen_t abc.txt w w+
sleep 1
./fdopen_t abc.txt w a
sleep 1
./fdopen_t abc.txt w a+
sleep 1
./fdopen_t abc.txt a r
sleep 1
./fdopen_t abc.txt a r+
sleep 1
./fdopen_t abc.txt a a
sleep 1
./fdopen_t abc.txt a a+
sleep 1
./fdopen_t abc.txt a w
sleep 1
./fdopen_t abc.txt a w+
sleep 1
./fdopen_t abc.txt r+ w
sleep 1
./fdopen_t abc.txt r+ r
sleep 1
./fdopen_t abc.txt r+ r+
sleep 1
./fdopen_t abc.txt r+ w+
sleep 1
./fdopen_t abc.txt r+ a
sleep 1
./fdopen_t abc.txt r+ a+
sleep 1
./fdopen_t abc.txt w+ r
sleep 1
./fdopen_t abc.txt w+ r+
sleep 1
./fdopen_t abc.txt w+ w
sleep 1
./fdopen_t abc.txt w+ w+
sleep 1
./fdopen_t abc.txt w+ a
sleep 1
./fdopen_t abc.txt w+ a+
sleep 1
./fdopen_t abc.txt a+ r
sleep 1
./fdopen_t abc.txt a+ r+
sleep 1
./fdopen_t abc.txt a+ w
sleep 1
./fdopen_t abc.txt a+ w+
sleep 1
./fdopen_t abc.txt a+ a
sleep 1
./fdopen_t abc.txt a+ a+
sleep 1
