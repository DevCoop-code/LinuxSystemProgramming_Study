#include <sys/types.h>
#include <sys/stat.h>       // 파일 상태
#include <fcntl.h>
// int open(const char* name, int flags);
// int open(const char* name, int flags, mode_t mode);
/*
open() 플래그
Parameter(flags): O_RDONLY, O_WRONLY, O_RDWR
- O_RDONLY: 읽기 전용 
- O_WRONLY: 쓰기 전용
- O_RDWR: 읽기와 쓰기 모드
.....
Parameter(mode): 파일을 생성하지 않는다면 mode인자는 무시, 반대로 mode 인자 없이 O_CREATE로 파일을 생성하면 파일 권한이 정의되지 않아 골치 아픈 일이 생김
(O_CREATE를 사용할 때는 꼭 mode를 확인)
파일이 생성되면 새로 만들어진 파일의 접근 권한은 mode 인자에 따라 설정.

// int creat(const char* name, mode_t mode);
/*
open시 O_WRONLY | O_CREATE | O_TRUNC 조합은 너무나도 일반적이라 아예 이런 동작 방식을 지원하는 시스템 콜인 creat가 존재
*/

#include <unistd.h>
#include <sys/errno.h>
// ssize_t read (int fd, void *buf, size_t len);
/*
파일을 읽는 매커니즘은 POSIX.1에 정의된 read() 시스템 콜을 사용하는 것
호출할 때마다 fd가 참조하는 파일의 현재 파일 오픗셋에서 len 바이트만큼 buf로 읽어 들임. 성공하면 buf에 쓴 바이트 숫자를 반환, 파일 오프셋은 fd에서 읽은 바이트 크기만큼 전진
읽은 바이트 수는 unsigned long 타입 크기와 동일하며 32bit 리눅스 시스템에서는 4 byte, 64bit 리눅스 시스템에서는 8 byte임

read 함수 호출 시 여러 에러 값
EBADF: 주어진 파일 디스크립터가 유효하지 않거나 읽기 가능한 모드로 열리지 않음
EFAULT: buf로 전달된 포인터가 호출하는 프로세스의 주소 공간 밖에 존재
EINVAL: 파일 디스크립터가 읽기를 허용하지 않는 객체에 맵핑
EIO: 저수준 입출력 에러가 발생

read() 크기 제약
POSIX size 크기 타입
- size_t: 바이트 단위로 크기를 측정하기 위해 사용되는 값을 저장, size_t의 최대값=SIZE_MAX
- ssize_t: 부호가 있는(signed) size_t 타입(음수는 에러를 포함하기 위해 사용), ssize_t의 최대값=SSIZE_MAX
*/

