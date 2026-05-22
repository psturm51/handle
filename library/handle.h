#ifndef HANDLE_H
#define HANDLE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/spi/spidev.h>
#include <linux/input.h>
#include <linux/videodev2.h>
#include <sys/inotify.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define FLAG_SERIAL_LOW_SPEED      0x01      //! 9600 bps
#define FLAG_SERIAL_MED_SPEED      0x02      //! 115200 bps
#define FLAG_SERIAL_HI_SPEED       0x04      //! 1000000 bps
#define FLAG_SERIAL_ULTRA_SPEED    0x08      //! 2000000 bps

//! Some custom flag definitions
#define FLAG_RW       O_RDWR
#define FLAG_RO       O_RDONLY

#define FLAG_SOCKET_CLIENT      0x20000000
#define FLAG_SOCKET_SERVER      0x10000000

#define FLAG_SOCKET_STREAM      0x8000000
#define FLAG_SOCKET_UDP         0x4000000

//#define FLAG_INOTIFY_MANAGER    0x8
//#define FLAG_INOTIFY_WATCHER    0x4

//#define INOTE_EV_SIZE (sizeof(struct inotify_event));
//#define INOTE_BUFF_LEN  (1024*INOTE_EV_SIZE)
/**
Valid Inotify events:
  IN_ACCESS
  IN_ATTRIB
  IN_CLOSE_WRITE
  IN_CLOSE_NOWRITE
  IN_CREATE
  IN_DELETE
  IN_DELETE_SELF
  IN_MODIFY
  IN_MOVE_SELF
  IN_MOVED_FROM
  IN_MOVED_TO
  IN_OPEN


  IN_ALL_EVENTS (All events)
  IN_MOVE (IN_MOVED_FROM | IN_MOVED_TO)
  IN_CLOSE (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)

Valid Inotify flags:
  IN_DONT_FOLLOW
  IN_EXCL_UNLINK
  IN_MASK_ADD
  IN_ONESHOT
  IN_ONLYDIR
  IN_MASK_CREATE

Valid Inotify event flags:
  IN_IGNORED
  IN_ISDIR
  IN_Q_OVERFLOW
  IN_UNMOUNT
*/

namespace engine {
  typedef unsigned char u8;
  typedef unsigned short u16;
  typedef unsigned int u32;
  typedef unsigned long long u64;

  typedef signed char s8;
  typedef signed short s16;
  typedef signed int s32;
  typedef signed long long s64;

  typedef float f32;
  typedef double f64;
  typedef long double f128;

  enum fileHandleType {
    FHT_NULL=0,
    FHT_FILE,
    FHT_PIPE,
    FHT_EXEC,
    FHT_DIR,
    FHT_SOCKET,
    FHT_SERIAL,
    FHT_SPI,
    FHT_I2C,
    FHT_BT,
    FHT_KEYBOARD,
    FHT_INOTIFY,
    FHT_MOUSE,
    FHT_TIMER,
    FHT_CAMERA,
    FHT_MICROPHONE,
    FHT_SPEAKER
  };

  

  class handle {
  private:
    virtual bool assertKeyboard();
    virtual s32 xioctl(s32 request, void *arg);
    virtual s32 set_interface_attribs(s32 speed,s32 parity);
    //! Open a serial descriptor
    virtual s32 openSerial(s32 baud);

  public:
    handle():
    filepointer(0){
      this->descriptor = -1;
      this->type = FHT_NULL;
    }
    handle(const std::string& px,fileHandleType t=FHT_FILE):
    path(px),
    type(t),
    filepointer(0)
    {}
    handle(int desc,fileHandleType t=FHT_FILE):
    path(),
    type(t),
    filepointer(0)
    {
      this->descriptor = desc;
    }
    virtual ~handle(){
      this->type = FHT_NULL;
      this->descriptor = 0;
      this->filepointer = 0;
    }

    virtual handle& operator=(const handle& other);

    //! Open the descriptor, possibly using some flags
    virtual s32 open(u32 flags = 0);
    
    //! Generic descriptor close. Works with just about everything
    virtual void close();

