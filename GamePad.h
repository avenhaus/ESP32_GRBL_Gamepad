#ifndef GAME_PAD_H
#define GAME_PAD_H

#include "Config.h"

typedef enum grbl_command {
  CMD_END_OF_LIST,
  CMD_HOME,
  CMD_HOME_X,
  CMD_HOME_Y,
  CMD_HOME_Z,
  CMD_HOME_A,
  CMD_ZERO_ALL,
  CMD_ZERO_XY,
  CMD_ZERO_X,
  CMD_ZERO_Y,
  CMD_ZERO_Z,
  CMD_ZERO_A,
  CMD_RESET,
  CMD_START,
  CMD_HOLD,
  CMD_ALARM_RESET,
  CMD_SPINDLE,
  CMD_FLOOD,
  CMD_MIST,
  CMD_NOP_1,
  CMD_NOP_2,
  CMD_NOP_3,
  CMD_NOP_4,
  CMD_NOP_5,
  CMD_1,
  CMD_2,
  CMD_3,
  CMD_4,
  CMD_5,
  CMD_6,
  CMD_7,
  CMD_8,
  CMD_9
} grbl_command_t;


typedef struct CncData {
  float throttle_x;
  float throttle_y;
  float throttle_z;
  float throttle_a;
  float feed_mode;
  
  grbl_command_t cmds[32];
} CncData;


#if ENABLE_GAME_PAD

typedef struct GamePadData {
    int8_t lx;
    int8_t ly;
    int8_t rx;
    int8_t ry;

    bool select;
    bool l3;
    bool r3;
    bool start;

    bool up;
    bool right;
    bool down;
    bool left;

    uint8_t l2;
    uint8_t r2;
    bool l1;
    bool r1;

    bool triangle;
    bool circle;
    bool cross;
    bool square;

    bool contr;
} GamePadData;

void game_pad_init(void);
bool get_game_pad_data(GamePadData& data);
void check_game_pad(CncData& cdata);

#endif // ENABLE_GAME_PAD

#endif // GAME_PAD_H