#include <unistd.h>
// ssize_t write (int fd, const void *buf, size_t count);
/*
파일을 쓰는 매컨니즘은 POSIX.1에 정의된 write() 시스템 콜을 사용
count 바이트만큼 파일 디스크립터 fd가 참조하는 파일의 현재 파일 위치에 시작 지점이 buf인 내용을 기록
fd가 표현하는 객체에 탐색 기능이 없다면 쓰기 작업은 항상 '처음'위치에서 일어남

성공하면 쓰기에 성공한 바이트 수를 반환, 파일 오프셋도 같은 크기만큼 전진함

write 함수 호출 시 여러 에러 값
EBADF: 파일 디스크립터가 유효하지 않거나 쓰기 모드가 아니다
EFAULT: buf의 포인터가 호출하는 프로세스 주소 공간 안에 있지 않다
EFBIG: 쓰기 작업이 프로세스 단위로 걸려 있는 최대 파일 제약이나 내부 구현 제약보다 더 큰 파일을 만듬
EINVAL: 파일 디스크립터가 쓰기에 적합하지 않은 객체에 맵핑되어 있음
EIO: 저수준의 입출력 에러가 발생
ENOSPC: 파일 디스크립터가 들어 있는 파일시스템에 충분한 공간이 없음
EPIPE: 파일 디스크립터가 파이프와 소켓에 연결되어 있지만, 반대쪽 읽기 단이 닫혀버렸다. 또한 프로세스는 SIGPIPE 시그널을 받는다

write 동작 방식
- write 호출이 반환될 때, 사용자 영역에서 커널에 넘긴 버퍼에서 커널 버퍼로 데이터가 복사된 상태이지만 의도한 목적지에 데이터를 썼다는 보장은 되지 않음, 실제로 쓰기 호출에서 돌아오는 시간은 데이터를 쓰기에는 너무나 촉박함
사용자 영역에서 write() 시스템콜을 호출하면 리눅스 커널은 몇 가지 점검을 수행한 후 단순히 데이터를 버퍼로 복사해놓음. 커널은 모든 변경된 버퍼를 수집해 최적 수준으로 정렬한 다음에
배경 작업으로 디스크에 씀(쓰기저장, writeback)
=> 이러한 방식은 쓰기 호출을 번개처럼 빠르게 수행해 거의 즉시 반환하도록 만듬. 커널은 좀 더 여유가 생길 때까지 쓰기 작업을 늦추고 한꺼번에 여러 작업을 모아 배치함
이러한 지연된 쓰기가 POSIX에서 정의한 의미를 바꾸지는 않음
조금 전에 쓴 데이터를 읽도록 요청하는 순간, 이 데이터가 버퍼에 들어 있으며 아직 디스크에는 기록되지 않은 상태라면 디스크에 저장된 데이터가 아닌 버퍼에 들어 있는 데이터를 바로 가져옴 -> 성능 개선(읽기 작업을 위해 디스크까지 가지 않고 메모리 내부에 존재하는 캐시를 참조하기 때문)

지연된 쓰기와 관련해서는 문제도 존재
1. 데이터를 디스크에 쓰기 전 시스템이 죽으면 디스크에 쓰지 못한채로 데이터가 날아감
2. 쓰기 순서를 강제할 방법이 없음
    - 커널은 성능 개선에 적합한 방씩으로 쓰기 요청 순서를 바꿀것임
3. 입출력 에러 리포트가 힘듬
    - ex] 물리적인 드라이브의 오류로 인해 입출력 에러가 쓰기 저장 시점에 발생하면 쓰기 요청한 프로세스로 리포트가 불가능함

커널은 지연된 쓰기에 따른 위험을 최소화하려고 시도함 제때에 데이터를 기록하도록 보증하기 위해 커널은 최대 버퍼 나이를 만들어 나이가 꽉 찬 변경된 버퍼를 빠짐없이 기록함
*/

// int fsync(int fd);
// int fdatasync(int fd);
/*
애플리케이션에서 직접 데이터가 디스크에 기록되는 시점을 제어하고 싶은 경우 리눅스 커널에서는 성능을 희생하는 대신 입출력을 동기화하는 몇 가지 옵션을 제공
fsync(): 데이터가 디스크에 기록되도록 확인하기 위한 시스템콜
- 파일 디스크립터에 맵핑된 파일의 모든 변경점을 디스크에 기록(fd는 반드시 쓰기 모드로 열려야 함)
- 데이터와 파일 생성 시간 같은 inode에 포함된 메타데이터를 모두 디스크에 기록함
- fsync()는 하드 디스크에 데이터와 메타데이터가 성공적으로 기록을 완료할 때까지 반환하지 않고 기다림

fdatasync(): fsync()와 동일한 기능을 하지만 데이터만 기록함

fdata, fdatasync 모두 변경된 파일이 포함된 디렉터리 엔트리에 대한 디스크 동기화는 보장하지 않음
파일의 링크가 최근에 갱신되었고 파일 데이터도 디스크에 제대로 기록되었지만 관련된 디렉터리 엔트리가 디스크에 기록되지 않았을 경우에는 그 파일에 대한 접근이 불가능
디렉터리 엔트리 역시 디스크에 강제로 기록하려면 디렉터리 자체를 대상으로 연 파일 디스크립터를 fsync()에 인자로 넘겨야 함

fsync, fdatasync 반환값과 에러 코드
호출이 성공하면 0을 반환, 실패하면 두 함수 모두 -1을 반환 errno는 아래 세 값 중 하나를 가짐
EBADF: 주어진 파일 디스크립터가 유효하지 않거나 쓰기 모드가 아님
EINVAL: 주어진 파일 디스크립터가 동기화를 지원하지 않는 객체에 매핑
EIO: 동기화 과정 중에 저수준 입출력 에러 발생
*/