    //! Read data from the descriptor
    virtual s32 read(void *buffer,s32 size);
    //! Write data to the descriptor
    virtual s32 write(const void *buffer,s32 size);
    //! Start the file iterator from the beginning
    virtual void rewind();
    //! Check the total size of the descriptor
    virtual s64 size();
    //! Check the current position of the descriptor
    virtual s64 position(s64 pos = 0);
    //! Check how many bytes are available on the handle for reading
    virtual s64 available();
    //! For use in regular files, allocate an amount of blank disk.
    virtual s32 allocate(s64 s);
    //! Use different methods to change blocking mechanics for the descriptor
    virtual s32 setBlocking(bool b);
    /** Context dependant:
     * For a timer descriptor, it will set the nessecary timer specifics,
     * otherwise, it will return -1; */
    virtual s32 setTime(s64 alarm,s64 frequency,u32 flags=0);
   
    /** Context dependant:
      FHT_INOTIFY: notify any changes on this handle
    */
    virtual handle* add(handle *h,u32 flags);
    /** Context dependant:
      FHT_INOTIFY: cease notify any changes on this handle
    */
    virtual void remove(handle *h);
    /**
     * Accept another socket from this socket. It will only work for
     * listening sockets.
    */
    virtual handle* accept();
    /** Transfer some bytes using i2c or spi bus only. For low-level
     * hardware io
    */
    virtual s32 transfer(u8 command,u8 address,u8 offset,u8 *buffer,u16 length,u32 flags);
    //! For i2c only, sets slave address
    virtual s32 setAddress(s16 address);

    //! IOCTL
    virtual s32 ioctl(s32 request,void *arg);
    virtual s32 ioctl(u32 request,void *arg);

    //! THE FOLLOWING FUNCTIONS ARE CAMERA SPECIFIC
    virtual s32 getCameraCapabilities(struct v4l2_capability *cap);
    //! END CAMERA FUNCTIONS

    std::string path;

    fileHandleType type;
	
    s32 descriptor;
	
    FILE *filepointer;
  };
  inline handle& handle::operator=(const handle& other){
    this->path = other.path;
    this->type = other.type;
    this->descriptor = other.descriptor;
    this->filepointer = other.filepointer;
    return *this;
  }
  inline s32 handle::set_interface_attribs( s32 speed, s32 parity)
  {
    struct termios tty;
    if (tcgetattr(this->descriptor, &tty) != 0)
    {
      return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0;        // no signaling chars, no echo,
                            // no canonical processing
    tty.c_oflag = 0;        // no remapping, no delays
    tty.c_cc[VMIN] = 0;     // read doesn't block
    tty.c_cc[VTIME] = 5;    // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(this->descriptor, TCSANOW, &tty) != 0)
    {
      return -1;
    }
    return 0;
  }
  inline s32 handle::xioctl(s32 request, void *arg)
  {
    for (s32 i = 0; i < 3; i++)
    {
      s32 r = ::ioctl(this->descriptor, request, arg);
      if (r != -1 || errno != EINTR)
        return r;
    }
    return -1;
  }
  inline s32 handle::transfer(u8 command,u8 address,u8 offset,u8 *buffer,u16 length,u32 flags){
    s32 retval = 0;
      switch(this->type){
        case FHT_I2C: {
          //! Do an i2c transfer
          uint8_t command[] = { 0xE5, offset };
          struct i2c_msg messages[] = {
            { address, 0, sizeof(command), command },
            { address, I2C_M_RD, length, buffer }
          };
          struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };
          s32 result = ::ioctl(this->descriptor, I2C_RDWR, &ioctl_data);
          if(result != 2){
            retval = -1;
          }
          //! Nothing more to do here.
        } break;
        case FHT_SPI: {
          //! Do an spi transfer
        } break;
      }
      return retval;
  }
  inline bool handle::assertKeyboard(){
    bool isKeyboard = false;
    s32 event_bitmap = 0;
    s32 kbd_bitmap = KEY_A | KEY_B | KEY_C | KEY_Z;

    ::ioctl(this->descriptor, EVIOCGBIT(0, sizeof(event_bitmap)), &event_bitmap);
    if((EV_KEY & event_bitmap) == EV_KEY){
        // The device acts like a keyboard

        ::ioctl(this->descriptor, EVIOCGBIT(EV_KEY, sizeof(event_bitmap)), &event_bitmap);
        if((kbd_bitmap & event_bitmap) == kbd_bitmap){
            // The device supports A, B, C, Z keys, so it probably is a keyboard
            isKeyboard = true;
        }

    }

    return isKeyboard;
  }
  inline s32 handle::setBlocking(bool b){
    //! Set blocking / non-blocking mode
    s32 retval = 0;
    //! Convert to int
    s32 should_block = (b==true)?1:0;
    switch(this->type){
      case FHT_SERIAL: {
        //! This is a serial port. Set serial port specifics
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (this->descriptor, &tty) != 0)
        {
          retval = -1;
          break;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (this->descriptor, TCSANOW, &tty) != 0){
          retval = -1;
        }
      } break;
      case FHT_FILE:
      case FHT_INOTIFY:
      case FHT_SOCKET: {
        s32 flags = fcntl(this->descriptor, F_GETFL, 0);
        if (flags == -1) return false;
        flags = should_block ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        retval =  (fcntl(this->descriptor, F_SETFL, flags) == 0) ? 0 : -1;
      } break;
    }

