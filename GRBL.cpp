#include <Arduino.h>

#include "Config.h"
#include "GRBL.h"

Stream* grblStream = nullptr;

char grbl_out_buffer[128];
char grbl_in_buffer[128];
size_t grbl_in_data = 0;

int grbl_init_state = 0;
float is_jogging = false;
bool cmd_sent = false;

CncData cnc_data;
uint32_t next_ts = 0;
uint32_t config_retry_ts = 0;

/*==========================================================*\
 * GRBL commands
\*==========================================================*/
// https://www.cnc4fun.com/wp-content/uploads/2019/12/Grbl-Commands-v1.1-2.pdf
char grbl_commands [MAX_GRBL_CMD][64] = {
  "$H",                   // Home all
  "$HX",
  "$HY",
  "$HZ",
  "$HA",
  "G10 L20 P0 X0 Y0 Z0",  // Zero all
  "G10 L20 P0 X0 Y0",     // Zero XY
  "G10 L20 P0 X0",
  "G10 L20 P0 Y0",
  "G10 L20 P0 Z0",
  "G10 L20 P0 A0",
  {0x18, 0},    // Reset
  "~",          // Start / Resume
  "!",          // Feed hold
  "$X",         // Reset Alarm Lock
  {0x9E, 0},    // Spindle
  {0xA0, 0},    // Flood Coolant
  {0xA1, 0},    // Mist Coolant
};


/*==========================================================*\
 * GRBL configuration
\*==========================================================*/
float max_feed_x = 2000.0;
float max_feed_y = 2000.0;
float max_feed_z = 500.0;
float max_feed_a = 500.0;
float jog_adjust = 0.92;    // Adjust calculate jog distance values to match reality
float jog_dt = 0.1;         // Time interval between jog commands


/*==========================================================*\
 * Initialize GRBl controller interface
\*==========================================================*/
void grbl_init() {
  #if USE_GRBL_SERIAL
  Serial1.begin(GRBL_SERIAL_SPEED, SERIAL_8N1, UART1_RXD_PIN, UART1_TXD_PIN);
  grblStream = &Serial1;
  #endif
  next_ts = millis() + jog_dt * 1000;
}

/*==========================================================*\
 * Handle incomeing line from GRBL controller
\*==========================================================*/

void grbl_handle_line(const char* line) {
  // DEBUG_printf(FST("GRBL-in: %s\n"), line);
  size_t n = 0;
  if (line[0] == '$') {
    int par = atoi(line+1);
    if (par) {
      n = 1;
      while (line[n] && line[n] != '=') { n++; }
      if (line[n] == '=') { n++; }
      float val = atof(line+n);
      DEBUG_printf(FST("GRBL-parameter: %d : %f\n"), par, val);
      grbl_init_state = 2;
      if (par == 110) {
        max_feed_x = val;
      } else if (par == 111) {
        max_feed_y = val;
      } else if (par == 112) {
        max_feed_z = val;
      } else if (par == 113) {
        max_feed_a = val;
      }
    }
  }
}


/*==========================================================*\
 * Read from GRBL controller and handle any input
\*==========================================================*/
void grbl_handle_input() {
  while (grblStream->available() > 0) {
    char c = grblStream->read();
    if (c == '\n' || c == '\r' || c == '\0') {
      grbl_in_buffer[grbl_in_data] = '\0';
      if (grbl_in_data) { grbl_handle_line(grbl_in_buffer); }
      grbl_in_data = 0;
      continue;
    }
    if (grbl_in_data < sizeof(grbl_in_buffer)-1) {
      grbl_in_buffer[grbl_in_data++] = c;
    }
  }
}

