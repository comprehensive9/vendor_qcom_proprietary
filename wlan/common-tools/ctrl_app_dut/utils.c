/* Copyright (c) 2020 Wi-Fi Alliance                                                */

/* Permission to use, copy, modify, and/or distribute this software for any         */
/* purpose with or without fee is hereby granted, provided that the above           */
/* copyright notice and this permission notice appear in all copies.                */

/* THE SOFTWARE IS PROVIDED 'AS IS' AND THE AUTHOR DISCLAIMS ALL                    */
/* WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                    */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL                     */
/* THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR                       */
/* CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING                        */
/* FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF                       */
/* CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT                       */
/* OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS                          */
/* SOFTWARE. */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#ifdef _OPENWRT_
#include <sys/time.h>
#endif
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#ifdef _OPENWRT_QTI_
#include <net/if.h>
#else
#include <linux/if.h>
#endif
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdint.h>
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#include "vendor_specific.h"
#include "utils.h"
#include "eloop.h"

/* Log */
int stdout_level = LOG_LEVEL_DEBUG;
int syslog_level = LOG_LEVEL_INFO;

/* multiple VAPs */
int interface_count = 0;
int configured_interface_count = 0;
struct interface_info interfaces[8];
int band_mbssid_cnt[16];
struct interface_info* default_interface;
static struct loopback_info loopback = {};
int ap_interface_5gh = 0;

#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
int use_openwrt_wpad = 0;
#endif

void send_continuous_loopback_packet(void *eloop_ctx, void *sock_ctx);

void debug_print_timestamp(void) {
    time_t rawtime;
    struct tm *info;
    char buffer[32];

    time(&rawtime);
    info = localtime(&rawtime);
    if (info) {
        strftime(buffer, sizeof(buffer), "%b %d %H:%M:%S", info);
        printf("%s ", buffer);
    }
}

void indigo_logger(int level, const char *fmt, ...) {
    char *format, *log_type;
    int maxlen;
    int priority;
    va_list ap;

    maxlen = strlen(fmt) + 100;
    format = malloc(maxlen);
    if (!format) {
        return;
    }

    switch (level) {
    case LOG_LEVEL_DEBUG_VERBOSE:
        log_type = "debugverbose";
        break;
    case LOG_LEVEL_DEBUG:
        log_type = "debug";
        break;
    case LOG_LEVEL_INFO:
        log_type = "info";
        break;
    case LOG_LEVEL_NOTICE:
        log_type = "notice";
        break;
    case LOG_LEVEL_WARNING:
        log_type = "warning";
        break;
    default:
        log_type = "info";
        break;
    }

    snprintf(format, maxlen, "controlappc.%8s  %s", log_type, fmt);

    if (level >= stdout_level) {
        debug_print_timestamp();
        va_start(ap, fmt);
        vprintf(format, ap);
        va_end(ap);
        printf("\n");
    }

    if (level >= stdout_level) {
        switch (level) {
        case LOG_LEVEL_DEBUG_VERBOSE:
        case LOG_LEVEL_DEBUG:
                priority = LOG_DEBUG;
                break;
        case LOG_LEVEL_INFO:
                priority = LOG_INFO;
                break;
        case LOG_LEVEL_NOTICE:
                priority = LOG_NOTICE;
                break;
        case LOG_LEVEL_WARNING:
                priority = LOG_WARNING;
                break;
        default:
                priority = LOG_INFO;
                break;
        }
        va_start(ap, fmt);
        vsyslog(priority, format, ap);
        va_end(ap);
    }

    free(format);
}

/* System */
int pipe_command(char *buffer, int buffer_size, char *cmd, char *parameter[]) {
    int pipefds[2], len = 0;
    pid_t pid;

    if (pipe(pipefds) == -1){
        indigo_logger(LOG_LEVEL_ERROR, "Failed to create the pipe");
        return -1;
    }

    pid = fork();
    if (pid == -1) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to fork");
        return -1;
    }

    if (pid == 0) {
        // Replace stdout with the write end of the pipe
        dup2(pipefds[1], STDOUT_FILENO);
        // Close read to pipe, in child
        close(pipefds[0]);
        execv(cmd, parameter);
        exit(EXIT_SUCCESS);
    } else {
        close(pipefds[1]);
        len = read(pipefds[0], buffer, buffer_size);
        indigo_logger(LOG_LEVEL_DEBUG_VERBOSE, "Pipe system call= %s, Return length= %d, result= %s", cmd, len, buffer);
        close(pipefds[0]);
        wait(NULL); /* Parent waits for the child to terminate */
    }
    return len;
}

char* read_file(char *fn) {
    struct stat st;
    int fd, size;
    char *buffer = NULL;

    memset(&st, 0, sizeof(struct stat));
    stat(fn, &st);
    size = st.st_size;

    fd = open(fn, O_RDONLY);
    if (fd) {
        buffer = (char*)malloc(sizeof(char)*(size+1));
        if (!buffer) {
            close(fd);
            return NULL;
        }
        memset(buffer, 0, size+1);
        read(fd, buffer, size);
        close(fd);
    }
    return buffer;
}

int write_file(char *fn, char *buffer, int len) {
    int fd;

#if defined(ANDROID) || defined(MDM)
    fd = open(fn, O_CREAT | O_WRONLY | O_TRUNC, RD_WR_EN);
#else
    fd = open(fn, O_CREAT | O_WRONLY | O_TRUNC);
#endif
    if (fd > 0) {
        (void)write(fd, buffer, len);
        close(fd);
        return 0;
    }
    return -1;
}

int append_file(char *fn, char *buffer, int len) {
    int fd;

#if defined(ANDROID) || defined(MDM)
    fd = open(fn, O_CREAT | O_WRONLY | O_APPEND, RD_WR_EN);
#else
    fd = open(fn, O_CREAT | O_WRONLY | O_APPEND);
#endif
    if (fd > 0) {
        (void)write(fd, buffer, len);
        close(fd);
        return 0;
    }
    return -1;
}

/* strrstr(), reversed strstr(), is not available in some compilers. Here is the implementation. */
static char* indigo_strrstr(char *input, const char *token) {
    char *result = NULL, *p = NULL;

    if (*token == '\0') {
        return (char *) input;
    }

    while (1) {
        p = strstr(input, token);
        if (p == NULL)
            break;
        result = p;
        input = p + 1;
    }

    return result;
}

/* Loopback */
int loopback_socket = 0;

static void loopback_server_receive_message(int sock, void *eloop_ctx, void *sock_ctx) {
    struct sockaddr_storage from;
    unsigned char buffer[BUFFER_LEN];
    int fromlen, len;

    fromlen = sizeof(from);
    len = recvfrom(sock, buffer, BUFFER_LEN, 0, (struct sockaddr *) &from, (socklen_t*) &fromlen);
    if (len < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Loopback server recvfrom[server] error");
        return ;
    }

    indigo_logger(LOG_LEVEL_INFO, "Loopback server received length = %d", len);

    len = sendto(sock, (const char *)buffer, len, MSG_CONFIRM, (struct sockaddr *)&from, sizeof(from));

    indigo_logger(LOG_LEVEL_INFO, "Loopback server echo back length = %d", len);
}

