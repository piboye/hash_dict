#ifndef FILE_MAP_H_INCLUDE__ 
#define FILE_MAP_H_INCLUDE__ 

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>	

class FileMap
{
public:
        FileMap() {
            buf_	= NULL;
            size_ = 0;
            memset(error_msg_, 0, sizeof(error_msg_));
        }

        ~FileMap() {
            close();
        }
        
#define FileMap_ERROR(fmt, ...) \
    snprintf(this->error_msg_, sizeof(this->error_msg_)-1, "[%d]" fmt,  __LINE__, ##__VA_ARGS__)

        char const * error_msg() const {
            return error_msg_;
        }

        void* addr()	{ return buf_;}
        void const * addr()	const { return buf_;}
        size_t size() const { return size_;}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init
 *  Description:  
 *  Return: <0 failed , 0 is file create, 1 is file exist
 * =====================================================================================
 */
        int init(const char* file_name , off_t size)
        {
            if (file_name == NULL) {
                FileMap_ERROR("file name is empty!");
                return -__LINE__;
            }

            int fd = -1;
            int exists = 1;
            struct stat the_stat;

            fd = open(file_name, O_RDWR);
            if (fd < 0) {
                fd = open( file_name , O_RDWR | O_CREAT, 0666);
                if (fd < 0) {
                    FileMap_ERROR("open failed, errno:%d, errmsg:%s, file:%s\n",\
                        errno,  strerror(errno), file_name);
                    return -__LINE__;
                }
                ::lseek(fd, size-1, SEEK_SET);
                ::write(fd, "", 1);
                exists = 0;
            } else {
                if (stat(file_name, &the_stat) < 0) {
                    FileMap_ERROR("statfailed, errno:%d, errmsg:%s, fle:%s\n",\
                        errno, strerror(errno), file_name);
                    ::close(fd);
                    return -__LINE__;
                }
                if (size == 0)  size =  the_stat.st_size; 
                if (the_stat.st_size != (off_t)size) {
                    FileMap_ERROR("file length(%lld) less then wanted length(%lld), file_name:%s\n", \
                        the_stat.st_size, size, file_name);
                    ::close(fd);
                    return -__LINE__;
                }
            }

            buf_ = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if ( buf_ == NULL ) {
                FileMap_ERROR("mmap failed, errno:%d, errmsg:%s, file: %s\n",\
                    errno, strerror(errno), file_name);
                ::close(fd);
                return -__LINE__;
            }

            ::close(fd);

            file_name_ = file_name;
            size_ = size;
            return exists;
        }

        int close()
        {
            return munmap(buf_, size_);
        }

    
    protected:
        std::string file_name_;
        char error_msg_[256];
        void*			buf_;
        unsigned int	size_;
};
#endif //FILE_MAP_H_INCLUDE__ 