/*==========================================================*\
 * Read from GRBL controller and handle any input
\*==========================================================*/
void grbl_run() {
  if (!grblStream) { return; }

  if (grbl_init_state == 0 || (grbl_init_state == 1 && millis() > config_retry_ts)) {
    next_ts = millis() + jog_dt * 1000;
    grbl_send(FST("$$\n")); // Read config like max feed rates 
    grbl_init_state = 1;
    config_retry_ts = millis() + 2000; // Retry every 2 sec
  }

  grbl_handle_input();

  if (millis() >= next_ts) {
    check_game_pad(cnc_data);
    grbl_jog(grbl_out_buffer, cnc_data);
    grbl_send_cmd(grbl_out_buffer, cnc_data);
    next_ts += jog_dt * 1000;
  }
}


/*==========================================================*\
 * Send a command (from button press) to GRBL controller
\*==========================================================*/
bool grbl_send_cmd(char* buffer, CncData cnc_data) {
  size_t n = 0;
  if (!cmd_sent) {
    while (cnc_data.cmds[n]) {
      size_t cmd = ((size_t) cnc_data.cmds[n]) -1;
      grbl_send(grbl_commands[cmd]);
      grbl_send(FST("\n"));
      n++;
      cmd_sent = true;
      break;
    }
  } else if (!cnc_data.cmds[0]) {
      cmd_sent = false;    
  }
  
}

/*==========================================================*\
 * Send jog commands to GRBL controller
\*==========================================================*/
bool grbl_jog(char* buffer, CncData cnc_data) {
  float feed = 0.0;
  buffer[0] = 0;

  if (cnc_data.throttle_x || cnc_data.throttle_y || cnc_data.throttle_z || cnc_data.throttle_a) {

    float feed_x = abs(cnc_data.throttle_x) * max_feed_x * cnc_data.feed_mode;
    float dx = feed_x * jog_dt / 60.0 * jog_adjust;
    if (cnc_data.throttle_x < 0) { dx = -dx; }
    
    float feed_y = abs(cnc_data.throttle_y) * max_feed_y * cnc_data.feed_mode;
    float dy = feed_y * jog_dt / 60.0 * jog_adjust;
    if (cnc_data.throttle_y < 0) { dy = -dy; }

    float feed_z = abs(cnc_data.throttle_z) * max_feed_z * cnc_data.feed_mode;
    float dz = feed_z * jog_dt / 60.0 * jog_adjust;
    if (cnc_data.throttle_z < 0) { dz = -dz; }

    float feed_a = abs(cnc_data.throttle_a) * max_feed_a * cnc_data.feed_mode;
    float da = feed_a * jog_dt / 60.0 * jog_adjust;
    if (cnc_data.throttle_a < 0) { da = -da; }

    feed = feed_x;
    if (feed_y) { feed = sqrt((feed * feed) + (feed_y * feed_y)); }
    if (feed_z) { feed = sqrt((feed * feed) + (feed_z * feed_z)); }
    if (feed_a) { feed = sqrt((feed * feed) + (feed_a * feed_a)); }
    
    size_t n = 0;
    if (feed > 1000.0) { n = sprintf(buffer, FST("$J=G91 G21 F%d"), (int) round(feed)); }
    else { n = sprintf(buffer, FST("$J=G91 G21 F%0.4g"), feed); }    
    if (dx) { n += sprintf(buffer+n, FST(" X%0.4g"), dx); }
    if (dy) { n += sprintf(buffer+n, FST(" Y%0.4g"), dy); }
    if (dz) { n += sprintf(buffer+n, FST(" Z%0.4g"), dz); }
    if (da) { n += sprintf(buffer+n, FST(" A%0.4g"), da); }
    buffer[n++] = '\n';
    buffer[n] = '\0';
  }
  if (feed) {
    grbl_send(buffer);
    is_jogging = true;
  } else if (is_jogging) {
    buffer[0] = 0x85;  // Abort Jog command
    buffer[1] = '\n';
    buffer[2] = 0;
    grbl_send(buffer);
    is_jogging = false;
  }
  return feed;
}


/*==========================================================*\
 * Write to GRBL
\*==========================================================*/
void grbl_send(const char* buffer) {
  if (grblStream) { grblStream->print(buffer); }
  DEBUG_print(buffer);
}