static void loopback_server_timeout(void *eloop_ctx, void *timeout_ctx) {
    int s = (intptr_t)eloop_ctx;
    eloop_unregister_read_sock(s);
    close(s);
    loopback_socket = 0;
    indigo_logger(LOG_LEVEL_INFO, "Loopback server stops");
}

int loopback_server_start(char *local_ip, char *local_port, int timeout) {
    int s = 0;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

   /* Open UDP socket */
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to open server socket");
        return -1;
    }

    /* Bind specific IP */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (local_ip) {
        addr.sin_addr.s_addr = inet_addr(local_ip);
    }

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to bind server socket");
        close(s);
        return -1;
    }

    if (getsockname(s, (struct sockaddr *)&addr, &len) == -1) {
        indigo_logger(LOG_LEVEL_INFO, "Failed to get socket port number");
        close(s);
        return -1;
    } else {
        indigo_logger(LOG_LEVEL_INFO, "loopback server port number %d\n", ntohs(addr.sin_port));
        snprintf(local_port, 16, "%d", ntohs(addr.sin_port));
    }

    /* Register to eloop and ready for the socket event */
    if (eloop_register_read_sock(s, loopback_server_receive_message, NULL, NULL)) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to initiate ControlAppC");
        close(s);
        return -1;
    }
    loopback_socket = s;
    eloop_register_timeout(timeout, 0, loopback_server_timeout, (void*)(intptr_t)s, NULL);
    indigo_logger(LOG_LEVEL_INFO, "Loopback Client starts ip %s port %s", local_ip, local_port);

    return 0;
}

int loopback_server_stop(void) {
    if (loopback_socket) {
        eloop_cancel_timeout(loopback_server_timeout, (void*)(intptr_t)loopback_socket, NULL);
        eloop_unregister_read_sock(loopback_socket);
        close(loopback_socket);
        loopback_socket = 0;
    }
    return 0;
}

int loopback_server_status(void) {
    return !!loopback_socket;
}

unsigned short icmp_checksum(unsigned short *buf, int size)
{
    unsigned long sum = 0;
    while (size > 1) {
        sum += *buf;
        buf++;
        size -= 2;
    }
    if (size == 1)
        sum += *(unsigned char *)buf;
    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);
    return ~sum;
}

void setup_icmphdr(u_int8_t type, u_int8_t code, u_int16_t id,
    u_int16_t seq, struct icmphdr *icmphdr, int packet_size)
{
    memset(icmphdr, 0, sizeof(struct icmphdr));
    icmphdr->type = type;
    icmphdr->code = code;
    icmphdr->un.echo.id = id;
    icmphdr->un.echo.sequence = seq;
    icmphdr->checksum = icmp_checksum((unsigned short *)icmphdr, packet_size);
}

void send_one_loopback_icmp_packet(struct loopback_info *info) {
    int n;
    char server_reply[1600];
    struct in_addr insaddr;
    struct icmphdr *icmphdr, *recv_icmphdr;
    struct iphdr *recv_iphdr;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(info->target_ip);

    icmphdr = (struct icmphdr *)&info->message;

    info->pkt_sent++;
    setup_icmphdr(ICMP_ECHO, 0, 0, info->pkt_sent, icmphdr, info->pkt_size);

    n = sendto(info->sock, (char *)info->message, info->pkt_size, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (n < 0) {
        indigo_logger(LOG_LEVEL_WARNING, "Send failed on icmp packet %d", info->pkt_sent);
        goto done;
    }
    indigo_logger(LOG_LEVEL_INFO, "Packet %d: Send icmp %d bytes data to ip %s",
                  info->pkt_sent, n, info->target_ip);

    memset(&server_reply, 0, sizeof(server_reply));
    n = recv(info->sock, server_reply, sizeof(server_reply), 0);
    if (n < 0) {
        indigo_logger(LOG_LEVEL_WARNING, "recv failed on icmp packet %d", info->pkt_sent);
        goto done;
    } else {
        recv_iphdr = (struct iphdr *)server_reply;
        recv_icmphdr = (struct icmphdr *)(server_reply + (recv_iphdr->ihl << 2));
        insaddr.s_addr = recv_iphdr->saddr;

        if (!strcmp(info->target_ip, inet_ntoa(insaddr)) && recv_icmphdr->type == ICMP_ECHOREPLY) {
            indigo_logger(LOG_LEVEL_INFO, "icmp echo reply from %s, Receive echo %d bytes data", info->target_ip, n - 20);
            info->pkt_rcv++;
        } else {
            indigo_logger(LOG_LEVEL_INFO, "Received packet is not the ICMP reply from the DUT");
        }
    }

done:
    eloop_register_timeout(0, info->rate * 1000000, send_continuous_loopback_packet, info, NULL);
}

void send_one_loopback_udp_packet(struct loopback_info *info) {
    char server_reply[1600];
    ssize_t recv_len = 0, send_len = 0;

    memset(&server_reply, 0, sizeof(server_reply));

    info->pkt_sent++;
    send_len = send(info->sock, info->message, strlen(info->message), 0);
    if (send_len < 0) {
        indigo_logger(LOG_LEVEL_INFO, "Send failed on packet %d", info->pkt_sent);
        // In case Tool doesn't send stop or doesn't receive stop
        if (info->pkt_sent < 1000)
            eloop_register_timeout(0, info->rate*1000000, send_continuous_loopback_packet, info, NULL);
        return;
    }
    indigo_logger(LOG_LEVEL_INFO, "Packet %d: Send loopback %d bytes data",
            info->pkt_sent, send_len);

    recv_len = recv(info->sock, server_reply, sizeof(server_reply), 0);
    if (recv_len < 0) {
        indigo_logger(LOG_LEVEL_INFO, "recv failed on packet %d", info->pkt_sent);
        // In case Tool doesn't send stop or doesn't receive stop
        if (info->pkt_sent < 1000)
            eloop_register_timeout(0, info->rate*1000000, send_continuous_loopback_packet, info, NULL);
        return;
    }
    info->pkt_rcv++;
    indigo_logger(LOG_LEVEL_INFO, "Receive echo %d bytes data", recv_len);

    eloop_register_timeout(0, info->rate*1000000, send_continuous_loopback_packet, info, NULL);
}

void send_continuous_loopback_packet(void *eloop_ctx, void *sock_ctx) {
    struct loopback_info *info = (struct loopback_info *)eloop_ctx;

    if (info->pkt_type == DATA_TYPE_ICMP) {
        send_one_loopback_icmp_packet(info);
    } else {
        send_one_loopback_udp_packet(info);
    }
}

/* Stop to send continuous loopback data */
int stop_loopback_data(int *pkt_sent)
{
    if (loopback.sock <= 0)
        return 0;

    eloop_cancel_timeout(send_continuous_loopback_packet, &loopback, NULL);
    close(loopback.sock);
    loopback.sock = 0;
    if (pkt_sent)
        *pkt_sent = loopback.pkt_sent;

    return loopback.pkt_rcv;
}

int send_udp_data(char *target_ip, int target_port, int packet_count, int packet_size, double rate) {
    int s = 0, i = 0;
    struct sockaddr_in addr;
    int pkt_sent = 0, pkt_rcv = 0;
    char message[1600], server_reply[1600], ifname[32];
    ssize_t recv_len = 0, send_len = 0;
    struct timeval timeout;

    /* Open UDP socket */
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to open socket");
        return -1;
    }

    if (rate < 1) {
        timeout.tv_sec = 0;
        timeout.tv_usec = rate * 1000000;
    } else {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }
    snprintf(ifname, sizeof(ifname), "%s", get_wireless_interface());
    const int len = strnlen(ifname, IFNAMSIZ);
    if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, ifname, len) < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "failed to bind the interface %s", ifname);
        close(s);
        return -1;
    }

    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (target_ip) {
        addr.sin_addr.s_addr = inet_addr(target_ip);
    }
    addr.sin_port = htons(target_port);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Connect failed. Error");
        close(s);
        return -1;
    }

    indigo_logger(LOG_LEVEL_INFO, "packet_count %d rate %lf\n",
                  packet_count, rate);

    /* Continuous data case: reply OK and use eloop timeout to send data */
    if (packet_count == -1) {
        loopback.sock = s;
        loopback.pkt_type = DATA_TYPE_UDP;
        loopback.rate = rate;
        loopback.pkt_sent = loopback.pkt_rcv = 0;
        memset(loopback.message, 0, sizeof(loopback.message));
        for (i = 0; (i < packet_size) && (i < sizeof(loopback.message)); i++)
            loopback.message[i] = 0x0A;
        eloop_register_timeout(0, 0, send_continuous_loopback_packet, &loopback, NULL);
        indigo_logger(LOG_LEVEL_INFO, "Send continuous loopback data to ip %s port %u",
                      target_ip, target_port);
        return 0;
    }

    memset(message, 0, sizeof(message));
    for (i = 0; (i < packet_size) && (i < sizeof(message)); i++)
        message[i] = 0x0A;

    for (pkt_sent = 1; pkt_sent <= packet_count; pkt_sent++) {
        memset(&server_reply, 0, sizeof(server_reply));

        send_len = send(s, message, strlen(message), 0);
        if (send_len < 0) {
            indigo_logger(LOG_LEVEL_INFO, "Send failed on packet %d", pkt_sent);
            usleep(rate*1000000);
            continue;
        }
        indigo_logger(LOG_LEVEL_INFO, "Packet %d: Send loopback %d bytes data to ip %s port %u",
                      pkt_sent, send_len, target_ip, target_port);

        recv_len = recv(s, server_reply, sizeof(server_reply), 0);
        if (recv_len < 0) {
            indigo_logger(LOG_LEVEL_INFO, "recv failed on packet %d", pkt_sent);
            if (rate > 1)
                usleep((rate-1)*1000000);
            continue;
        }
        pkt_rcv++;
        usleep(rate*1000000);

        indigo_logger(LOG_LEVEL_INFO, "Receive echo %d bytes data", recv_len);
    }
    close(s);

    return pkt_rcv;
}

