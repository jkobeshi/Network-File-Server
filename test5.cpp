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

    fs_readblock("user1", "/dir/dir2/dir4/file", 0, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 1, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 2, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 3, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 4, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 5, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 6, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 7, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 8, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 9, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 10, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 11, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 12, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 13, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 14, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 15, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 16, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 17, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 18, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 19, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 20, readdata);
    fs_readblock("user1", "/dir/dir2/dir4/file", 21, readdata);
















    status = fs_create("user1", "/dir/dir3/dir4/dir7", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/dir3/file10", 'f');
    assert(!status);
    
    status = fs_writeblock("user1", "/dir/dir3/file10", 0, writedata);
    assert(!status);
    
    status = fs_writeblock("user1", "/dir/dir3/file10", 1, writedata);
    assert(!status);
    
    status = fs_writeblock("user1", "/dir/dir3/file10", 2, writedata);
    assert(!status);
    
    status = fs_writeblock("user1", "/dir/dir3/file10", 3, writedata);
    assert(!status);

    status = fs_readblock("user1", "/dir/dir3/file10", 3, readdata);
    assert(!status);

    status = fs_readblock("user1", "/dir/dir3/file10", 0, readdata);
    assert(!status);

    status = fs_readblock("user1", "/dir/dir3/file10", 1, readdata);
    assert(!status);

    status = fs_readblock("user1", "/dir/dir2/dir4/file", 0, readdata);
    assert(!status);
    status = fs_writeblock("user1", "/dir/dir3/dir4/file2", 0, writedata);
    assert(!status);
    status = fs_readblock("user1", "/dir/dir3/dir4/file2", 0, readdata);
    assert(!status);
    
    status = fs_delete("user1", "/dir/dir3/dir4/file2");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3/dir4/file");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3/dir4/dir7");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3/dir4");
    assert(!status);

    status = fs_delete("user1", "/dir/dir2/dir4/file2");
    assert(!status);

    status = fs_delete("user1", "/dir/dir2/dir4/file");
    assert(!status);

    status = fs_delete("user1", "/dir/dir2/dir4");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3/file2");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3/file");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3/file10");
    assert(!status);

    status = fs_delete("user1", "/dir/dir3");
    assert(!status);

    status = fs_delete("user1", "/dir/dir2/file2");
    assert(!status);

    status = fs_delete("user1", "/dir/dir2/file");
    assert(!status);

    status = fs_delete("user1", "/dir/dir2");
    assert(!status);

    status = fs_delete("user1", "/dir/file2");
    assert(!status);

    status = fs_delete("user1", "/dir/file");
    assert(!status);

    status = fs_delete("user1", "/dir");
    assert(!status);

}