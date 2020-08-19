#include <sys/types.h>
#include <sys/stat.h>       // 파일 상태
#include <fcntl.h>
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
*/
// int open(const char* name, int flags);
// int open(const char* name, int flags, mode_t mode);

/*
open시 O_WRONLY | O_CREATE | O_TRUNC 조합은 너무나도 일반적이라 아예 이런 동작 방식을 지원하는 시스템 콜인 creat가 존재
*/
// int creat(const char* name, mode_t mode);

#include <unistd.h>
#include <sys/errno.h>
/*
파일을 읽는 매커니즘은 POSIX.1에 정의된 read() 시스템 콜을 사용하는 것
호출할 때마다 fd가 참조하는 파일의 현재 파일 오픗셋에서 len 바이트만큼 buf로 읽어 들임. 성공하면 buf에 쓴 바이트 숫자를 반환, 파일 오프셋은 fd에서 읽은 바이트 크기만큼 전진
읽은 바이트 수는 unsigned long 타입 크기와 동일하며 32bit 리눅스 시스템에서는 4 byte, 64bit 리눅스 시스템에서는 8 byte임
*/
// ssize_t read (int fd, void *buf, size_t len);
/*
read 함수 호출 시 여러 에러 값
EBADF: 주어진 파일 디스크립터가 유효하지 않거나 읽기 가능한 모드로 열리지 않음
EFAULT: buf로 전달된 포인터가 호출하는 프로세스의 주소 공간 밖에 존재
EINVAL: 파일 디스크립터가 읽기를 허용하지 않는 객체에 맵핑
EIO: 저수준 입출력 에러가 발생
*/
/*
read() 크기 제약
POSIX size 크기 타입
- size_t: 바이트 단위로 크기를 측정하기 위해 사용되는 값을 저장, size_t의 최대값=SIZE_MAX
- ssize_t: 부호가 있는(signed) size_t 타입(음수는 에러를 포함하기 위해 사용), ssize_t의 최대값=SSIZE_MAX
*/

#include <unistd.h>
/*
파일을 쓰는 매컨니즘은 POSIX.1에 정의된 write() 시스템 콜을 사용
count 바이트만큼 파일 디스크립터 fd가 참조하는 파일의 현재 파일 위치에 시작 지점이 buf인 내용을 기록
fd가 표현하는 객체에 탐색 기능이 없다면 쓰기 작업은 항상 '처음'위치에서 일어남

성공하면 쓰기에 성공한 바이트 수를 반환, 파일 오프셋도 같은 크기만큼 전진함
*/
// ssize_t write (int fd, const void *buf, size_t count);

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
    return 0;
}