int send_icmp_data(char *target_ip, int packet_count, int packet_size, double rate)
{
    int n, sock, i;
    char buf[1600], server_reply[1600], ifname[32];
    struct sockaddr_in addr;
    struct in_addr insaddr;
    struct icmphdr *icmphdr, *recv_icmphdr;
    struct iphdr *recv_iphdr;
    struct timeval timeout;
    int pkt_sent = 0, pkt_rcv = 0;

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0) {
        return -1;
	}

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(target_ip);

    if (rate < 1) {
        timeout.tv_sec = 0;
        timeout.tv_usec = rate * 1000000;
    } else {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }

    snprintf(ifname, sizeof(ifname), "%s", get_wireless_interface());
    const int len = strnlen(ifname, IFNAMSIZ);
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, ifname, len) < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "failed to bind the interface %s", ifname);
        return -1;
    }

    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

    /* Continuous data case: reply OK and use eloop timeout to send data */
    if (packet_count == -1) {
        memset(&loopback, 0, sizeof(loopback));
        loopback.sock = sock;
        loopback.pkt_type = DATA_TYPE_ICMP;
        loopback.rate = rate;
        loopback.pkt_size = packet_size;
        snprintf(loopback.target_ip, sizeof(loopback.target_ip), "%s", target_ip);
        for (i = sizeof(struct icmphdr); (i < packet_size) && (i < sizeof(loopback.message)); i++)
            loopback.message[i] = 0x0A;
        eloop_register_timeout(0, 0, send_continuous_loopback_packet, &loopback, NULL);
        indigo_logger(LOG_LEVEL_INFO, "Send continuous loopback data to ip %s", loopback.target_ip);
        return 0;
    }

    icmphdr = (struct icmphdr *)&buf;
    memset(&buf, 0, sizeof(buf));
    for (i = sizeof(struct icmphdr); (i < packet_size) && (i < sizeof(buf)); i++)
        buf[i] = 0x0A;

    for (pkt_sent = 1; pkt_sent <= packet_count; pkt_sent++) {
        memset(&server_reply, 0, sizeof(server_reply));
        setup_icmphdr(ICMP_ECHO, 0, 0, pkt_sent, icmphdr, packet_size);

        n = sendto(sock, (char *)buf, packet_size, 0, (struct sockaddr *)&addr, sizeof(addr));
        if (n < 0) {
            indigo_logger(LOG_LEVEL_WARNING, "Send failed on icmp packet %d", pkt_sent);
            usleep(rate * 1000000);
            continue;
        }
        indigo_logger(LOG_LEVEL_INFO, "Packet %d: Send icmp %d bytes data to ip %s",
                      pkt_sent, n, target_ip);

        n = recv(sock, server_reply, sizeof(server_reply), 0);
        if (n < 0) {
            indigo_logger(LOG_LEVEL_WARNING, "recv failed on icmp packet %d", pkt_sent);
            if (rate > 1)
                usleep((rate - 1) * 1000000);
            continue;
        } else {
            recv_iphdr = (struct iphdr *)server_reply;
            recv_icmphdr = (struct icmphdr *)(server_reply + (recv_iphdr->ihl << 2));
            insaddr.s_addr = recv_iphdr->saddr;

            if (!strcmp(target_ip, inet_ntoa(insaddr)) && recv_icmphdr->type == ICMP_ECHOREPLY) {
                /* IP header 20 bytes */
                indigo_logger(LOG_LEVEL_INFO, "icmp echo reply from %s, Receive echo %d bytes data", target_ip, n - 20);
                pkt_rcv++;
            } else {
                indigo_logger(LOG_LEVEL_INFO, "Received packet is not the ICMP reply from the DUT");
            }
        }
        usleep(rate * 1000000);
    }

    close(sock);
    return pkt_rcv;
}

