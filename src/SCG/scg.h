#pragma once

#ifndef _SCG_H_
#define _SCG_H_

#include<Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// top-down color bits
extern unsigned *scg_back_buffer;
extern int window_width;
extern int window_height;

// title can be set to NULL, when you don't want to specify the title
// event_process can be set to NULL, when call_back event process function is not needed, 
extern int scg_create_window(int width, int height, const TCHAR* title, WNDPROC event_process);

// close window properties
extern void scg_close_window(void);

// put draw things to foreground
extern void scg_refresh(void);

#ifdef __cplusplus
}
#endif


#endif