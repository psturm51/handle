#include "../library/handle.h"

int main()
{
    printf("Testing handle class\r\n");
    engine::handle h("/dev/video0", engine::FHT_CAMERA);
    if (h.open() == -1)
    {
        printf("Could not open file\r\n");
        printf("Error: %i\r\n", errno);
        return -1;
    }
    struct v4l2_capability cap;
    if (h.getCameraCapabilities(&cap) == -1)
    {
        printf("Could not get camera capabilities\r\n");
        printf("Error: %i\r\n", errno);
        return -1;
    }
    printf("Driver: %s\r\n", cap.driver);
    printf("Card: %s\r\n", cap.card);
    printf("Bus info: %s\r\n", cap.bus_info);
    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("Device supports capture\r\n");
    }
    if (!(cap.capabilities & V4L2_CAP_READWRITE))
    {
        printf("Device does not support read/write i/o\r\n");
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        printf("Device does not support streaming i/o\r\n");
    }

    h.close();

    return 0;
}