int send_broadcast_arp(char *target_ip, int *send_count, int rate) {
    char buffer[S_BUFFER_LEN];
    FILE *fp;
    int recv = 0;

#ifdef _OPENWRT_
    snprintf(buffer, sizeof(buffer), "arping -I %s %s -c %d -b | grep broadcast", get_wireless_interface(), target_ip, *send_count);
#else
    snprintf(buffer, sizeof(buffer), "arping -i %s %s -c %d -W %d | grep packet", get_wireless_interface(), target_ip, *send_count, rate);
#endif
    fp = popen(buffer, "r");
    if (fp == NULL)
        return 0;
#ifdef _OPENWRT_
    //Format: Sent 3 probe(s) (3 broadcast(s))
    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%*s %d", send_count);
    //Format: Received 0 reply (0 request(s), 0 broadcast(s))
    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%*s %d", &recv);
#else
    //arping output format: 1 packets transmitted, 1 packets received,   0% unanswered (0 extra)
    fscanf(fp, "%d %*s %*s %d", send_count, &recv);
#endif
    indigo_logger(LOG_LEVEL_INFO, "ARP TEST - send: %d recv: %d", *send_count, recv );
    pclose(fp);

    return recv;
}

int find_interface_ip(char *ipaddr, int ipaddr_len, char *name) {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr = NULL;

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, name) == 0) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            if (ipaddr) {
                strlcpy(ipaddr, addr, ipaddr_len);
            }
            return 1;
        }
    }
    freeifaddrs(ifap);
    return 0;
}

int get_mac_address(char *buffer, int size, char *interface) {
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (fd <= 0) {
        goto done;
    }
    strlcpy(s.ifr_name, interface, sizeof(s.ifr_name));
    if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
        snprintf(buffer, size, "%02x:%02x:%02x:%02x:%02x:%02x",
            (char)s.ifr_addr.sa_data[0]&0x00ff, (char)s.ifr_addr.sa_data[1]&0x00ff, (char)s.ifr_addr.sa_data[2]&0x00ff,
            (char)s.ifr_addr.sa_data[3]&0x00ff, (char)s.ifr_addr.sa_data[4]&0x00ff, (char)s.ifr_addr.sa_data[5]&0x00ff);
        close(fd);
        return 0;
    }
    close(fd);
done:
    return 1;
}

int set_mac_address(char *ifname, char *mac) {
    char cmd[S_BUFFER_LEN];
    /* If the system doesn't support ip command, please use ifconfig. E.g., */
    /* snprintf(cmd, sizeof(cmd), "ifconfig %s hw ether %s", ifname, mac_addr) */
    snprintf(cmd, sizeof(cmd), "ip link set dev %s address %s", ifname, mac);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, cmd);
    return system(cmd);
}

#ifdef _OPENWRT_QTI_
int bridge_created = 1;
#else
int bridge_created = 0;
#endif

int is_bridge_created(void) {
    return bridge_created;
}

void bridge_init(char *br) {
    /* Create bridge for multiple VAPs */
    if (configured_interface_count >= 2) {
        create_bridge(br);
        add_all_wireless_interface_to_bridge(br);
    }
}

int create_bridge(char *br) {
    char cmd[S_BUFFER_LEN];

    /* Create new bridge */
    snprintf(cmd, sizeof(cmd), "brctl addbr %s", br);
    system(cmd);

    /* Bring up bridge */
    control_interface(br, "up");

    bridge_created = 1;

    return 0;
}

int add_interface_to_bridge(char *br, char *ifname) {
    char cmd[S_BUFFER_LEN];

    /* Reset IP address */
    reset_interface_ip(ifname);

    /* Add interface to bridge */
    snprintf(cmd, sizeof(cmd), "brctl addif %s %s", br, ifname);
    system(cmd);
    printf("%s\n", cmd);

    return 0;
}

int reset_bridge(char *br) {
    char cmd[S_BUFFER_LEN];

    /* Bring down bridge */
    control_interface(br, "down");
    snprintf(cmd, sizeof(cmd), "brctl delbr %s", br);
    system(cmd);

    bridge_created = 0;

    return 0;
}

int add_wireless_interface(char *ifname) {
    char cmd[S_BUFFER_LEN];

    snprintf(cmd, sizeof(cmd), "iw dev %s interface add %s type managed", get_wireless_interface(), ifname);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, cmd);
    system(cmd);

    return 0;
}

int delete_wireless_interface(char *ifname) {
    char cmd[S_BUFFER_LEN];

    snprintf(cmd, sizeof(cmd), "iw dev %s del", ifname);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, cmd);
    system(cmd);

    return 0;
}

int control_interface(char *ifname, char *op) {
    char cmd[S_BUFFER_LEN];
    /* If the system doesn't support ip command, please use ifconfig. E.g., */
    /* snprintf(cmd, sizeof(cmd), "ifconfig %s %s", ifname, op); */
    snprintf(cmd, sizeof(cmd), "ip link set %s %s", ifname, op);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, cmd);
    system(cmd);

    return 0;
}

int set_interface_ip(char *ifname, char *ip) {
    char cmd[S_BUFFER_LEN];
    /* If the system doesn't support ip command, please use ifconfig. */
    /* Please also update the caller to use netmask instead of CIDR. E.g., */
    /* snprintf(cmd, sizeof(cmd), "ifconfig %s %s", ifname, ip); */
    snprintf(cmd, sizeof(cmd), "ip addr add %s dev %s", ip, ifname);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, cmd);
    system(cmd);

    return 0;
}

int reset_interface_ip(char *ifname) {
    char cmd[S_BUFFER_LEN];
    /* If the system doesn't support ip command, please use ifconfig. E.g., */
    /* snprintf(cmd, sizeof(cmd), "ifconfig %s 0.0.0.0", ifname); */
    snprintf(cmd, sizeof(cmd), "ip addr flush dev %s", ifname);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, cmd);
    return system(cmd);
}

void detect_del_arp_entry(char *ip) {
    char buffer[S_BUFFER_LEN];
    char res_ip[32], res_dev[16], res_inf[16];
    FILE *fp;

    snprintf(buffer, sizeof(buffer), "ip neigh show %s", ip);
    fp = popen(buffer, "r");
    if (fp == NULL)
        return;

    if (NULL == fgets(buffer, sizeof(buffer), fp)) {
    } else if (3 == sscanf(buffer, "%s %s %s", res_ip, res_dev, res_inf)) {
        if (!strcmp(res_ip, ip) && !strcmp(res_dev, "dev")) {
            indigo_logger(LOG_LEVEL_INFO, "Delete existing ARP entry: %s", ip);
            snprintf(buffer, sizeof(buffer), "ip neigh del %s %s %s", res_ip, res_dev, res_inf);
            system(buffer);
        } else {
            indigo_logger(LOG_LEVEL_INFO, "Format mismatch?: %s %s %s\n", res_ip, res_dev, res_inf);
        }
    }
    pclose(fp);

    return;
}

int add_all_wireless_interface_to_bridge(char *br) {
    int i;

    for (i = 0; i < interface_count; i++) {
        if (interfaces[i].identifier != UNUSED_IDENTIFIER) {
            control_interface(interfaces[i].ifname, "up");
            add_interface_to_bridge(br, interfaces[i].ifname);
        }
    }

    return 0;
}

/* Environment */
int service_port = SERVICE_PORT_DEFAULT;