// void sync(void)
/*
호출은 항상 성공하며 버퍼의 모든 내용을 디스크에 강제로 기록
버퍼의 모든 내용을 디스크에 기록한 뒤에 반환하도록 강제하지 않음 그냥 모든 버퍼를 디스크에 기록하는 과정을 시작하도록 요구만 할 뿐
But, 리눅스에서는 버퍼를 모두 기록할 때까지 기다림. 데이터를 디스크에 강제로 기록하기 위해서는 fsync, fdatasync를 사용하는 것을 권장, 작업량이 많은 시스템에서는 sync() 호출이 반환하기까지 수 분이 걸릴수도 있음
*/

// int close (int fd);
/*
close를 호출하면 열려있는 파일 디스크립터 fd에 연관된 파일과의 맵핑을 해제, 프로세스에서 파일을 떼어냄.
But, 파일을 닫더라도 파일을 디스크에 강제로 쓰지는 않음. 파일을 닫기 전에 디스크에 확실히 기록하려면 동기식 입출력(fsync, fdatasync)을 해야 함
close는 파일을 닫는 작업 이외에 연결이 끊어진 파일을 디스크에서 물리적으로 완전히 제거하는 부작용이 발생할 수도 있음
*/

// off_t lseek (int fd, off_t pos, int origin);
/*
파일의 특정 위치로 직접 이동해야할 경우 사용
lseek()은 파일 오프셋 갱신 외에 다른 동작은 하지 않음. 어떤 입출력도 발생시키지 않음
Parameter(origin)
- SEEK_CUR: fd의 파일 오프셋을 현재 오프셋에서 pos를 더한 값으로 설정
- SEEK_END: fd의 파일 오프셋ㅅ을 현재 파일 크기에서 pos를 더한 값으로 설정
- SEEK_SET: fd의 파일 오프셋을 pos 값으로 설정
lseek()는 파일의 시작, 혹은 끝 지점으로 오프셋을 이동하거나, 현재 파일의 오프셋을 알아 내는데 가장 많이 사용

SEEK_END를 이용해 파일 끝을 넘어서도록 위치를 지정하는 것만으로는 아무런 일이 발생하지 않음. But, 쓰기 요청이 들어오면 파일의 마지막 오프셋과 새로운 오프셋
사이에 새로운 공간이 만들어지며 0으로 채워짐
이렇게 0으로 채운 공간을 구멍이라함. 
*/

