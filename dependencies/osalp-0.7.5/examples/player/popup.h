#ifndef _POPUP_H_
#define _POPUP_H_

extern Widget
popup_build(
   Widget  parent,
   int     menu_type);

void
popup_device_dialog(
   Widget w);

#define ENV_FILE       "aflibplayer"
#define PORT_KEY       "PORT_KEY:"

#endif