char hapd_exec_file[64];
char hapd_full_exec_path[64] = HAPD_EXEC_FILE_DEFAULT;
char hapd_ctrl_path[64] = HAPD_CTRL_PATH_DEFAULT;
char hapd_full_ctrl_path[128];
char hapd_global_ctrl_path[64] = HAPD_GLOBAL_CTRL_PATH_DEFAULT;
char hapd_conf_file[64] = HAPD_CONF_FILE_DEFAULT;
int hostapd_debug_level = DEBUG_LEVEL_DISABLE;

char wpas_exec_file[64];
char wpas_full_exec_path[64] = WPAS_EXEC_FILE_DEFAULT;
char wpas_ctrl_path[64] = WPAS_CTRL_PATH_DEFAULT;
char wpas_full_ctrl_path[128];
char wpas_global_ctrl_path[64] = WPAS_GLOBAL_CTRL_PATH_DEFAULT;
char wpas_conf_file[64] = WPAS_CONF_FILE_DEFAULT;
int wpas_debug_level = DEBUG_LEVEL_DISABLE;

struct interface_info* assign_wireless_interface_info(struct bss_identifier_info *bss) {
    int i;

    for (i = 0; i < interface_count; i++) {
        if ((interfaces[i].band == bss->band) &&
             (interfaces[i].identifier == UNUSED_IDENTIFIER)) {
            configured_interface_count++;
            interfaces[i].identifier = bss->identifier;
            interfaces[i].mbssid_enable = bss->mbssid_enable;
            interfaces[i].transmitter = bss->transmitter;
            if (bss->mbssid_enable) {
                interfaces[i].hapd_bss_id = band_mbssid_cnt[bss->band];
                band_mbssid_cnt[bss->band]++;
            }
            memset(interfaces[i].hapd_conf_file, 0, sizeof(interfaces[i].hapd_conf_file));
            snprintf(interfaces[i].hapd_conf_file, sizeof(interfaces[i].hapd_conf_file),
                     "%s/hostapd_%s.conf", HAPD_CONF_FILE_DEFAULT_PATH, interfaces[i].ifname);
            return &interfaces[i];
        }
    }

    return NULL;
}

struct interface_info* get_wireless_interface_info(int band, int identifier) {
    int i;

    for (i = 0; i < interface_count; i++) {
        if ((interfaces[i].band == band) &&
             ((interfaces[i].identifier != UNUSED_IDENTIFIER) &&
              (interfaces[i].identifier == identifier))) {
            return &interfaces[i];
        }
    }

    return NULL;
}

int get_debug_level(int value) {
    if (value == 0) {
        return DEBUG_LEVEL_DISABLE;
    } else if (value == 1) {
        return DEBUG_LEVEL_BASIC;
    }
    return DEBUG_LEVEL_ADVANCED;
}

/* get hostapd's file name */
char* get_hapd_exec_file(void) {
    return hapd_exec_file;
}

/* parse hostapd full path and set hostapd's file name */
int set_hapd_exec_file(char* path) {
    char *ptr = indigo_strrstr(path, "/");

    if (ptr) {
        strlcpy(hapd_exec_file, ptr+1, sizeof(hapd_exec_file));
    } else {
        strlcpy(hapd_exec_file, path, sizeof(hapd_exec_file));
    }
    return 0;
}

/* get hostapd's full path */
char* get_hapd_full_exec_path(void) {
    return hapd_full_exec_path;
}

/* set hostapd's full path */
int set_hapd_full_exec_path(char* path) {
    memset(hapd_full_exec_path, 0, sizeof(hapd_full_exec_path));
    snprintf(hapd_full_exec_path, sizeof(hapd_full_exec_path), "%s", path);

    set_hapd_exec_file(hapd_full_exec_path);
    return 0;
}

char* get_hapd_ctrl_path_by_id(struct interface_info* wlan) {
    memset(hapd_full_ctrl_path, 0, sizeof(hapd_full_ctrl_path));
    if (wlan) {
        snprintf(hapd_full_ctrl_path, sizeof(hapd_full_ctrl_path), "%s/%s", hapd_ctrl_path, wlan->ifname);
    }
    else {
        snprintf(hapd_full_ctrl_path, sizeof(hapd_full_ctrl_path), "%s/%s", hapd_ctrl_path, get_default_wireless_interface_info());
    }
    printf("hapd_full_ctrl_path: %s, wlan %p\n", hapd_full_ctrl_path, wlan);
    return hapd_full_ctrl_path;
}

char* get_hapd_ctrl_path(void) {
    memset(hapd_full_ctrl_path, 0, sizeof(hapd_full_ctrl_path));
#if defined(ANDROID) || defined(MDM)
    snprintf(hapd_full_ctrl_path, sizeof(hapd_full_ctrl_path), "%s", hapd_ctrl_path);
#else
    snprintf(hapd_full_ctrl_path, sizeof(hapd_full_ctrl_path), "%s/%s", hapd_ctrl_path, get_default_wireless_interface_info());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: CTRL_PATH: %s", __func__, hapd_full_ctrl_path);
    return hapd_full_ctrl_path;
}

int set_hapd_ctrl_path(char* path) {
    memset(hapd_ctrl_path, 0, sizeof(hapd_ctrl_path));
    snprintf(hapd_ctrl_path, sizeof(hapd_ctrl_path), "%s", path);
    return 0;
}

char* get_hapd_global_ctrl_path(void) {
    return hapd_global_ctrl_path;
}

int set_hapd_global_ctrl_path(char* path) {
    memset(hapd_global_ctrl_path, 0, sizeof(hapd_global_ctrl_path));
    snprintf(hapd_global_ctrl_path, sizeof(hapd_global_ctrl_path), "%s", path);
    return 0;
}

char* get_hapd_conf_file(void) {
    return hapd_conf_file;
}

int set_hapd_conf_file(char* path) {
    memset(hapd_conf_file, 0, sizeof(hapd_conf_file));
    snprintf(hapd_conf_file, sizeof(hapd_conf_file), "%s", path);
    return 0;
}

void set_hostapd_debug_level(int level) {
    hostapd_debug_level = level;
}

char* get_hostapd_debug_arguments(void) {
    if (hostapd_debug_level == DEBUG_LEVEL_ADVANCED) {
        return "-dddK";
    } else if (hostapd_debug_level == DEBUG_LEVEL_BASIC) {
        return "-dK";
    }
    return "";
}

char* get_wpas_exec_file(void) {
    return wpas_exec_file;
}

int set_wpas_exec_file(char* path) {
    char *ptr = indigo_strrstr(path, "/");
    if (ptr) {
        strlcpy(wpas_exec_file, ptr+1, sizeof(wpas_exec_file));
    } else {
        strlcpy(wpas_exec_file, path, sizeof(wpas_exec_file));
    }
    return 0;
}

char* get_wpas_full_exec_path(void) {
    return wpas_full_exec_path;
}

int set_wpas_full_exec_path(char* path) {
    memset(wpas_full_exec_path, 0, sizeof(wpas_full_exec_path));
    snprintf(wpas_full_exec_path, sizeof(wpas_full_exec_path), "%s", path);

    set_wpas_exec_file(wpas_full_exec_path);
    return 0;
}

