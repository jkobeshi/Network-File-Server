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

    status = fs_create("user1", "/dir", 'd');
    assert(!status);

    status = fs_create("user1", "/dirajdwhuyvhjbiudawbhjuiadwyuaidwawdadwuhuiawdhuiawdhuiawdhuiadhuiawdhiuygbuigafuigihafuiygiuabuigboubaefyugiusybuhishuhshuseihsuhsuihiuefhuihsfuiguifouiafguiafgiouaguiafguihasfuihasdiuhuiadhuidawhiuadwhuiahdfiuadhuiadhoifhaoishoidhaoiwhdiahjshaflishaoroiuawbiufagofuhaouishdfioahfdiuohosdihfoqahfioawghfduioagwfigawlig", 'd');
    assert(status);

    status = fs_create("asiodjawoijdawojdwoiajd", "/mop", 'f');
    assert(status);

    status = fs_create("user4", "/mop", 'f');
    assert(!status);

    status = fs_create("user4", "/mop/iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii", 'd');
    assert(status);

    status = fs_create("user1", "/dir/file", 'f');
    assert(!status);

    status = fs_create("user1", "/dir/file", 'f');
    assert(status);

    status = fs_create("user2", "/dir", 'd');
    assert(status);

    status = fs_create("user2", "/dir/file", 'f');
    assert(status);

    for(int i = 0; i < 124; ++i){
        status = fs_writeblock("user1", "/dir/file", i, writedata);
        assert(!status);
    }

    status = fs_delete("user2", "/dir/file");
    assert(status);

    status = fs_delete("user1", "/dir/file");
    assert(!status);

    status = fs_delete("user1", "/dir/file");
    assert(status);

    status = fs_delete("user1", "/dir/file");
    assert(status);

    status = fs_create("user1", "/dir/file", 'f');
    assert(!status);

    status = fs_readblock("user1", "/dir/file", 0, readdata);
    assert(status);

    status = fs_delete("user1", "/dir/file");
    assert(!status);

    status = fs_delete("user1", "/dir/file");
    assert(status);

    status = fs_create("user1/", "/", 'd');
    assert(status);

    status = fs_create("user1/", "/ ", 'd');
    assert(status);

    status = fs_create("usiuguigvluguilgliuguiguiguigler1/", "/7", 'd');
    assert(status);

    status = fs_create("user3", "/dir", 'd');
    assert(status);

    status = fs_create("user3", "/dir1/dir", 'd');
    assert(status);

    status = fs_create("user3", "/dir1", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir1", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir2", 'd');
    assert(!status);

    status = fs_delete("user3", "/dir1");
    assert(status);

    status = fs_create("user3", "/dir1/dir3", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir4", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir5", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir6", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir7", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir8", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir9", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir10", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir11", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir12", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir13", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir14", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir15", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir16", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir17", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir18", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir19", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir20", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir21", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir22", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir23", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir24", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir25", 'd');
    assert(!status);

    status = fs_create("user3", "/dir1/dir26", 'd');
    assert(!status);

    status = fs_delete("user1", "/");
    assert(status);

    status = fs_delete("user1", "/ ");
    assert(status);

    status = fs_writeblock("user1", "/", 0, writedata);
    assert(status);

    status = fs_readblock("user1", "/", 0, readdata);
    assert(status);

    status = fs_writeblock("user3", "/dir1", 0, writedata);
    assert(status);

    status = fs_readblock("user3", "/dir1", 0, readdata);
    assert(status);

    status = fs_delete("user4", "/mop");
    assert(!status);

    status = fs_delete("user4", "/mop");
    assert(status);
    status = fs_delete("user4", "/mop");
    assert(status);

    status = fs_delete("user3", "/dir1/dir1");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir2");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir3");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir4");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir5");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir6");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir7");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir8");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir9");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir10");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir11");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir12");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir13");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir14");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir15");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir16");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir17");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir18");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir19");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir20");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir21");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir22");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir23");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir24");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir25");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir");
    assert(!status);
    status = fs_delete("user3", "/dir1/dir26");
    assert(!status);
}