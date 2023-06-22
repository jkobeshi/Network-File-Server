#include "fs_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <unordered_map>
#include <sstream>
#include <queue>


std::priority_queue<int, std::vector<int>, std::greater<int>> index_to_disk;
//concurrency map inode-index to mutex pointer
std::unordered_map<int, std::mutex*> mutex_map;

//return pointer to next lock
int avail_in_disk(){
    if(index_to_disk.empty()){
        return -1;
    }
    else{
        int ind = index_to_disk.top();
        index_to_disk.pop();
        return ind;
    }
}

//returns inode of last fd in pathname
std::pair<fs_inode, int> node_traversal(std::string pathname, int start_block, int client_sock, std::string owner) {
    // root node
    fs_inode inode_temp;
    int block = start_block;
    
    //conc: hand over hand locking - acquire start blocks lock
    mutex_map[block]->lock();
    disk_readblock(start_block, &inode_temp);
    
    std::string temp = "";
    for(unsigned int i = 0; i < pathname.size(); i++) {
        if(pathname[i] != '/'){
            temp += pathname[i];
        }
        if((pathname[i] == '/' || i == pathname.size() - 1) && (i != 0)) {
            bool found = 0;
            for(uint32_t k = 0; k < inode_temp.size; k++) {
                fs_direntry entries[FS_DIRENTRIES];
                disk_readblock(inode_temp.blocks[k], &entries);
                for(unsigned int j = 0; j < FS_DIRENTRIES; j++) {
                    if(std::string(entries[j].name) == temp && entries[j].inode_block != 0) {
                        temp = "";
                        //conc: reading new inodes
                        mutex_map[entries[j].inode_block]->lock();
                        mutex_map[block]->unlock();
                        block = entries[j].inode_block;

                        disk_readblock(entries[j].inode_block, &inode_temp);

                        //has to be same owner
                        if(std::string(inode_temp.owner) != owner){
                            close(client_sock);
                            mutex_map[block]->unlock();
                            return std::pair(inode_temp, -1);
                        }

                        //Error checking: is empty directory or file before last slash
                        if((inode_temp.size == 0 || inode_temp.type == 'f') && i != pathname.size() - 1) {
                            close(client_sock);
                            mutex_map[block]->unlock();
                            return std::pair(inode_temp, -1);
                        }
                        found = 1;
                        break;
                    }
                }
                if(found){
                    break;
                }
            }
            //error checking: entry does not exist
            if(!found) {
                close(client_sock);
                mutex_map[block]->unlock();
                return std::pair(inode_temp, -1);
            }
        }
    }
    return std::pair(inode_temp, block);
}

