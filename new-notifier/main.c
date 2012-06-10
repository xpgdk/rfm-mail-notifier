#include <msp430.h>
#include <net430.h>
#include <stdbool.h>
#include <string.h>

#include <tcp.h>
#include <mem.h>
#include "config.h"
#include "rfm.h"

#define PACKET_BAT_LEVEL        0xF1
#define PACKET_ACK              0xF2
#define PACKET_SIGNAL           0xF3

const uint8_t mac_addr[] = {0x00, 0xC0, 0x033, 0x50, 0x48, 0x10};

const static uint8_t dst[] = {0x26, 0x07, 0xF2, 0x98, 0x00, 0x02, 0x01, 0x20,
                              0x00, 0x00, 0x00, 0x00, 0x02, 0x43, 0xA6, 0x58};

const static char httpResponseHeader[] = "HTTP/1.1 200 OK\r\n"
                                        "Server: net430\r\n"
                                        "Content-Type: application/json\r\n\r\n"
                                        "{'time': $NET_TIME$,"
                                        " 'temperature': $TEMP$,"
                                        " 'requestPath': '$REQUEST$'"
                                        "}";

const static char httpRequest[] = "GET /post-notify.php HTTP/1.0\r\n"
                                  "Host: script.xpg.dk\r\n\r\n";
//"Host: localhost\r\n\r\n";

enum HttpState {
  CLOSED = 0,
  IDLE,
  GOT_REQUEST,
  GOT_PARTIAL_REQUEST
};

static enum HttpState   httpState = CLOSED;
/*static uint16_t         temperature;
static uint16_t         last_measurement = -1;*/
//static char             requestPath[10];
static uint16_t         batLevel = 0;
static bool             sendSignal = false;
static bool             sendRequest = false;
static uint16_t         notificationCounter = 0;

void tcp_send_int(int socket, uint16_t i) {
  uint8_t buf[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
  uint8_t count;

  itoa(i, buf, 10);

  // Count valid chars
  for(count=0; buf[count] >= 48 && buf[count] <= 57; count++);

  tcp_send_data(socket, buf, count);
}

void tcp_send_template_data(int socket, const char *buf, uint16_t count) {
  const char *end = buf + count;
  while (buf < end) {
    const char *p = strchr(buf, '$');
    // Copy data up to p
    if (p == NULL) {
      p = end;
    }

    tcp_send_data(socket, buf, p - buf);
    buf = p;

    if (p != end) {
      buf++;
      // Perform replacement
      const char *e = strchr(buf, '$');
      if (e == NULL) {
        e = end;
      } else {
        // Match is between buf and e
        if (strncmp(buf, "NET_TIME", 8) == 0) {
          tcp_send_int(socket, net_get_time());
        } else if (strncmp(buf, "TEMP", 4) == 0) {
#if 0
          tcp_send_int(socket, temperature/1364);
          tcp_send_data(socket, ".", 1);
          tcp_send_int(socket, (temperature%1364)/136);
#endif
        } else if (strncmp(buf, "REQUEST", 7) == 0) {
          //tcp_send_data(socket, requestPath, strlen(requestPath));
        }
        e++;
      }
      buf = e;
    }
  }
}

void server_callback(int socket, uint8_t new_state, uint16_t count, DATA_CB data, void *priv) {
  if( count > 0 && httpState == IDLE) {
    uint8_t buf[10];
    uint16_t s;

    // First bytes are request
    s = data(buf, 4, priv);
    if( strncmp(buf, "GET", 3) == 0 ) {
      debug_puts("GET request");
      debug_nl();
      httpState = GOT_REQUEST;

      // Get path, we only support 10 bytes of path.
      // If we don't get the separating whitespace in there, we simply ignore the rest
#if 0
      s = data(requestPath, 10, priv);
      char *sep = strchr(requestPath, ' ');
      if (sep != NULL) {
        *sep = '\0';
      } else {
        requestPath[10] = '\0';
      }
      debug_puts(requestPath);
      debug_nl();
#endif
    } else {
      debug_puts("Unknown request: ");
      debug_puts(buf);
      debug_nl();
    }
  }

  if( new_state == TCP_STATE_CLOSED ) {
    httpState = CLOSED;
  }
}

void client_callback(int socket, uint8_t new_state, uint16_t count,
    DATA_CB data, void *priv) {
  CHECK_SP("client_callback: ");
  debug_puts("Client state: ");
  debug_puthex(new_state);
  debug_nl();

  if (new_state == TCP_STATE_ESTABLISHED && count == 0) {
    sendRequest = true;
  }
  if (new_state == TCP_STATE_CLOSED) {
    P2IE |= BIT1;
  }
}

int main(void) {
  net430_init(mac_addr);

  /* Initialize RFM-module */
  rf12_initialize(3, RF12_433MHZ, 33);

  int server_sock = tcp_socket(server_callback, 500);
  int client_socket = tcp_socket(client_callback, 20);

  //temp_sensor_init();

  while (true) {
    net430_tick();

    if (httpState == CLOSED) {
      tcp_listen(server_sock, 80);
      httpState = IDLE;
    } else if( httpState == GOT_REQUEST ) {
      tcp_send_start(server_sock);
      tcp_send_template_data(server_sock, httpResponseHeader, sizeof(httpResponseHeader)-1);
      tcp_send_end(server_sock);
      tcp_close(server_sock);
    }

#if 0
    if ( net_get_time() != last_measurement ) {
      last_measurement = net_get_time();
      temp_sensor_read(INCH_10);
    } else {
      temp_sensor_read_result(&temperature);
      temperature = -40826 + 564*(temperature-600);
    }
#endif

#if 1
    if (sendRequest) {
      sendRequest = false;
      tcp_send(client_socket, httpRequest, sizeof(httpRequest) - 1);
      tcp_close(client_socket);
    }

    if (sendSignal) {
      uint8_t addr[16];
      debug_puts("Button pressed");
      debug_nl();
      notificationCounter++;
      net_get_address(ADDRESS_STORE_MAIN_OFFSET, addr);
      tcp_connect(client_socket, addr, dst, 80);
      sendSignal = false;
    }

    if (rf12_recvDone()) {
      if (rf12_crc == 0) {
        debug_puts("Got RF12 packet: ");
        debug_puthex(rf12_data[0]);
        debug_nl();
        if (rf12_data[0] == PACKET_SIGNAL) {
          batLevel = rf12_data[1] << 8 | rf12_data[2];
          debug_puts("BAT LEVEL: ");
          debug_puthex(batLevel);
          //debug_puts(rf12_data + 1);
          debug_nl();
          sendSignal = true;
        } else if( rf12_data[0] == PACKET_BAT_LEVEL) {
          batLevel = rf12_data[1] << 8 | rf12_data[2];
        }
      }
    }
#endif

    if (enc_idle ) {
      __bis_SR_register(CPUOFF | GIE);
    }
  }
  return 0;
}

void __attribute__((interrupt PORT1_VECTOR))
PORT1_ISR(void) {
  if (P1IFG & ENC_INT) {
    enc_handle_int();
    __bic_SR_register_on_exit(CPUOFF);
  }
  P1IFG = 0;
}

#if 1
void __attribute__((interrupt PORT2_VECTOR))
PORT2_ISR(void) {
  if (P2IFG & BIT5) {
    __bic_SR_register_on_exit(CPUOFF);
    rf12_interrupt();
  }
  if (P2IFG & BIT1) {
    sendSignal = true;
    P2IE &= ~BIT1;
    __bic_SR_register_on_exit(CPUOFF);
  }
  P2IFG = 0;
}
#endif
