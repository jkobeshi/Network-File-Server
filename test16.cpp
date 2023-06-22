#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);


    // user 1 dir
    status = fs_create("u1", "/1dA", 'd');
    assert(!status);

    status = fs_create("u1", "/1dB", 'd');
    assert(!status);

    status = fs_create("u1", "/1dA/dA", 'd');
    assert(!status);

    status = fs_create("u1", "/1dA/dB", 'd');
    assert(!status);


    // user 2 dir
    status = fs_create("u2", "/2dA", 'd');
    assert(!status);

    status = fs_create("u2", "/2dB", 'd');
    assert(!status);

    status = fs_create("u2", "/2dA/dA", 'd');
    assert(!status);

    status = fs_create("u2", "/2dA/dB", 'd');
    assert(!status);


    // user 3 dir
    status = fs_create("u3", "/3dA", 'd');
    assert(!status);
    
    status = fs_create("u3", "/3dB", 'd');
    assert(!status);

    status = fs_create("u3", "/3dA/dA", 'd');
    assert(!status);

    status = fs_create("u3", "/3dA/dB", 'd');
    assert(!status);


    // create files
    status = fs_create("u1", "/1dA/dB/f1", 'f');
    assert(!status);
    status = fs_create("u2", "/2dA/dB/f1", 'f');
    assert(!status);
    status = fs_create("u3", "/3dA/dB/f1", 'f');
    assert(!status);

    status = fs_writeblock("u3", "/3dA/dB/f1", 0, writedata);
    assert(!status);
    status = fs_writeblock("u3", "/3dA/dB/f1", 1, writedata);
    assert(!status);

    status = fs_readblock("u3", "/3dA/dB/f1", 1, readdata);
    assert(!status);
    status = fs_readblock("u3", "/3dA/dB/f1", 0, readdata);
    assert(!status);

    status = fs_writeblock("u3", "/3dA/dB/f1", 2, readdata);
    assert(!status);

    status = fs_delete("u3", "/3dA/dB/f1");
    assert(!status);
}