//clinet_sock comes from accept()
int handleConnection(int client_sock) {
    //Recieve request: byte by byte recieve header before data
    try
    {
        /* code */
    
    
    
    int count = 0;
    char buf[FS_BLOCKSIZE];
    do {
        int rval = recv(client_sock, buf + count, 1, 0);
        if(rval == -1) {
            close(client_sock);
            return -1;
        }
        count += rval;
    } while (buf[count - 1] != '\0' && count < (int)FS_BLOCKSIZE);  // recv() returns 0 when client closes
    
    //General case variables
    std::stringstream s(buf);
    std::string request = "";
    std::string username = "";
    std::string pathname = "";
    char readData[FS_BLOCKSIZE];
    
    //General Case: All requests have request and username
    s >> request >> username >> pathname;

    // 5.7 hint: Recomposed requests
    std::string recomposed_request = "";
    recomposed_request = request + " " + username + " " + pathname;

    //Error Checking: request header checking length - TODO: whitespace checking(completed, recomposed request)
    if((username.length() > FS_MAXUSERNAME) || 
        (pathname.length() > FS_MAXPATHNAME)){
        close(client_sock);
        return -1;
    }
    
    //error checking: must start with /, must not end with /
    if(pathname[0] != '/' || pathname[pathname.size() - 1] == '/') {
        close(client_sock);
        return -1;
    }

    //error checking: malformed requests
    int last_slash = -1;
    std::string temp_dir_name = "";
    for(unsigned int i = 0; i < pathname.size(); i++) {
        if(pathname[i] != '/'){
            temp_dir_name += pathname[i];
        }
        else{
            last_slash = i;
        }
        if(pathname[i] == '/' || i == pathname.size() - 1) {
            if(temp_dir_name.size() > FS_MAXFILENAME) {
                close(client_sock);
                return -1;
            }
            temp_dir_name = "";
        }
    }
    //no slash in pathname, invalid
    if (last_slash == -1) { 
        close(client_sock);
        return -1;
     }


    //dirPathName is the name of everything before the last slash
    //entryName everything after lasat slash
    std::string dirPathName = pathname.substr(0, last_slash);
    std::string entryName = pathname.substr(last_slash + 1, pathname.length());


    //FS_CREATE checking*
    if(request == "FS_CREATE") {
        char type = '\0';
        s >> type;

        //Error checking: Hint 5.7
        recomposed_request = recomposed_request + " " + type + '\0';
        if(strcmp(recomposed_request.c_str(), buf) != 0) {
            close(client_sock);
            return -1;
        }

        //Error Checking: Invalid Type
        if(type != 'f' && type != 'd'){
            close(client_sock);
            return -1;
        }
        
        // grab lock of parent dir
        std::pair<fs_inode, int> path = node_traversal(dirPathName, 0, client_sock, username);
        if(path.second == -1){
            return -1;
        }
        fs_inode cd = path.first;
        int cd_inode_block = path.second;

        //Error checking: Type of current directory is not a directory
        if(cd.type == 'f'){
            close(client_sock);
            mutex_map[cd_inode_block]->unlock();
            return -1;
        }
        
        //Traverse through direntries
        fs_direntry entries[FS_DIRENTRIES]; 
        int indexToEntry = 0; 
        int entryBlock = -1;
        for(uint32_t i = 0; i < cd.size; ++i){
            fs_direntry temp_entries[FS_DIRENTRIES];
            disk_readblock(cd.blocks[i], &temp_entries);
            for(unsigned int j = 0; j < FS_DIRENTRIES; ++j){
                
                //Error Checking: Entry Exists
                if(std::string(temp_entries[j].name) == entryName && temp_entries[j].inode_block != 0){
                    close(client_sock);
                    mutex_map[cd_inode_block]->unlock();
                    return -1;
                }

                //Finding lowest unused location
                if((temp_entries[j].inode_block == 0) && (entryBlock == -1)){
                    for(unsigned int k = 0; k < FS_DIRENTRIES; ++k){
                        entries[k] = temp_entries[k];
                    }
                    entryBlock = cd.blocks[i];
                    indexToEntry = j;
                }
            }
        }

        //Error checking: No block space for new dir
        if((entryBlock == -1) && (cd.size >= FS_MAXFILEBLOCKS)){
            close(client_sock);
            mutex_map[cd_inode_block]->unlock();
            return -1;
        }

        //Error checking: Reserving disk space
        if(index_to_disk.size() < 2 && entryBlock == -1){
            close(client_sock);
            mutex_map[cd_inode_block]->unlock();
            return -1;
        }

        int newInodeBlock = avail_in_disk();
        if(newInodeBlock == -1){
            close(client_sock);
            mutex_map[cd_inode_block]->unlock();
            return -1;
        }

        //concurrency - declare mutex pointer for new inode and map
        std::mutex* new_mutex = new std::mutex();
        mutex_map[newInodeBlock] = new_mutex;

        mutex_map[newInodeBlock]->lock();
        //create new Inode 
        fs_inode newInode;
        for(unsigned int i = 0; i < username.length(); ++i){
            newInode.owner[i] = username[i];
        }
        newInode.owner[username.length()] = '\0';
        newInode.type = type;
        newInode.size = 0;
        disk_writeblock(newInodeBlock, &newInode);
        mutex_map[newInodeBlock]->unlock();

        //create new entry 
        fs_direntry newEntry;
        for(unsigned int i = 0; i < entryName.length(); ++i){
            newEntry.name[i] = entryName[i];
        }
        newEntry.name[entryName.length()] = '\0';
        newEntry.inode_block = newInodeBlock;


        // case if no space is found, make new directories
        if(entryBlock == -1) {
            int newEntryBlock = avail_in_disk();
            if(newEntryBlock == -1){ // no open disk spots
                close(client_sock);
                mutex_map[cd_inode_block]->unlock();
                return -1;
            }
            //initialize and increase size
            cd.blocks[cd.size] = newEntryBlock;
            ++cd.size;
            
            fs_direntry newEntries[FS_DIRENTRIES];
            newEntries[0] = newEntry;
            for(unsigned int i = 1; i < FS_DIRENTRIES; ++i){
                newEntries[i].inode_block = 0;
            }
            disk_writeblock(newEntryBlock, &newEntries);
            disk_writeblock(cd_inode_block, &cd);
        }
        // case if space is found
        else {
            entries[indexToEntry] = newEntry;
            disk_writeblock(entryBlock, &entries);
        }
        // conc- unlock when done
        mutex_map[cd_inode_block]->unlock();
    }
    //FS_READBLOCK*
    else if(request == "FS_READBLOCK") {
        std::string temp = "";
        s >> temp;

        //Error checking: Hint 5.7
        recomposed_request = recomposed_request + " " + temp + '\0';
        if(strcmp(recomposed_request.c_str(), buf) != 0) {
            close(client_sock);
            return -1;
        }
        uint32_t block;
        try
        {
            block = stoi(temp); 
        }
        catch(const std::exception& e)
        {
            close(client_sock);
            return -1;
        }

        //Error checking: Block out of range
        if(block >= FS_MAXFILEBLOCKS || block < 0){
            close(client_sock);
            return -1;
        }

        std::pair<fs_inode, int> path = node_traversal(pathname, 0, client_sock, username);
        if(path.second == -1){
            return -1;
        }
        fs_inode cd = path.first;

        //Error checking: not a file
        if(cd.type == 'd'){
            close(client_sock);
            mutex_map[path.second]->unlock();
            return -1;
        }

        //Error checking: Reading unused block
        if(block >= cd.size){
            close(client_sock);
            mutex_map[path.second]->unlock();
            return -1;

        }
        disk_readblock(cd.blocks[block], readData);
        //conc unlock when done
        mutex_map[path.second]->unlock();
    }
    //FS_WRITEBLOCK*
    else if(request == "FS_WRITEBLOCK") {
        std::string temp = "";
        s >> temp;

        //Error checking: Hint 5.7
        recomposed_request = recomposed_request + " " + temp + '\0';
        if(strcmp(recomposed_request.c_str(), buf) != 0) {
            close(client_sock);
            return -1;
        }
        
        uint32_t block;
        try
        {
            block = stoi(temp);
        }
        catch(const std::exception& e)
        {
            close(client_sock);
            return -1;
        }

        //Error checking: blocks out of range
        if(block >= FS_MAXFILEBLOCKS || block < 0){
            close(client_sock);
            return -1;
        }

        // recieve data
        unsigned int wCount = 0;
        char wBuffer[FS_BLOCKSIZE];
        do {
            int wReceiveSize = recv(client_sock, wBuffer + wCount, 1, 0);
            if(wReceiveSize == -1){
                close(client_sock);
                return -1;
            }
            wCount += wReceiveSize;
        } while (wCount < FS_BLOCKSIZE);

        //Node traversal
        std::pair<fs_inode, int> path = node_traversal(pathname, 0, client_sock, username);
        if(path.second == -1){
            return -1;
        }
        fs_inode cd = path.first;
        int cd_inode_block = path.second;

        //Error checking: not a file
        if(cd.type == 'd'){
            close(client_sock);
            mutex_map[path.second]->unlock();
            return -1;

        }

        //Error checking: not next block
        if(block > cd.size){
            close(client_sock);
            mutex_map[path.second]->unlock();
            return -1;
        }
        
        //if block == cd.size, we need to reserve a new block
        if(block == cd.size){
            int nextBlock = avail_in_disk();
            if(nextBlock == -1){
                close(client_sock);
                mutex_map[path.second]->unlock();
                return -1;
            }

            cd.blocks[cd.size] = nextBlock;
            cd.size++;
            disk_writeblock(cd.blocks[block], wBuffer);
            disk_writeblock(cd_inode_block, &cd);
        }
        else{
            disk_writeblock(cd.blocks[block], wBuffer);
        }
        //conc unlock when done
        mutex_map[path.second]->unlock();
    }
    //FS_DELETE*
    else if(request == "FS_DELETE") {
        //Error checking: reconstruction
        if(strcmp(recomposed_request.c_str(), buf) != 0) {
            close(client_sock);
            return -1;
        }

        //conc: at this point, node traversed to is locked
        std::pair<fs_inode, int> path = node_traversal(dirPathName, 0, client_sock, username);
        if(path.second == -1){
            return -1;
        }
        fs_inode cd = path.first;
        int cd_inode_block = path.second;
        
        //Error checking: The second to last path cannot be a file
        if(cd.type == 'f'){
            close(client_sock);
            mutex_map[path.second]->unlock();
            return -1;
        }

        //Error checking: The second to last has no such file/dir in it
        if(cd.size == 0){
            close(client_sock);
            mutex_map[path.second]->unlock();
            return -1;
        }
        
        //Continue traversal
        unsigned int numUnused = 0;
        fs_inode finalCd;
        fs_direntry entries[FS_DIRENTRIES];
        int indexOfEntry = 0;
        int entriesIndex = -1;
        for(uint32_t i = 0; i < cd.size; ++i){
            numUnused = 0;
            disk_readblock(cd.blocks[i], &entries);
            for(unsigned int j = 0; j < FS_DIRENTRIES; ++j){
                if(entries[j].inode_block == 0){ // counting number of unused direntries 
                    numUnused++;
                }
                if(std::string(entries[j].name) == entryName && (entries[j].inode_block != 0)){
                    numUnused++;
                    entriesIndex = i;
                    indexOfEntry = j;
                    
                    mutex_map[entries[j].inode_block]->lock();
                    
                    disk_readblock(entries[j].inode_block, &finalCd);
                    
                    //Error checking: is empty directory
                    if(finalCd.type == 'd' && finalCd.size != 0) {
                        close(client_sock);
                        mutex_map[cd_inode_block]->unlock();
                        mutex_map[entries[j].inode_block]->unlock();
                        return -1;
                    }

                    //Error checking: Not owned by user
                    if(std::string(finalCd.owner) != username) {
                        close(client_sock);
                        mutex_map[cd_inode_block]->unlock();
                        mutex_map[entries[j].inode_block]->unlock();
                        return -1;
                    }
                }
            }
            if(entriesIndex != -1){
                break;
            }
        }
        //Error checking: Path of last directory does not exist
        if(entriesIndex == -1){
            close(client_sock);
            mutex_map[cd_inode_block]->unlock();
            mutex_map[path.second]->unlock();
            return -1;
        }

        int finalCdBlock = entries[indexOfEntry].inode_block;
        //if entire block is unused, we free the block and shift over
        if(numUnused == FS_DIRENTRIES){
            index_to_disk.push(cd.blocks[entriesIndex]);
            cd.size--;
            for(uint32_t i = entriesIndex; i < cd.size; ++i){
                cd.blocks[i] = cd.blocks[i + 1];
            }
            disk_writeblock(cd_inode_block, &cd);
        }
        else{
            entries[indexOfEntry].inode_block = 0;
            disk_writeblock(cd.blocks[entriesIndex], &entries);
        }
        mutex_map[cd_inode_block]->unlock();

        //writing over data of file
        if(finalCd.type == 'f'){ 
            for(uint32_t i = 0; i < finalCd.size; ++i){
                index_to_disk.push(finalCd.blocks[i]);
            }
        }
        index_to_disk.push(finalCdBlock);

        //inode mutex deletion
        std::mutex* temp_mutex = mutex_map[finalCdBlock];
        mutex_map[finalCdBlock]->unlock(); //unlock before deletion(double check this)
        mutex_map.erase(finalCdBlock);
        delete temp_mutex;
    }
    else {
        // invalid request header
        close(client_sock);
        return -1;
    }
    //End recieve request

    //Send back request
    int sent = 0;
    do {
        int temp_sent = send(client_sock, buf + sent, count - sent, 0);
        if(temp_sent == -1) {
            close(client_sock);
            return -1;
        }
        sent += temp_sent;
    } while (sent < count);

    if(request == "FS_READBLOCK") {
        int status = send(client_sock, readData, FS_BLOCKSIZE, 0);
        if(status == -1) {
            close(client_sock);
            return -1;
        }
    }
    close(client_sock);
    return 0;
    }
    catch(const std::exception& e)
    {
        close(client_sock);
        return -1;
    }
}

