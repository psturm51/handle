#include "../library/handle.h"

int main(){
    printf("Testing handle class\r\n");
    engine::handle h("./data/test.txt",engine::FHT_FILE);
    if(h.open() == -1){
        printf("Could not open file\r\n");
        return -1;
    }
    char buffer[h.size()+1];
    memset(buffer,0,h.size()+1);
    int read = h.read(buffer,h.size());

    printf("Read %i bytes\r\n",read);
    printf("File contents: %s\r\n",buffer);

    return false;
}