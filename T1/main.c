#include <stdio.h>
#include <stdint.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define PORT 123
#define NTP_EPOCH_TIMESTAMP 2208988800UL

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
  if (argv[1] == NULL){
    perror("You need to put the host IP address:\n\n./main [Host IP address]\n\n");
    return 0;
  }

  int socket_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), recvlen;
  int try = 0;
  bool succeed, failed;
  struct sockaddr_in server_address;
  struct in_addr host_address;
  struct hostent *host;
  struct timeval timeout={20,0};

  if (socket_udp < 0 ){
    perror("Error opening socket");
    return 0;
  }

  // Set all values to 0 and set the first string byte to 0x1b
  ntp_packet packet = {};
  memset(&packet, 0, sizeof(ntp_packet));

  //                                           li   vn  mode
  // Set li = 0, vn = 3, and mode = 3 (0x1b = [00][011][011])
  *((char *)&packet + 0) = 0x1b;

  // Get host IP from command line
  inet_aton(argv[1], &host_address);
  host = gethostbyaddr(&host_address, sizeof(host_address), AF_INET);

  // Erase the data in the all server_address bytes
  bzero((char*) &server_address, sizeof(server_address));
  // Add IPv4 for the address family
  server_address.sin_family = AF_INET;

  // Copy host address to server_address
  bcopy((char*)host->h_addr, (char*) &server_address.sin_addr.s_addr, host->h_length);

  // Add the socket port
  server_address.sin_port = htons(PORT);

  while(!failed && !succeed){
    // Call up the server and send it the NTP packet 
    recvlen = sendto(socket_udp, &packet, sizeof packet, 0, (struct sockaddr *) &server_address, sizeof(server_address));
    if (recvlen != sizeof packet) {
      perror("Error to send socket UDP");
      close(socket_udp);
      return 0;
    }

    // Wait the server reply
    setsockopt(socket_udp,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));

    // Receive the packet back from the server
    recvlen = recvfrom(socket_udp, &packet, sizeof packet, 0, NULL, NULL);
    if (recvlen >= 0) {
      //Message Received
      packet.txTm_s = ntohl(packet.txTm_s); 
      packet.txTm_f = ntohl(packet.txTm_f);
      time_t datetime = (time_t) (packet.txTm_s - NTP_EPOCH_TIMESTAMP);
      struct tm *tm = localtime(&datetime);
      printf("Data/hora: %s\n", asctime(tm));
      succeed = true;

    } else if (try == 1) {
      perror("Data/hora: não foi possível contactar servidor\n");
      close(socket_udp);
      failed = true;
    } else {
      //Message Receive Timeout or other error
      printf("Trying again...\n");
      try++;
    }
  }

  return 0;
}