void initialize_tree(int node, std::vector<int> &valid_in_disk) {
    //initializate file system 
    fs_inode inode_temp;
    valid_in_disk[node] = 1;
    // reading i_node
    disk_readblock(node, &inode_temp);
    //conc-initialize root mutex
    std::mutex* new_mutex = new std::mutex();
    mutex_map[node] = new_mutex;

    for(uint32_t i = 0; i < inode_temp.size; ++i){
        //covers case of data or dir_entries - mark as taken in disk
        valid_in_disk[inode_temp.blocks[i]] = 1;
        // if directory, we have more work to do
        if(inode_temp.type == 'd'){
            fs_direntry entries[FS_DIRENTRIES];
            disk_readblock(inode_temp.blocks[i], &entries);
            // flip bit on inodes, call next recursive case
            for(unsigned int j = 0; j < FS_DIRENTRIES; ++j){
                if(entries[j].inode_block != 0){            
                    initialize_tree(entries[j].inode_block, valid_in_disk);
                }
            }
        }
    }
}



int main(int argc, char **argv){
    std::vector<int> valid_in_disk(FS_DISKSIZE, 0);
    initialize_tree(0, valid_in_disk);
    for(unsigned int i = 0 ; i < FS_DISKSIZE; ++i){
        if(valid_in_disk[i] == 0){
            index_to_disk.push(i);
        }
    }

    //Setting Socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    //Use setsockopt with level SOL_SOCKET and optname SO_REUSEADDR
    const int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //End Sock

    //Binding Port Number to Socket
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int addrLen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
        //Assigning port number
    int port_number = 0;
    if(argc == 2) {
        port_number = atoi(argv[1]);
    }
    addr.sin_port = htons(port_number);
    bind(server_sock, (struct sockaddr*) &addr, addrLen);
    //End Bind
    // set port number from case of OS choice
    getsockname(server_sock, (struct sockaddr*) &addr, (socklen_t*)(&addrLen));
    port_number = ntohs(addr.sin_port);

    //Listen
    listen(server_sock, 30);
    //End Listen

    std::cout << "\n@@@ port " << port_number << std::endl;

    while(true) {
        //Accepting connection
        int client_sock = accept(server_sock, 0, 0);
        if(client_sock != -1) {
            std::thread t1(handleConnection, client_sock);
            t1.detach();
            // 🤝🔐 <- hand over hand locking finished
        }
        // piazza post @1424
        else {
            //close(server_sock);
            return 0;
        }
    }
    return 0;
}