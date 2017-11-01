#include "learn_node.h"

#define SSDP_PORT 1900
#define SSDP_ADDR "239.255.255.250"
#define SEARCH_INTERVAL 2000
#define SEARCH_TIMEOUT 1000
#define SSDP_RESP_STR "NOTIFY * HTTP/1.1\r\n\
HOST: 239.255.255.250:1900\r\n\
NT: homebase:device\r\n\
NTS: ssdp:alive\r\n\
USN: uuid:f40c2981-7329-40b7-8b04-27f187aecfb8\r\n\
LOCATION: http://10.0.0.107:10293\r\n\
CACHE-CONTROL: max-age=1800\r\n\
DEVICE_TYPE: bridge\r\n"
#define SSDP_SEARCH_STR "M-SEARCH * HTTP/1.1\r\n\
HOST: 239.255.255.250:1900\r\n\
ST: homebase:device\r\n\
MAN: \"ssdp:discover\"\r\n\
MX: 3\r\n\
\r\n"

typedef struct r_udp_send_t {
  uv_udp_send_t req;
  uv_buf_t buf;
} r_udp_send_t;

void free_req(r_udp_send_t *req) {
    free(req->buf.base);
    free(req);
}

void alloc_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init(RMALLOC_ARR(char, suggested_size), (uint)suggested_size);
}

void on_server_sent_heartbeat(uv_udp_send_t* req, int status) {
    if (status < 0) {
        RERROR_CODE(status);
    }
    free_req((r_udp_send_t*)req);
}

void server_send_heartbeat(uv_udp_t *handle, const struct sockaddr* addr) {
    static const size_t resp_len = sizeof SSDP_SEARCH_STR;
    r_udp_send_t *r_req = RMALLOC(r_udp_send_t);
    uv_udp_send_t *req = &r_req->req;
    uv_buf_t *buf = &r_req->buf;
    alloc_buf(NULL, resp_len, buf);
    memcpy(buf->base, SSDP_RESP_STR, buf->len);
    uv_udp_send(req, handle, buf, 1, addr, on_server_sent_heartbeat);
}

void on_server_recv_multicast(uv_udp_t* handle,
                              ssize_t nread,
                              const uv_buf_t* buf,
                              const struct sockaddr* addr,
                              unsigned flags) {
    if (nread < 0) {
        RERROR_CODE(nread);
        uv_close((uv_handle_t*)handle, NULL);
        free(buf->base);
        return;
    }
    if (addr) {
        struct sockaddr_in *sin = (struct sockaddr_in*)addr;
        char *ip = inet_ntoa(sin->sin_addr);
        RLOG("[server] multicast from %s:%d\n%s", ip, sin->sin_port, buf->base);
        server_send_heartbeat(handle, addr);
    } else {
        free(buf->base);
    }
}

void on_rokid_recv_heartbeat(uv_udp_t* handle,
                             ssize_t nread,
                             const uv_buf_t* buf,
                             const struct sockaddr* addr,
                             unsigned flags) {
    if (nread < 0) {
        RERROR_CODE(nread);
        uv_close((uv_handle_t*)handle, NULL);
        free(buf->base);
        return;
    }
    if (addr) {
        struct sockaddr_in *sin = (struct sockaddr_in*)addr;
        char *ip = inet_ntoa(sin->sin_addr);
        RLOG("[rokid] heartbeat from %s:%d\n%s", ip, sin->sin_port, buf->base);
    } else {
        free(buf->base);
    }
}

void on_rokid_sent_multicast(uv_udp_send_t* req, int status) {
    if (status < 0) {
        RERROR_CODE(status);
    }
    free_req((r_udp_send_t*)req);
}

void rokid_send_multicast(uv_timer_t *handle) {
    static const size_t search_len = sizeof SSDP_SEARCH_STR;
    uv_udp_t *rokid_socket = (uv_udp_t*)handle->data;
    r_udp_send_t *r_req = RMALLOC(r_udp_send_t);
    uv_udp_send_t *req = &r_req->req;
    uv_buf_t *buf = &r_req->buf;
    alloc_buf(NULL, search_len, buf);
    memcpy(buf->base, SSDP_SEARCH_STR, buf->len);
    struct sockaddr addr;
    uv_ip4_addr(SSDP_ADDR, SSDP_PORT, (struct sockaddr_in*)&addr);
    uv_udp_send(req, rokid_socket, buf, 1, &addr, on_rokid_sent_multicast);
}

void rokid_search() {
    uv_udp_t *rokid_socket = RMALLOC(uv_udp_t);
    uv_udp_init(uv_default_loop(), rokid_socket);
    struct sockaddr_in rokid_addr;
    uv_ip4_addr("0.0.0.0", 0, &rokid_addr);
    uv_udp_bind(rokid_socket, (struct sockaddr*)&rokid_addr, 0);
    uv_udp_recv_start(rokid_socket, alloc_buf, on_rokid_recv_heartbeat);
    uv_udp_set_broadcast(rokid_socket, 1);

    uv_timer_t *timer_req = RMALLOC(uv_timer_t);
    timer_req->data = rokid_socket;
    uv_timer_init(uv_default_loop(), timer_req);
    uv_timer_start(timer_req, rokid_send_multicast, SEARCH_TIMEOUT, SEARCH_INTERVAL);
}

void server_listen() {
    uv_udp_t *server_socket = RMALLOC(uv_udp_t);
    uv_udp_init(uv_default_loop(), server_socket);
    struct sockaddr_in server_addr;
    uv_ip4_addr("0.0.0.0", SSDP_PORT, &server_addr);
    uv_udp_bind(server_socket, (struct sockaddr*)&server_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(server_socket, alloc_buf, on_server_recv_multicast);
    uv_udp_set_membership(server_socket, SSDP_ADDR, NULL, UV_JOIN_GROUP);
}

int main(int argc, char **argv) {
//  server_listen();
  rokid_search();
  return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
