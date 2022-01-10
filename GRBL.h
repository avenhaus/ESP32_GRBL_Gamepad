#ifndef GRBL_H
#define GRBL_H

#include "GamePad.h"

extern char grbl_commands [MAX_GRBL_CMD][64];
extern char grbl_out_buffer[128];

void grbl_init();
void grbl_run();
bool grbl_jog(char* buffer, CncData cnc_data);
bool grbl_send_cmd(char* buffer, CncData cnc_data);
void grbl_send(const char* buffer);


#endif // GRBL_H