char* get_wpas_ctrl_path(void) {
    memset(wpas_full_ctrl_path, 0, sizeof(wpas_full_ctrl_path));
#if defined(ANDROID) || defined(MDM)
    snprintf(wpas_full_ctrl_path, sizeof(wpas_full_ctrl_path), "%s", wpas_ctrl_path);
#else
    snprintf(wpas_full_ctrl_path, sizeof(wpas_full_ctrl_path), "%s/%s", wpas_ctrl_path, get_default_wireless_interface_info());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: CTRL_PATH: %s", __func__, wpas_full_ctrl_path);
    return wpas_full_ctrl_path;
}

int set_wpas_ctrl_path(char* path) {
    snprintf(wpas_ctrl_path, sizeof(wpas_ctrl_path), "%s", path);
    return 0;
}

char* get_wpas_global_ctrl_path(void) {
    return wpas_global_ctrl_path;
}

int set_wpas_global_ctrl_path(char* path) {
    snprintf(wpas_global_ctrl_path, sizeof(wpas_global_ctrl_path), "%s", path);
    return 0;
}

char* get_wpas_conf_file(void) {
    return wpas_conf_file;
}

int set_wpas_conf_file(char* path) {
    memset(wpas_conf_file, 0, sizeof(wpas_conf_file));
    snprintf(wpas_conf_file, sizeof(wpas_conf_file), "%s", path);
    return 0;
}

void set_wpas_debug_level(int level) {
    wpas_debug_level = level;
}

char* get_wpas_debug_arguments(void) {
    if (wpas_debug_level == DEBUG_LEVEL_ADVANCED) {
        return "-ddd";
    } else if (wpas_debug_level == DEBUG_LEVEL_BASIC) {
        return "-d";
    }
    return "";
}

int add_wireless_interface_info(int band, int bssid, char *name) {
    if (interface_count >= 8) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Interface count exceed max supported count.");
        return -1;
    }

    interfaces[interface_count].band = band;
    interfaces[interface_count].bssid = -1;
    interfaces[interface_count].identifier = UNUSED_IDENTIFIER;
    strlcpy(interfaces[interface_count].ifname, name, sizeof(interfaces[interface_count].ifname));
    interface_count++;
    return 0;
}

int show_wireless_interface_info(void) {
    int i;
    char *band;
    indigo_logger(LOG_LEVEL_INFO, "interface_count=%d", interface_count);

    for (i = 0; i < interface_count; i++) {
        if (interfaces[i].band == BAND_24GHZ) {
            band = "2.4GHz";
        } else if (interfaces[i].band == BAND_5GHZ) {
            band = "5GHz";
        } else if (interfaces[i].band == BAND_6GHZ) {
            band = "6GHz";
        } else if (interfaces[i].band == BAND_5GHZH) {
            band = "5GHzH";
        } else {
            indigo_logger(LOG_LEVEL_INFO,"Unknown band %d Intf %s idntfr %d",
                          interfaces[i].band,
                          interfaces[i].ifname,
                          interfaces[i].identifier);
            continue;
        }

        indigo_logger(LOG_LEVEL_INFO, "Interface Name: %s, Band: %s, identifier %d",
            interfaces[i].ifname, band, interfaces[i].identifier);
    }
    return 0;
}

int parse_wireless_interface_info(char *info) {
    char *token = NULL, *saveptr = NULL;
    char *delimit = ",";

    token = strtok_r(info, delimit, &saveptr);

    while(token != NULL) {
        if (strncmp(token, "2:", 2) == 0) {
            add_wireless_interface_info(BAND_24GHZ, -1, token+2);
        } else if (strncmp(token, "5:", 2) == 0) {
            add_wireless_interface_info(BAND_5GHZ, -1, token+2);
        } else if (strncmp(token, "6:", 2) == 0) {
            add_wireless_interface_info(BAND_6GHZ, -1, token+2);
#ifdef _OPENWRT_QTI_
        } else if (strncmp(token, "5h:", 3) == 0) {
            ap_interface_5gh = 1;
            add_wireless_interface_info(BAND_5GHZH, -1, token+3);
#endif
        } else {
            return -1;
        }
        token = strtok_r(NULL, delimit, &saveptr);
    }

    return 0;
}

char* get_default_wireless_interface_info(void) {
    int i;
    for (i = 0; i < interface_count; i++) {
        if (interfaces[i].identifier != UNUSED_IDENTIFIER) {
            return interfaces[i].ifname;
        }
    }
    if (default_interface) {
        return default_interface->ifname;
    }
    else
        return interfaces[0].ifname;
}

void set_default_wireless_interface_info(int band) {
    int i;

    for (i = 0; i < interface_count; i++) {
        if (interfaces[i].band == band) {
            default_interface = &interfaces[i];
            indigo_logger(LOG_LEVEL_DEBUG, "Set default_interface %s", default_interface->ifname);
            break;
        }
    }
}

void reset_default_wireless_interface_info(void) {
    default_interface = NULL;
}

/* Parse BSS IDENTIFIER TLV */
void parse_bss_identifier(int bss_identifier, struct bss_identifier_info* bss) {
    bss->band = bss_identifier & 0x0F;
    bss->identifier = (bss_identifier & 0xF0) >> 4;
    bss->mbssid_enable = (bss_identifier & 0x100) >> 8;
    bss->transmitter = (bss_identifier & 0x200) >> 9;
    return;
}

int clear_interfaces_resource(void) {
    int i, ret = 0;
    for (i = 0; i < interface_count; i++)
    {
        if (interfaces[i].identifier != UNUSED_IDENTIFIER) {
            interfaces[i].identifier = UNUSED_IDENTIFIER;
        }
    }
    configured_interface_count = 0;
    memset(band_mbssid_cnt, 0, sizeof(band_mbssid_cnt));

    return ret;
}

void iterate_all_wlan_interfaces(void (*callback_fn)(void *)) {
    int i;
    for (i = 0; i < interface_count; i++)
    {
        if (interfaces[i].identifier != UNUSED_IDENTIFIER) {
            callback_fn((void *)&interfaces[i]);
        }
    }

    return ;
}

/* This API is useful only when for provisioning multiple VAPs */
int is_band_enabled(int band) {
    int i;
    for (i = 0; i < interface_count; i++)
    {
        if (interfaces[i].identifier != UNUSED_IDENTIFIER &&
                interfaces[i].band == band) {
            return 1;
        }
    }
    return 0;
}


char* get_all_hapd_conf_files(int *swap_hapd) {
    int i, valid_id_cnt = 0;
    static char conf_files[128];

    memset(conf_files, 0, sizeof(conf_files));
    for (i = 0; i < interface_count; i++) {
        if (interfaces[i].identifier != UNUSED_IDENTIFIER) {
#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
            if (use_openwrt_wpad && !interfaces[i].mbssid_enable) {
                *swap_hapd = 1;
                continue;
            } else if (!use_openwrt_wpad && interfaces[i].mbssid_enable) {
                continue;
            }
#endif
            valid_id_cnt++;
            strlcat(conf_files, interfaces[i].hapd_conf_file, sizeof(conf_files));
            strlcat(conf_files, " ", sizeof(conf_files));
        }
    }
    if (valid_id_cnt)
        return conf_files;
    else
        return hapd_conf_file;
}

