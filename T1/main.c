#include <stdio.h>
#include <stdint.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 123

typedef struct {

  uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                           // li.   Two bits.   Leap indicator.
                           // vn.   Three bits. Version number of the protocol.
                           // mode. Three bits. Client will pick mode 3 for client.

  uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  uint8_t precision;       // Eight bits. Precision of the local clock.

  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.

  uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.

int main(int argc, const char* argv[]) {

  int socket_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in server_address;
  struct in_addr address;
  struct hostent *host;

  // Set all values to 0 and set the first string byte to 0x1b
  ntp_packet packet = {};
  memset(&packet, 0, sizeof(ntp_packet));
  *((char *)&packet + 0) = 0x1b;

  time_t datetime = time(NULL);
  struct tm *tm = localtime(&datetime);

  printf("%s\n", asctime(tm));

  inet_aton(argv[1], &address);
  host = gethostbyaddr(&address, sizeof(address), AF_INET);
  printf("%s\n", host->h_addr);

  bzero((char*) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;

  bcopy((char*)host->h_addr, (char*) &server_address.sin_addr.s_addr, host->h_length);

  server_address.sin_port = htons(PORT);

  return 0;
}