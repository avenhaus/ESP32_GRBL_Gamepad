#include <Arduino.h>
#include <Ps3Controller.h>

#include "Config.h"
#include "GamePad.h"

#define JOY_DEAD_ZONE 10

bool ps3_invert_x = false;
bool ps3_invert_y = true;
bool ps3_invert_z = true;
bool ps3_invert_a = false;

#if ENABLE_GAME_PAD
void ps3_on_connect() {
    DEBUG_println(FST("PS3 controller connected."));
}

void ps3_on_disconnect() {
    DEBUG_println(FST("PS3 controller disconnected."));
}

void check_game_pad(CncData& cdata) {
  cdata = {0};
  GamePadData gpd;
  static bool is_gp_connected = false;
  static bool disabled = true;
  
  if (get_game_pad_data(gpd)) {
    is_gp_connected = true;
    if (abs(gpd.rx) > JOY_DEAD_ZONE) {
      cdata.throttle_x = gpd.rx > 0 ? (gpd.rx - JOY_DEAD_ZONE) / (127.0 - JOY_DEAD_ZONE) : (gpd.rx + JOY_DEAD_ZONE) / (128.0 - JOY_DEAD_ZONE);
      if (ps3_invert_x) { cdata.throttle_x = -cdata.throttle_x; }
      // DEBUG_printf(FST("RX: %d  TX: %0.3f\n"), gpd.rx, cdata.throttle_x);
    }
    if (abs(gpd.ry) > JOY_DEAD_ZONE) {
      cdata.throttle_y = gpd.ry > 0 ? (gpd.ry - JOY_DEAD_ZONE) / (127.0 - JOY_DEAD_ZONE) : (gpd.ry + JOY_DEAD_ZONE) / (128.0 - JOY_DEAD_ZONE);
      if (ps3_invert_y) { cdata.throttle_y = -cdata.throttle_y; }
      // DEBUG_printf(FST("RY: %d  TY: %0.3f\n"), gpd.ry, cdata.throttle_y);
    }
    if (abs(gpd.ly) > JOY_DEAD_ZONE) {
      cdata.throttle_z = gpd.ly > 0 ? (gpd.ly - JOY_DEAD_ZONE) / (127.0 - JOY_DEAD_ZONE) : (gpd.ly + JOY_DEAD_ZONE) / (128.0 - JOY_DEAD_ZONE);
      if (ps3_invert_z) { cdata.throttle_z = -cdata.throttle_z; }
      // DEBUG_printf(FST("LY: %d  TZ: %0.3f\n"), gpd.ly, cdata.throttle_z);
    }

    cdata.feed_mode = gpd.l1 ? 0.01 : 0.1;
    if (gpd.l2) {
      cdata.feed_mode *= gpd.l2 / 25.5;
      DEBUG_printf(FST("L2: %d  Feed Mode: %0.3f\n"), gpd.l2, cdata.feed_mode);
    }

    size_t n = 0;
    if ( gpd.start ) { cdata.cmds[n++] = CMD_START; }
    if ( gpd.select ) { cdata.cmds[n++] = CMD_HOLD; }
    if ( gpd.contr ) { cdata.cmds[n++] = CMD_ALARM_RESET; }
    if ( gpd.r1) {
      if ( gpd.circle ) { cdata.cmds[n++] = CMD_HOME; }
      if ( gpd.square ) { cdata.cmds[n++] = CMD_HOME_X; }
      if ( gpd.triangle ) { cdata.cmds[n++] = CMD_HOME_Y; }
      if ( gpd.cross ) { cdata.cmds[n++] = CMD_HOME_Z; }
    } else {
      if ( gpd.circle ) { cdata.cmds[n++] = CMD_ZERO_ALL; }
      if ( gpd.square ) { cdata.cmds[n++] = CMD_ZERO_X; }
      if ( gpd.triangle ) { cdata.cmds[n++] = CMD_ZERO_Y; }
      if ( gpd.cross ) { cdata.cmds[n++] = CMD_ZERO_Z; }      
    }
    cdata.cmds[n] = CMD_END_OF_LIST;
    
  } else if (is_gp_connected) {
    is_gp_connected = false;
  }

}

bool get_game_pad_data(GamePadData& data) {

    if (!Ps3.isConnected()) { return false; }

    data.lx = Ps3.data.analog.stick.lx;
    data.ly = Ps3.data.analog.stick.ly;
    data.rx = Ps3.data.analog.stick.rx;
    data.ry = Ps3.data.analog.stick.ry;

    data.select = Ps3.data.button.select;
    data.l3 = Ps3.data.button.l3;
    data.r3 = Ps3.data.button.r3;
    data.start = Ps3.data.button.start;

    data.up = Ps3.data.button.up;
    data.right = Ps3.data.button.right;
    data.down = Ps3.data.button.down;
    data.left = Ps3.data.button.left;

    data.l2 = Ps3.data.analog.button.l2;
    data.r2 = Ps3.data.analog.button.r2;
    data.l1 = Ps3.data.button.l1;
    data.r1 = Ps3.data.button.r1;

    data.triangle = Ps3.data.button.triangle;
    data.circle = Ps3.data.button.circle;
    data.cross = Ps3.data.button.cross;
    data.square = Ps3.data.button.square;

    data.contr = Ps3.data.button.ps;
    return true;
}

void game_pad_init() {
    Ps3.attachOnConnect(ps3_on_connect);
    Ps3.attachOnDisconnect(ps3_on_disconnect);

    Ps3.begin("00:12:34:56:78:9B");
    //Ps3.begin();

    String address = Ps3.getAddress();
    DEBUG_print(FST("Bluetooth MAC: "));    
    DEBUG_println(address);

    DEBUG_println(FST("Ready for PS3 controller."));    
}
#endif //ENABLE_GAME_PAD