    return retval;
  }
  inline handle* handle::add(handle *h,u32 flags){
    switch(this->type){
      case FHT_INOTIFY: {
        if(h->type != FHT_INOTIFY) h->type = FHT_INOTIFY;
        //! Add the handle to the watch list
        h->descriptor = inotify_add_watch(this->descriptor,h->path.c_str(), flags );
      } break;
    }
    return h;
  }
  inline void handle::remove(handle *h){
    switch(this->type){
      case FHT_INOTIFY: {
        //! Remove the handle from the watch list
        inotify_rm_watch(this->descriptor,h->descriptor);
      } break;
    }
  }
  inline s32 handle::openSerial(s32 baud){
    //! The resource type is a serial port
    this->descriptor = ::open(this->path.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (this->descriptor <0){
      printf("Could not open serial port\r\n");
      return -1;
    }
    if(baud == 9600)
      set_interface_attribs (B9600, 0);  // set speed to 9,600 bps, 8n1 (no parity)
    else if(baud == 115200)
      set_interface_attribs (B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    else if(baud == 1000000)
      set_interface_attribs (B1000000, 0);  // set speed to 1,000,000 bps, 8n1 (no parity)
    else if(baud == 2000000)
      set_interface_attribs (B2000000, 0);  // set speed to 2,000,000 bps, 8n1 (no parity)
    //}
    this->setBlocking(true);
    return 0;
  }
  inline s32 handle::setTime(s64 alarm,s64 frequency,u32 flags){

    if(this->type != FHT_TIMER) return -1;

    struct itimerspec nspec;
    if(alarm>=0){
      //! Do some basic maths
      s64 a_sec = alarm / 1000000000;
      s64 a_nsec = alarm % 1000000000;
    

      //! Set the alarm structure
      nspec.it_value.tv_sec = a_sec;
      nspec.it_value.tv_nsec = a_nsec;
    }

    if(frequency >=0){
      //! Do some basic maths
      s64 f_sec = frequency / 1000000000;
      s64 f_nsec = frequency % 1000000000;

      //! Set the interval structure
      nspec.it_interval.tv_sec = f_sec;
      nspec.it_interval.tv_nsec = f_nsec;
    }

    s32 retval = timerfd_settime(this->descriptor,0,&nspec,NULL);
    if(retval < 0){
      printf("Could not set timer\r\n");
    }
    return retval;
  }
  inline s32 handle::open(u32 flags){
    s32 retval = 0;
    switch(this->type){
      case FHT_FILE: {
        //! The resource type is a standard file
        this->descriptor = ::open(this->path.c_str(),flags,O_LARGEFILE);
        if(this->descriptor == -1){
          retval = -1;
        }
      } break;
      case FHT_EXEC: {
        this->filepointer = popen(this->path.c_str(),"r");
        if(!this->filepointer){
          retval = -1;
        }

        this->descriptor = fileno(this->filepointer);
      } break;
      case FHT_CAMERA: {
        //! Open the descriptor as though it is a camera

        //! Open the descriptor in read-only mode
        this->descriptor = ::open(this->path.c_str(),O_RDWR | O_NONBLOCK);

        //! Check if descriptor opened
        if(this->descriptor == -1){
          retval = -1;
        }

        // 2. Query capabilities
        struct v4l2_capability cap;
        if (::ioctl(this->descriptor, VIDIOC_QUERYCAP, &cap) < 0)
        {
          ::close(this->descriptor);
          retval = -1;
          break;
        }

        struct v4l2_format fmt;
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = 640;
        fmt.fmt.pix.height = 480;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;

        if (::ioctl(this->descriptor, VIDIOC_S_FMT, &fmt) < 0)
        {
          ::close(this->descriptor);
          retval = -1;
        }
      } break;
      case FHT_SERIAL: {
        //! The resource type is a serial port
        int speed = 0;

        if(flags & FLAG_SERIAL_LOW_SPEED)
          speed = 9600;
        else if (flags & FLAG_SERIAL_MED_SPEED)
          speed = 115200;
        else if (flags & FLAG_SERIAL_HI_SPEED)
          speed = 1000000;
        else if (flags & FLAG_SERIAL_ULTRA_SPEED)
          speed = 2000000;
        else {
          printf("Invalid flags set\r\n");
          retval = -1;
          break;
        }
        retval = this->openSerial(speed);

      } break;
      case FHT_KEYBOARD: {
        this->descriptor = ::open(this->path.c_str(),O_RDONLY);
        if(this->descriptor == -1){
          retval = -1;
        }
        if(this->assertKeyboard() == false){
          //! This is not a keyboard file
          this->close();
          retval = -1;
        }
      } break;
      case FHT_INOTIFY: {
        //! Open an instance to the inotify manager
        this->descriptor = inotify_init();
        if(this->descriptor<0){
          printf("Could not create inotify instance\r\n");
          retval = -1;
        }
      } break;
      case FHT_TIMER: {
        this->descriptor = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
        if(this->descriptor<0){
          printf("Could not create timer instance: %i\r\n",errno);
          retval = -1;
        }
      } break;
      case FHT_SOCKET: {

        u32 lf = 0;

        if(flags&FLAG_SOCKET_STREAM)
          lf = SOCK_STREAM;
        else if (flags&FLAG_SOCKET_UDP)
          lf = SOCK_DGRAM;
        this->descriptor = ::socket(AF_INET,lf,0);
        if(this->descriptor == -1){
          printf("Could not create socket\r\n");
          retval = -1;
          break;
        }

        int abc = 1;

        if (setsockopt(this->descriptor, SOL_SOCKET, SO_REUSEADDR, &abc, sizeof(int)) < 0){
          printf("Could not enable address reuse\r\n");
        }

        printf("Printing ip address info: %s\r\n",this->path.c_str());

        //! Now to turn the pathname into an ip address
        char *it = (char*)this->path.c_str();
        char *del = it;
        char *sec = 0;
        char *end = it;

        char *pts[] = {0,0,0};
        int cnt=0;

        for(end;*end!= 0x00;++end){
          if(*end == ':'){
            del = end;
          }
          if(*end == '.'){
            pts[cnt] = end;
            ++cnt;
          }
        }
        sec = del +1;
        del[0] = 0;
        //! Grab the port number from the string
        int portNo = atoi(sec);

        struct sockaddr_in remote;
        memset(&remote,0,sizeof(remote));

        if(flags & FLAG_SOCKET_CLIENT){
          //! Create a new socket in client mode
          remote.sin_family = AF_INET;
          remote.sin_port = htons(portNo);

          if(inet_pton(AF_INET,it, &remote.sin_addr)<=0)
          {
            printf("Could not translate network address: %s\r\n",it);
            ::close(this->descriptor);
            return -1;
          }
          //! The almighty connect function
          if( ::connect(this->descriptor, (struct sockaddr *)&remote, sizeof(remote)) < 0){
            printf("Could not connect to remote host\r\n");
            ::close(this->descriptor);
            return -1;
          }
        } else if (flags & FLAG_SOCKET_SERVER){
          //! Create a new socket in server mode
          remote.sin_family = AF_INET;
          remote.sin_addr.s_addr = htonl(INADDR_ANY);
          remote.sin_port = htons(portNo);

          if(bind(this->descriptor, (struct sockaddr*)&remote, sizeof(remote))<0){
            printf("Could not bind to socket\r\n");
            ::close(this->descriptor);
            return -1;
          }
          //! We have bound to the port
        }
        del[0]=':';

      } break;
      case FHT_I2C: {
        //! Open i2c device. Happy days
        //! The resource type is an i2c channel
        this->descriptor = ::open(this->path.c_str(),O_RDWR);
        if(this->descriptor == -1){
          retval = -1;
        }
      } break;
      default: {
        this->descriptor = ::open(this->path.c_str(), O_RDWR);
        if (this->descriptor == -1)
        {
          retval = -1;
        }
      } break;
    }

    return retval;
  }
  inline void handle::close(){
    switch(this->type){
      case FHT_EXEC: {
        pclose(this->filepointer);
        this->filepointer = 0;
      } break;
      case FHT_SOCKET: {
        ::shutdown(this->descriptor,SHUT_RDWR);
      } break;
      default:{
        ::close(this->descriptor);
      } break;
    }
  }
  inline s32 handle::read(void *buffer,s32 size){
    s32 retval = 0;

    switch(this->type){
      case FHT_SOCKET: {
          retval = recv(this->descriptor,buffer,size,0);
      } break;
      default: {
          retval = ::read(this->descriptor,buffer,size);
      } break;
    }

    return retval;
  }
  inline s32 handle::write(const void *buffer,s32 size){
    if(this->type == FHT_SOCKET)
      return send(this->descriptor,buffer,size,0);
    return ::write(this->descriptor,buffer,size);
  }
  inline void handle::rewind(){
    lseek64(this->descriptor,0,SEEK_SET);
  }
  inline s64 handle::size(){
    struct stat s;
    if (fstat(this->descriptor, &s) == -1) {
      return -1;
    }
    return(s.st_size);
  }
  inline s64 handle::position(s64 pos){
    return lseek64(this->descriptor,pos,SEEK_CUR);
  }
  inline s64 handle::available(){
    s64 o = 0;
    switch(this->type){
      case FHT_SERIAL:
      case FHT_EXEC:
      case FHT_SOCKET: {
        int out = 0;
        ::ioctl(this->descriptor,FIONREAD,&out);
        o = out;
      } break;
      case FHT_FILE: {
        o= this->size()-this->position();
      } break;
    };
    return o;
  }
  inline handle* handle::accept(){
    //! Check if this type of file supports this operation
    if(this->type != FHT_SOCKET) return 0;
    struct sockaddr_in remote;
    memset(&remote,0,sizeof(remote));

    socklen_t rsize = sizeof(remote);

    handle *nh = new handle(::accept(this->descriptor,(struct sockaddr*)&remote, &rsize),FHT_SOCKET);

    return nh;
  }
  //! Only for use on regular files. This allows us to preallocate large
  //! segments of disk for our applications. Pretty handy.
  inline s32 handle::allocate(s64 s){
    //! Check if this type of file supports this operation
    if(this->type != FHT_FILE) return -1;

    int bsize = 4096*8;
    //! Continue with the operation
    s64 blocks = s/(bsize);
    s64 remains = s%(bsize);

    u8 buff[(bsize)] = {0,};

    for(int i=0;i<blocks;++i){
      this->write(buff,(bsize));
    }
    this->write(buff,remains);
    return 0;
  }
  inline s32 handle::setAddress(s16 address){
    if(this->type != FHT_I2C) return -1;
    //! Set the specific address for the i2c slave
    return ::ioctl(this->descriptor, I2C_SLAVE,address);
  }
  inline s32 handle::ioctl(s32 request,void *arg){
    return ::ioctl(this->descriptor,request,arg);
  }
  inline s32 handle::ioctl(u32 request,void *arg){
    return ::ioctl(this->descriptor,request,arg);
  }
  //! CAMERA SPECIFIC FUNCTIONS
  inline s32 handle::getCameraCapabilities(struct v4l2_capability *cap){
    if(!cap) return -1;
    if(this->type != FHT_CAMERA) return -1;
    return ::ioctl(this->descriptor, VIDIOC_QUERYCAP, cap);
  }
}

#endif