char* get_wireless_interface(void) {
    return get_default_wireless_interface_info();
}

int set_wireless_interface(char *name) {
    memset(interfaces, 0, sizeof(interfaces));
    interface_count = 0;

    if (strstr(name, ":") || strstr(name, ",")) {
        return parse_wireless_interface_info(name);
    } else {
#ifdef _LAPTOP_
        add_wireless_interface_info(BAND_24GHZ, -1, name);
        add_wireless_interface_info(BAND_5GHZ, -1, name);
#else
        return -1;
#endif
    }
    return 0;
}

int get_service_port(void) {
    return service_port;
}

int set_service_port(int port) {
    service_port = port;
    return 0;
}

/* Channel functions */
struct channel_info band_24[] = { {1, 2412}, {2, 2417}, {3, 2422}, {4, 2427}, {5, 2432}, {6, 2437}, {7, 2442}, {8, 2447}, {9, 2452}, {10, 2457}, {11, 2462} };
struct channel_info band_5[] = { {36, 5180}, {40, 5200}, {44, 5220}, {48, 5240}, {52, 5260}, {56, 5280}, {60, 5300}, {64, 5320}, {100, 5500}, {104, 5520}, {108, 5540},
                                 {112, 5560}, {116, 5580}, {120, 5600}, {124, 5620}, {128, 5640}, {132, 5660}, {136, 5680}, {140, 5700}, {144, 5720}, {149, 5745},
                                 {153, 5765}, {157, 5785}, {161, 5805}, {165, 8525} };

int verify_band_from_freq(int freq, int band) {
    struct channel_info *info = NULL;
    int i, size = 0;

    if (band == BAND_24GHZ) {
        info = band_24;
        size = sizeof(band_24)/sizeof(struct channel_info);
    } else if (band == BAND_5GHZ) {
        info = band_5;
        size = sizeof(band_5)/sizeof(struct channel_info);
    } else if (band == BAND_6GHZ) {
        if (freq >= (5950 + 5*1) && freq <= (5950 + 5*233))
            return 0;
        else
            return -1;
    }

    for (i = 0; i < size; i++) {
        if (freq == info[i].freq) {
            return 0;
        }
    }

    return -1;
}

int get_center_freq_index(int channel, int width) {
    if (width == 1) {
        if (channel >= 36 && channel <= 48) {
            return 42;
        } else if (channel <= 64) {
            return 58;
        } else if (channel >= 100 && channel <= 112) {
            return 106;
        } else if (channel <= 128) {
            return 122;
        } else if (channel <= 144) {
            return 138;
        } else if (channel >= 149 && channel <= 161) {
            return 155;
        }
    } else if (width == 2) {
        if (channel >= 36 && channel <= 64) {
            return 50;
        } else if (channel >= 36 && channel <= 64) {
            return 114;
        }
    }
    return 0;
}

int get_6g_center_freq_index(int channel, int width) {
    int chwidth, i;

    if (width == 1) {
        chwidth = 80;
    } else if (width == 2) {
        chwidth = 160;
    } else {
        return channel;
    }

    for (i=1; i<233; i+=chwidth/5) {
        if (channel >= i && channel < i + chwidth/5)
            return i + (chwidth - 20)/10;
    }
    return 0;
}

int is_ht40plus_chan(int chan) {
    if (chan == 36 || chan == 44 || chan == 52 || chan == 60 ||
        chan == 100 || chan == 108 || chan == 116 || chan == 124 ||
        chan == 132 || chan == 140 || chan == 149 || chan == 157)
        return 1;
    else
        return 0;
}

int is_ht40minus_chan(int chan) {
    if (chan == 40 || chan == 48 || chan == 56 || chan == 64 ||
        chan == 104 || chan == 112 || chan == 120 || chan == 128 ||
        chan == 136 || chan == 144 || chan == 153 || chan == 161)
        return 1;
    else
        return 0;
}

/* String operation */
size_t strlcpy(char *dest, const char *src, size_t siz) {
	const char *s = src;
	size_t left = siz;

	if (left) {
		/* Copy string up to the maximum size of the dest buffer */
		while (--left != 0) {
			if ((*dest++ = *s++) == '\0')
				break;
		}
	}

	if (left == 0) {
		/* Not enough room for the string; force NUL-termination */
		if (siz != 0)
			*dest = '\0';
		while (*s++)
			; /* determine total src string length */
	}

	return s - src - 1;
}

int get_key_value(char *value, char *buffer, char *token) {
    char *ptr = NULL, *endptr = NULL;
    char _token[S_BUFFER_LEN];

    if (!value || !buffer || !token) {
        return -1;
    }

    memset(_token, 0, sizeof(_token));
    snprintf(_token, sizeof(_token), "\n%s=", token);
    ptr = strstr(buffer, _token);
    if (!ptr) {
        snprintf(_token, sizeof(_token), "%s=", token);
        if (strncmp(buffer, _token, strlen(_token)) == 0) {
            ptr = buffer;
        }
    }

    if (!ptr) {
        return -1;
    }

    ptr += strlen(_token);
    endptr = strstr(ptr, "\n");
    if (endptr) {
        memcpy(value, ptr, endptr - ptr);
    } else {
        strlcpy(value, ptr, S_BUFFER_LEN);
    }

    return 0;
}

/*
 *       These were generated with: openssl x509 -outform der -in $pemname | openssl dgst -sha256
 *       "rsa_server1_w1_fi.pem": "a7407d995678712bb7adb4e7a75e89674aba363dea0b8308c63b006329b0de2d",
 *       "rsa_server1ALT_w1_fi.pem": "79a9d7273368bee41566f79ae9fc84119f7c963cf8cfac5984e2e0adaeafb112",
 *       "rsa_server2_w1_fi.pem": "8d0e00b924e30f4595ae7f5ef9f1346e2c3f343dfb1caf1429b3bb6b32a1bf03",
 *       "rsa_server4_w1_fi.pem": "2703264d2d06727be661752ff5b57e85f842dc74e18aaa03316e7b2d08db6260",
 */
void get_server_cert_hash(char *pem_file, char *buffer, int size) {
#define NUM_ITEMS 4

    char file[NUM_ITEMS][32] = {
        "rsa_server1_w1_fi.pem",
        "rsa_server1ALT_w1_fi.pem",
        "rsa_server2_w1_fi.pem",
        "rsa_server4_w1_fi.pem"};
    char hash[NUM_ITEMS][128] = {"a7407d995678712bb7adb4e7a75e89674aba363dea0b8308c63b006329b0de2d",
                                 "79a9d7273368bee41566f79ae9fc84119f7c963cf8cfac5984e2e0adaeafb112",
                                 "8d0e00b924e30f4595ae7f5ef9f1346e2c3f343dfb1caf1429b3bb6b32a1bf03",
                                 "2703264d2d06727be661752ff5b57e85f842dc74e18aaa03316e7b2d08db6260"};
    int i = 0;

    for(i = 0; i< NUM_ITEMS; i++) {
        if (strcmp(file[i], pem_file) == 0) {
            snprintf(buffer, size, "hash://server/sha256/%s", hash[i]);
        }
    }
}

