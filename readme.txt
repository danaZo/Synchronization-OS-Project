how to run:
    use make all to compile all the parts

testing:
    to test parts 1-3 run the regular server with (./server)
    and then the test (./test_server)

    to test part 4 use make test_guard and run it
    the tests are based on asserts so if the executable run without throwing the test was succsefull

    to test part 5 use make test_singletom and run it
    the tests are based on asserts so if the executable run without throwing the test was succsefull

    to test part 6 use make all, run pollserver (./pserver) and 2 pollclients (./pclient) from 3 different terminals
    send message from one client and see that the other client get it

Outer source

    for part 4, we looked at the guard.cpp code that was shown on the practice with yoval

    for part 5 we looked on https://stackoverflow.com/questions/42555782/singleton-with-template
    and used a little part of it

    for part 6 we used parts of the beej poll server