// ssize_t pread(int fd, void *buf, size_t count, off_t pos);
// ssize_t pwrite(int fd, const void *buf, size_t count, off_t pos);
/*

*/
int main(void) {
    /*
    [Open] the file READ ONLY Mode
    */
    // int fd_1, fd_2;
    // fd_1 = open("/home/kidd/madagascar", O_RDONLY);
    // fd_2 = open("/home/teach/pearl", O_WRONLY | O_TRUNC);
    // if ((fd_1 == -1) || (fd_2 == -1)) {
    //     // Error
    // }
    
    /*
    [Open & Create] the file Write ONLY Mode
    */
   // O_CREATE: name에 적은 파일이 없으면 파일을 새로 만듬. 파일이 이미 있다면 O_EXCL을 붙이지 않는 이상 이 플래그는 아무런 효과가 없음
   // O_TRUNC: 파일이 존재하고, 일반 파일이며 flags 인자에 쓰기가 가능하도록 명시되어 있다면 파일 길이를 0으로 잘라버림
    // int fd;
    // fd = open ("testFile", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    // if (fd == -1) {
    //     printf("Fail to open or create testFile");
    // }

    /*
    [Creat] 호출
    */
//    int fd;
//    fd = creat("createTestFile", 0644);
//    if (fd == -1) {
//        // Error
//    }

     /*
     [Read]
     */
//    unsigned long word;
//    ssize_t nr;

   // 'fd'에서 몇 byte를 읽어 'word'에 저장
//    nr = read (fd, &word, sizeof (unsigned long));
//    if (nr == -1) {
//        // Error
//    }

    // int fd;
    // fd = open("test", O_RDONLY);
    // if (fd == -1) {
    //     printf("Fail to read file");
    // }

    // ssize_t ret;
    // unsigned long len = 8;
    // int buf;

    // while(len != 0 && (ret = read(fd, &buf, len)) != 0) {
    //     if (ret == -1) {
    //         if (errno == EINTR) {       //EINTR: Error Interrupt
    //             continue;
    //         }
    //         perror("read");
    //         break;
    //     }

    //     printf("ret: %d\n", ret);
    //     len -= ret;
    //     // buf += ret;
    //     printf("len size: %d\n", len);
    //     printf("buf: %d\n", buf);
    // }

    // int fd;
    // fd = open("../test", O_RDONLY);
    // if (fd == -1) {
    //     perror("open");
    // }

    // char word[8] = {0,};
    // ssize_t ret;
    // int len = 8;

    // while(len != 0 && (ret = read(fd, &word, len)) != 0) {
    //     if (ret == -1) {
    //         if (errno == EINTR) {
    //             continue;
    //         }
    //         perror("read");
    //         break;
    //     }

    //     for(int i = 0; i < 8; i++) {
    //         printf("%c\n", word[i]);
    //     }
    // }

    /*
    [Non-Block Read]
    논블록 읽기
    블록되는 대신 읽을 데이터가 없다는 사실을 알려주기 위해 호출이 되는 방식을 논블록 입출력이라 함
    논블록 입출력은 애플리케이션이 잠재적으로 다중 파일 입출력을 수행하도록 만듬 따라서 특정 파일에서 블록되는 바람에 다른 파일에서 사용 가능한 데이터를
    놓치는 현상이 벌어지지 않음

    구현방법
    - 추가적인 errno값인 EAGAIN을 점검
    Open을 호출하면서 O_NONBLOCK을 넘겨 파일 디스크립터를 논블록 모드로 열었지만 읽을 데이터가 없다면 read()호출은 블록되는 대신 -1을 반환하며 errno를 EAGAIN으로 설정
    논블록 읽기는 EAGAIN을 반드시 점검해야 함
    */
//    int BUFSIZE = 10;
//    char buf[BUFSIZE];
//    ssize_t nr;

//    start:
//    nr = read(fd, buf, BUFSIZE);
//    if (nr == -1) {
//        if (errno == EINTR) {
//            goto start;
//        }
//        if (errno == EAGAIN) {
//            // 나중에 다시 씨도
//        }
//        else {
//            // Error
//        }
//    }

    /*
    [Write]
    */
   int fd;
   const char* buf = "My ship is solid!";
   ssize_t nr;

    fd = creat("test", 0644);
    if ((fd == -1)) {
        perror("open");
    }

    size_t count = strlen(buf);
    /* 'buf'에 들어 있는 문자열을 'fd'에 기록 */
    nr = write(fd, buf, count);
    if (nr == -1) {
        perror("write");
    }
    else if(nr != count) {
        // Error 일 가능성이 있지만, errno는 설정되지 않음
    }

    /*
    [Sync]
    */
    int ret;
    ret = fsync(fd);
    if (ret == -1) {
        perror("fsync");
    }

    int pos = 0;
    pos = lseek(fd, 0, SEEK_END);
    if (pos == (off_t)-1) {
        perror("lseek");
    } else {
        printf("file total offset: %d\n", pos);
    }

    int len = 0;
    len = pos;
    char word[len];
    memset(word, 0, len);
    ssize_t readResult;

    int read_fd;
    read_fd = open("test", O_RDONLY);
    if (read_fd == -1) {
        perror("read");
    }

    while(len != 0 && (readResult = read(read_fd, &word, len)) != 0) {
        if (readResult == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
        }
    }
    
    for (int i = 0; i < len; i++) {
        printf("%c \n", word[i]);
    }

    ret = lseek(fd, (off_t)10, SEEK_END);
    if (ret == (off_t)-1) {
        // Error
    }

    nr = write(fd, buf, count);
    if (nr == -1) {
        perror("write");
    }

    ret = fsync(fd);
    if (ret == -1) {
        perror("fsync");
    }

    /*
    [Close]
    */
    int result;
    result = close(fd);
    if (result == -1) {
        perror("close");
    }

    result = close(read_fd);
    if (result == -1) {
        perror("close");
    }

    return 0;
}