int insert_wpa_network_config(char *config) {
    FILE *f_ptr, *f_tmp_ptr;
    char *path = get_wpas_conf_file();
    char *tmp_path = "/tmp/wpa_supplicant_tmp1.conf";
    char *target_str = "}"; /* get the last line in network profile */
    char buffer[S_BUFFER_LEN];

    f_ptr = fopen(path, "r");
    if (f_ptr == NULL) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to open the file %s.", path);
        return -1;
    }


    f_tmp_ptr = fopen(tmp_path, "w");
    if (f_tmp_ptr == NULL) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to open the file %s.", tmp_path);
        fclose(f_ptr);
        return -1;
    }

    memset(buffer, 0, sizeof(buffer));
    while ((fgets(buffer, S_BUFFER_LEN, f_ptr)) != NULL) {
        if (strstr(buffer, target_str) != NULL) {
            indigo_logger(LOG_LEVEL_DEBUG,
                "insert config: %s into the wpa_supplicant conf.", config);
            fputs(config, f_tmp_ptr);
        }

        fputs(buffer, f_tmp_ptr);
    }

    fclose(f_ptr);
    fclose(f_tmp_ptr);

    /* replace original file with new file */
    remove(path);
    rename(tmp_path, path);
    return 0;
}

void remove_pac_file(char *path) {
    char pac_path[S_BUFFER_LEN];

    memset(pac_path, 0, sizeof(pac_path));
    if (!strlen(path)) {
        return;
    } else if (path[0] != '/') {
        snprintf(pac_path, sizeof(pac_path), "/%s", path);
    } else {
        snprintf(pac_path, sizeof(pac_path), "%s", path);
    }

    indigo_logger(LOG_LEVEL_INFO, "Remove PAC file: [%s]", pac_path);

    if(access(pac_path, F_OK) == 0) {
        unlink(pac_path);
    }
}

/* HTTP post request */
/* Internal. Generate random string for the boundary */
static void rand_string(char *str, int size) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i = 0, key = 0;

    memset(str, 0, size);
    srand(time(NULL));
    for (i = 0; i < size; i++) {
        key = rand() % (int)(sizeof(charset)-1);
        str[i] = charset[key];
    }
}

/* Internal. Generate the boundary */
static void random_boundary(char *boundary, int size) {
    memset(boundary, '-', size);
    boundary[size-1] = '\0';
    rand_string(&boundary[size - (16+1)], 16);
}

/* Internal. Generate HTTP header for the multipart POST */
static char* http_header_multipart(char *path, char *host, int port, int content_length, char *boundary) {
    char *buffer = NULL;

    buffer = (char*)malloc(sizeof(char)*256);
    if (!buffer)
        return NULL;
    snprintf(buffer, (sizeof(char)*256),
        "POST %s HTTP/1.0\r\n" \
        "Host: %s:%d\r\n" \
        "User-Agent: ControlAppC\r\n" \
        "Accept: */*\r\n" \
        "Content-Length: %d\r\n" \
        "Connection: close\r\n" \
        "Content-Type: multipart/form-data; boundary=%s\r\n\r\n",
        path,
        host,
        port,
        content_length,
        boundary
    );

    return buffer;
}

/* Internal. Generate HTTP body for uploaded file */
static char* http_body_multipart(char *boundary, char *param_name, char *file_name) {
    char *buffer = NULL, *file_content = NULL;
    int body_size = 0, file_size = 0;
    struct stat st;
    char *file_ptr = NULL;

    /* Get the file size and content */
    memset(&st, 0, sizeof(st));
    stat(file_name, &st);
    file_size = st.st_size;
    if (file_size == 0) {
        return buffer;
    }
    file_content = read_file(file_name);

    /* Fill the body buffer */
    body_size = file_size + 256;
    buffer = (char*)malloc(body_size*sizeof(char));
    if (!buffer)
        goto done;
    memset(buffer, 0, body_size);

    file_ptr = indigo_strrstr(file_name, "/");
    if (file_ptr) {
        file_ptr += 1;
    } else {
        file_ptr = file_name;
    }
    snprintf(buffer, (body_size*sizeof(char)),
        "--%s\r\n" \
        "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n" \
        "Content-Type: text/plain\r\n\r\n" \
        "%s\r\n\r\n" \
        "--%s--",
        boundary,
        param_name,
        file_ptr,
        file_content ? file_content : "",
        boundary
    );

done:
    if (file_content)
        free(file_content);

    return buffer;
}

/* Internal. Create HTTP socket */
static int http_socket(char *host, int port) {
    int socketfd = 0;
    struct sockaddr_in server_addr;

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        close(socketfd);
        return -1;
    }

    return socketfd;
}

/*  Upload log by specifying the host, port, path, and the local file name */
void http_file_post(char *host, int port, char *path, char *file_name) {
    int socketfd = 0, numbytes = 0;
    char *header = NULL, *body = NULL;
    char boundary[64];
    char response[10240];

    /* Generate boundary, header and body */
    random_boundary(boundary, 41);
    /* Parameter name needs to match with CompletedFileUpload in API */
    if (!strcmp(path, HAPD_UPLOAD_API))
        body = http_body_multipart(boundary, "hostApdLogFile", file_name);
    else if (!strcmp(path, WPAS_UPLOAD_API))
        body = http_body_multipart(boundary, "wpasLogFile", file_name);
    else {
        indigo_logger(LOG_LEVEL_ERROR, "Tool doesn't support %s ?", path);
        goto done;
    }
    /* Return if body is NULL */
    if (body == NULL) {
        goto done;
    }

    header = http_header_multipart(path, host, port, strlen(body), boundary);

    if (!header)
        goto done;

    socketfd = http_socket(host, port);
    if (send(socketfd, header, strlen(header), 0) == -1){
        indigo_logger(LOG_LEVEL_ERROR, "Failed to open HTTP socket");
        goto done;
    }

    if (send(socketfd, body, strlen(body), 0) == -1){
        indigo_logger(LOG_LEVEL_ERROR, "Failed to upload file");
        goto done;
    }

    while ((numbytes=recv(socketfd, response, sizeof(response), 0)) > 0) {
        response[numbytes] = '\0';
        indigo_logger(LOG_LEVEL_DEBUG, "Server response: %s", response);
    }
    indigo_logger(LOG_LEVEL_INFO, "Upload completes");

done:
    if (header) {
        free(header);
    }
    if (body) {
        free(body);
    }
    if (socketfd) {
        close(socketfd);
    }
}

#if (!defined(ANDROID) && !defined(_OPENWRT_QTI_)) || (defined(MDM))

size_t strlcat(char *dst, const char *str, size_t size)
{
	char *pos;
	size_t dstlen, srclen, copy;

	srclen = strlen(str);
	for (pos = dst; pos - dst < size && *pos != '\0'; pos++)
		;
	dstlen = pos - dst;
	if (*pos != '\0')
		return dstlen + srclen;
	if (dstlen + srclen + 1 > size)
		copy = size - dstlen - 1;
	else
		copy = srclen;
	memcpy(pos, str, copy);
	pos[copy] = '\0';
	return dstlen + srclen;
}

#endif /* ANDROID */
