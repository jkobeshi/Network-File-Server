#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata = "We hold these truths to be self-evi\0dent, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("user1", "/file", 'f');
    status = fs_create("user2", "/file1", 'f');
    status = fs_create("user3", "/file2", 'f');
    status = fs_create("user4", "/file3", 'f');
    status = fs_create("user5", "/file4", 'f');
    status = fs_create("user6", "/file5", 'f');
    status = fs_create("user7", "/file6", 'f');
    status = fs_create("user8", "/file7", 'f');
    status = fs_create("user9", "/file8", 'f');
    status = fs_create("user10", "/file9", 'f');
    status = fs_create("user11", "/file10", 'f');
    status = fs_create("user12", "/file11", 'f');
    status = fs_create("user 1", "/file12", 'f');
    assert(status);
    status = fs_create("user1", "/file13", 't');
    assert(status);
    status = fs_create("user1", "/file\n13", 'f');
    assert(status);
    status = fs_create("use\nr1", "/file13", 'f');
    assert(status);
    status = fs_create("user1", "/file14", '\n');
    assert(status);
    status = fs_create("user1", "/file15", ' ');
    assert(status);
    status = fs_create("user1", "/file16", '\0');
    assert(status);
    status = fs_create("user1", "/file\016", '\0');
    assert(status);

    status = fs_writeblock("user1", "/file", 0, writedata);
    assert(!status);

    status = fs_readblock("user1", "/file", 0, readdata);
    assert(!status);

    status = fs_readblock("user1", " /file", 0, readdata);
    assert(status);
    status = fs_readblock("user1", "/ file", 0, readdata);
    assert(status);
    
    status = fs_readblock("user1", "/file /", 0, readdata);
    assert(status);

    status = fs_readblock("user1", "/file 3", 0, readdata);
    assert(status);
    
    status = fs_writeblock("user1", "/ file", 0, writedata);
    assert(status);
    
    status = fs_writeblock("user1", "/file /", 0, writedata);
    assert(status);

    status = fs_writeblock("user1", "/file 3", 0, writedata);
    assert(status);
    
    status = fs_writeblock("user1", "/file 3", 0, writedata);
    assert(status);

    status = fs_create("u 1", "/dirrr", 'd');
    assert(status);

    status = fs_delete("u 1", "/dirrr");
    assert(status);

    status = fs_create("user1", "/1dir", 'd');
    assert(!status);

    status = fs_delete("user1", "/1dir 3da");
    assert(status);

    status = fs_create("user1", "/1dir/1file", 'f');
    assert(!status);

    status = fs_writeblock("user1", "/1dir/1file 3", 0, writedata);
    assert(status);

    status = fs_readblock("user1", "/1dir/1file 3", 0, readdata);
    assert(status);
    
    status = fs_writeblock("user1", "/1dir/1file", 00, writedata);
    assert(!status);

    status = fs_readblock("user1", "/1dir/1file", 01, readdata);
    assert(status);


    status = fs_delete("user1", "file2");
    assert(status);
    




    
}