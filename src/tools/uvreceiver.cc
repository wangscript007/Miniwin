#include<uv.h>
#include<sys/time.h>

struct timeval ts;
static uv_udp_t recv_socket;
static char slab[188*20];
static long long recv_bytes=0;
static long count=0;
static void alloc_cb(uv_handle_t* handle, size_t suggested_size,
     uv_buf_t* buf){
     buf->base = slab;
     buf->len = sizeof(slab);
}

static void recv_cb(uv_udp_t* handle,ssize_t nread,const uv_buf_t* rcvbuf,
      const struct sockaddr* addr,unsigned flags){
    unsigned char*dat=(unsigned char*)rcvbuf->base;
    recv_bytes+=nread;
    struct timeval te;
    count+=(nread!=0);
    if(count%200==0&&nread){
       gettimeofday(&te,NULL);
       long dur=te.tv_sec*1000+te.tv_usec/1000-ts.tv_sec*1000-ts.tv_usec/1000;
       printf("recv %d /%lld bytes data:%02x %02x %02x %02x  %ld/s\r\n",nread,recv_bytes,
           dat[0],dat[1],dat[2],dat[3],1000L*count/dur );
    }
}
int main(int argc,char*argv[]){
    uv_loop_t*loop = uv_default_loop();
    uv_udp_init(loop, &recv_socket);
    struct sockaddr_in recv_addr;
    uv_ip4_addr("0.0.0.0",0x8315, &recv_addr);
    uv_udp_bind(&recv_socket, (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&recv_socket, alloc_cb, recv_cb);
    gettimeofday(&ts,NULL);
    uv_run(loop, UV_RUN_DEFAULT);
    printf("recved %lld bytes\r\n",recv_bytes);
}
