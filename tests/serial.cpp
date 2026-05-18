#include "../library/handle.h"

int main()
{
    printf("Testing handle class\r\n");
    engine::handle h("/dev/ttyUSB0", engine::FHT_SERIAL);
    if (h.open(FLAG_SERIAL_LOW_SPEED) == -1)
    {
        printf("Could not open file\r\n");
        return -1;
    }
    char buffer[h.available() + 1];
    memset(buffer, 0, h.available() + 1);
    int read = h.read(buffer, h.available());

    printf("Read %i bytes\r\n", read);
    printf("File contents: %s\r\n", buffer);

    return false;
}