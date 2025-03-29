#ifdef PLATFORM_LINUX

#include <iostream>
#include <filesystem>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "LinuxEvdev.hpp"

const char* LinuxEvdev::KeyCodeNameMap[KEY_CNT] = { nullptr };

void LinuxEvdev::InitializeKeyCodeNameMap() {
    this->KeyCodeNameMap[KEY_RESERVED] = "KEY_RESERVED";
    this->KeyCodeNameMap[KEY_ESC] = "KEY_ESC";
    this->KeyCodeNameMap[KEY_1] = "KEY_1";
    this->KeyCodeNameMap[KEY_2] = "KEY_2";
    this->KeyCodeNameMap[KEY_3] = "KEY_3";
    this->KeyCodeNameMap[KEY_4] = "KEY_4";
    this->KeyCodeNameMap[KEY_5] = "KEY_5";
    this->KeyCodeNameMap[KEY_6] = "KEY_6";
    this->KeyCodeNameMap[KEY_7] = "KEY_7";
    this->KeyCodeNameMap[KEY_8] = "KEY_8";
    this->KeyCodeNameMap[KEY_9] = "KEY_9";
    this->KeyCodeNameMap[KEY_0] = "KEY_0";
    this->KeyCodeNameMap[KEY_MINUS] = "KEY_MINUS";
    this->KeyCodeNameMap[KEY_EQUAL] = "KEY_EQUAL";
    this->KeyCodeNameMap[KEY_BACKSPACE] = "KEY_BACKSPACE";
    this->KeyCodeNameMap[KEY_TAB] = "KEY_TAB";
    this->KeyCodeNameMap[KEY_Q] = "KEY_Q";
    this->KeyCodeNameMap[KEY_W] = "KEY_W";
    this->KeyCodeNameMap[KEY_E] = "KEY_E";
    this->KeyCodeNameMap[KEY_R] = "KEY_R";
    this->KeyCodeNameMap[KEY_T] = "KEY_T";
    this->KeyCodeNameMap[KEY_Y] = "KEY_Y";
    this->KeyCodeNameMap[KEY_U] = "KEY_U";
    this->KeyCodeNameMap[KEY_I] = "KEY_I";
    this->KeyCodeNameMap[KEY_O] = "KEY_O";
    this->KeyCodeNameMap[KEY_P] = "KEY_P";
    this->KeyCodeNameMap[KEY_LEFTBRACE] = "KEY_LEFTBRACE";
    this->KeyCodeNameMap[KEY_RIGHTBRACE] = "KEY_RIGHTBRACE";
    this->KeyCodeNameMap[KEY_ENTER] = "KEY_ENTER";
    this->KeyCodeNameMap[KEY_LEFTCTRL] = "KEY_LEFTCTRL";
    this->KeyCodeNameMap[KEY_A] = "KEY_A";
    this->KeyCodeNameMap[KEY_S] = "KEY_S";
    this->KeyCodeNameMap[KEY_D] = "KEY_D";
    this->KeyCodeNameMap[KEY_F] = "KEY_F";
    this->KeyCodeNameMap[KEY_G] = "KEY_G";
    this->KeyCodeNameMap[KEY_H] = "KEY_H";
    this->KeyCodeNameMap[KEY_J] = "KEY_J";
    this->KeyCodeNameMap[KEY_K] = "KEY_K";
    this->KeyCodeNameMap[KEY_L] = "KEY_L";
    this->KeyCodeNameMap[KEY_SEMICOLON] = "KEY_SEMICOLON";
    this->KeyCodeNameMap[KEY_APOSTROPHE] = "KEY_APOSTROPHE";
    this->KeyCodeNameMap[KEY_GRAVE] = "KEY_GRAVE";
    this->KeyCodeNameMap[KEY_LEFTSHIFT] = "KEY_LEFTSHIFT";
    this->KeyCodeNameMap[KEY_BACKSLASH] = "KEY_BACKSLASH";
    this->KeyCodeNameMap[KEY_Z] = "KEY_Z";
    this->KeyCodeNameMap[KEY_X] = "KEY_X";
    this->KeyCodeNameMap[KEY_C] = "KEY_C";
    this->KeyCodeNameMap[KEY_V] = "KEY_V";
    this->KeyCodeNameMap[KEY_B] = "KEY_B";
    this->KeyCodeNameMap[KEY_N] = "KEY_N";
    this->KeyCodeNameMap[KEY_M] = "KEY_M";
    this->KeyCodeNameMap[KEY_COMMA] = "KEY_COMMA";
    this->KeyCodeNameMap[KEY_DOT] = "KEY_DOT";
    this->KeyCodeNameMap[KEY_SLASH] = "KEY_SLASH";
    this->KeyCodeNameMap[KEY_RIGHTSHIFT] = "KEY_RIGHTSHIFT";
    this->KeyCodeNameMap[KEY_KPASTERISK] = "KEY_KPASTERISK";
    this->KeyCodeNameMap[KEY_LEFTALT] = "KEY_LEFTALT";
    this->KeyCodeNameMap[KEY_SPACE] = "KEY_SPACE";
    this->KeyCodeNameMap[KEY_CAPSLOCK] = "KEY_CAPSLOCK";
    this->KeyCodeNameMap[KEY_F1] = "KEY_F1";
    this->KeyCodeNameMap[KEY_F2] = "KEY_F2";
    this->KeyCodeNameMap[KEY_F3] = "KEY_F3";
    this->KeyCodeNameMap[KEY_F4] = "KEY_F4";
    this->KeyCodeNameMap[KEY_F5] = "KEY_F5";
    this->KeyCodeNameMap[KEY_F6] = "KEY_F6";
    this->KeyCodeNameMap[KEY_F7] = "KEY_F7";
    this->KeyCodeNameMap[KEY_F8] = "KEY_F8";
    this->KeyCodeNameMap[KEY_F9] = "KEY_F9";
    this->KeyCodeNameMap[KEY_F10] = "KEY_F10";
    this->KeyCodeNameMap[KEY_NUMLOCK] = "KEY_NUMLOCK";
    this->KeyCodeNameMap[KEY_SCROLLLOCK] = "KEY_SCROLLLOCK";
    this->KeyCodeNameMap[KEY_KP7] = "KEY_KP7";
    this->KeyCodeNameMap[KEY_KP8] = "KEY_KP8";
    this->KeyCodeNameMap[KEY_KP9] = "KEY_KP9";
    this->KeyCodeNameMap[KEY_KPMINUS] = "KEY_KPMINUS";
    this->KeyCodeNameMap[KEY_KP4] = "KEY_KP4";
    this->KeyCodeNameMap[KEY_KP5] = "KEY_KP5";
    this->KeyCodeNameMap[KEY_KP6] = "KEY_KP6";
    this->KeyCodeNameMap[KEY_KPPLUS] = "KEY_KPPLUS";
    this->KeyCodeNameMap[KEY_KP1] = "KEY_KP1";
    this->KeyCodeNameMap[KEY_KP2] = "KEY_KP2";
    this->KeyCodeNameMap[KEY_KP3] = "KEY_KP3";
    this->KeyCodeNameMap[KEY_KP0] = "KEY_KP0";
    this->KeyCodeNameMap[KEY_KPDOT] = "KEY_KPDOT";
    this->KeyCodeNameMap[KEY_ZENKAKUHANKAKU] = "KEY_ZENKAKUHANKAKU";
    this->KeyCodeNameMap[KEY_102ND] = "KEY_102ND";
    this->KeyCodeNameMap[KEY_F11] = "KEY_F11";
    this->KeyCodeNameMap[KEY_F12] = "KEY_F12";
    this->KeyCodeNameMap[KEY_RO] = "KEY_RO";
    this->KeyCodeNameMap[KEY_KATAKANA] = "KEY_KATAKANA";
    this->KeyCodeNameMap[KEY_HIRAGANA] = "KEY_HIRAGANA";
    this->KeyCodeNameMap[KEY_HENKAN] = "KEY_HENKAN";
    this->KeyCodeNameMap[KEY_KATAKANAHIRAGANA] = "KEY_KATAKANAHIRAGANA";
    this->KeyCodeNameMap[KEY_MUHENKAN] = "KEY_MUHENKAN";
    this->KeyCodeNameMap[KEY_KPJPCOMMA] = "KEY_KPJPCOMMA";
    this->KeyCodeNameMap[KEY_KPENTER] = "KEY_KPENTER";
    this->KeyCodeNameMap[KEY_RIGHTCTRL] = "KEY_RIGHTCTRL";
    this->KeyCodeNameMap[KEY_KPSLASH] = "KEY_KPSLASH";
    this->KeyCodeNameMap[KEY_SYSRQ] = "KEY_SYSRQ";
    this->KeyCodeNameMap[KEY_RIGHTALT] = "KEY_RIGHTALT";
    this->KeyCodeNameMap[KEY_LINEFEED] = "KEY_LINEFEED";
    this->KeyCodeNameMap[KEY_HOME] = "KEY_HOME";
    this->KeyCodeNameMap[KEY_UP] = "KEY_UP";
    this->KeyCodeNameMap[KEY_PAGEUP] = "KEY_PAGEUP";
    this->KeyCodeNameMap[KEY_LEFT] = "KEY_LEFT";
    this->KeyCodeNameMap[KEY_RIGHT] = "KEY_RIGHT";
    this->KeyCodeNameMap[KEY_END] = "KEY_END";
    this->KeyCodeNameMap[KEY_DOWN] = "KEY_DOWN";
    this->KeyCodeNameMap[KEY_PAGEDOWN] = "KEY_PAGEDOWN";
    this->KeyCodeNameMap[KEY_INSERT] = "KEY_INSERT";
    this->KeyCodeNameMap[KEY_DELETE] = "KEY_DELETE";
    this->KeyCodeNameMap[KEY_MACRO] = "KEY_MACRO";
    this->KeyCodeNameMap[KEY_MUTE] = "KEY_MUTE";
    this->KeyCodeNameMap[KEY_VOLUMEDOWN] = "KEY_VOLUMEDOWN";
    this->KeyCodeNameMap[KEY_VOLUMEUP] = "KEY_VOLUMEUP";
    this->KeyCodeNameMap[KEY_POWER] = "KEY_POWER";
    this->KeyCodeNameMap[KEY_KPEQUAL] = "KEY_KPEQUAL";
    this->KeyCodeNameMap[KEY_KPPLUSMINUS] = "KEY_KPPLUSMINUS";
    this->KeyCodeNameMap[KEY_PAUSE] = "KEY_PAUSE";
    this->KeyCodeNameMap[KEY_SCALE] = "KEY_SCALE";
    this->KeyCodeNameMap[KEY_KPCOMMA] = "KEY_KPCOMMA";
    this->KeyCodeNameMap[KEY_HANGEUL] = "KEY_HANGEUL";
    this->KeyCodeNameMap[KEY_HANJA] = "KEY_HANJA";
    this->KeyCodeNameMap[KEY_YEN] = "KEY_YEN";
    this->KeyCodeNameMap[KEY_LEFTMETA] = "KEY_LEFTMETA";
    this->KeyCodeNameMap[KEY_RIGHTMETA] = "KEY_RIGHTMETA";
    this->KeyCodeNameMap[KEY_COMPOSE] = "KEY_COMPOSE";
    this->KeyCodeNameMap[KEY_STOP] = "KEY_STOP";
    this->KeyCodeNameMap[KEY_AGAIN] = "KEY_AGAIN";
    this->KeyCodeNameMap[KEY_PROPS] = "KEY_PROPS";
    this->KeyCodeNameMap[KEY_UNDO] = "KEY_UNDO";
    this->KeyCodeNameMap[KEY_FRONT] = "KEY_FRONT";
    this->KeyCodeNameMap[KEY_COPY] = "KEY_COPY";
    this->KeyCodeNameMap[KEY_OPEN] = "KEY_OPEN";
    this->KeyCodeNameMap[KEY_PASTE] = "KEY_PASTE";
    this->KeyCodeNameMap[KEY_FIND] = "KEY_FIND";
    this->KeyCodeNameMap[KEY_CUT] = "KEY_CUT";
    this->KeyCodeNameMap[KEY_HELP] = "KEY_HELP";
    this->KeyCodeNameMap[KEY_MENU] = "KEY_MENU";
    this->KeyCodeNameMap[KEY_CALC] = "KEY_CALC";
    this->KeyCodeNameMap[KEY_SETUP] = "KEY_SETUP";
    this->KeyCodeNameMap[KEY_SLEEP] = "KEY_SLEEP";
    this->KeyCodeNameMap[KEY_WAKEUP] = "KEY_WAKEUP";
    this->KeyCodeNameMap[KEY_FILE] = "KEY_FILE";
    this->KeyCodeNameMap[KEY_SENDFILE] = "KEY_SENDFILE";
    this->KeyCodeNameMap[KEY_DELETEFILE] = "KEY_DELETEFILE";
    this->KeyCodeNameMap[KEY_XFER] = "KEY_XFER";
    this->KeyCodeNameMap[KEY_PROG1] = "KEY_PROG1";
    this->KeyCodeNameMap[KEY_PROG2] = "KEY_PROG2";
    this->KeyCodeNameMap[KEY_WWW] = "KEY_WWW";
    this->KeyCodeNameMap[KEY_MSDOS] = "KEY_MSDOS";
    this->KeyCodeNameMap[KEY_COFFEE] = "KEY_COFFEE";
    this->KeyCodeNameMap[KEY_ROTATE_DISPLAY] = "KEY_ROTATE_DISPLAY";
    this->KeyCodeNameMap[KEY_CYCLEWINDOWS] = "KEY_CYCLEWINDOWS";
    this->KeyCodeNameMap[KEY_MAIL] = "KEY_MAIL";
    this->KeyCodeNameMap[KEY_BOOKMARKS] = "KEY_BOOKMARKS";
    this->KeyCodeNameMap[KEY_COMPUTER] = "KEY_COMPUTER";
    this->KeyCodeNameMap[KEY_BACK] = "KEY_BACK";
    this->KeyCodeNameMap[KEY_FORWARD] = "KEY_FORWARD";
    this->KeyCodeNameMap[KEY_CLOSECD] = "KEY_CLOSECD";
    this->KeyCodeNameMap[KEY_EJECTCD] = "KEY_EJECTCD";
    this->KeyCodeNameMap[KEY_EJECTCLOSECD] = "KEY_EJECTCLOSECD";
    this->KeyCodeNameMap[KEY_NEXTSONG] = "KEY_NEXTSONG";
    this->KeyCodeNameMap[KEY_PLAYPAUSE] = "KEY_PLAYPAUSE";
    this->KeyCodeNameMap[KEY_PREVIOUSSONG] = "KEY_PREVIOUSSONG";
    this->KeyCodeNameMap[KEY_STOPCD] = "KEY_STOPCD";
    this->KeyCodeNameMap[KEY_RECORD] = "KEY_RECORD";
    this->KeyCodeNameMap[KEY_REWIND] = "KEY_REWIND";
    this->KeyCodeNameMap[KEY_PHONE] = "KEY_PHONE";
    this->KeyCodeNameMap[KEY_ISO] = "KEY_ISO";
    this->KeyCodeNameMap[KEY_CONFIG] = "KEY_CONFIG";
    this->KeyCodeNameMap[KEY_HOMEPAGE] = "KEY_HOMEPAGE";
    this->KeyCodeNameMap[KEY_REFRESH] = "KEY_REFRESH";
    this->KeyCodeNameMap[KEY_EXIT] = "KEY_EXIT";
    this->KeyCodeNameMap[KEY_MOVE] = "KEY_MOVE";
    this->KeyCodeNameMap[KEY_EDIT] = "KEY_EDIT";
    this->KeyCodeNameMap[KEY_SCROLLUP] = "KEY_SCROLLUP";
    this->KeyCodeNameMap[KEY_SCROLLDOWN] = "KEY_SCROLLDOWN";
    this->KeyCodeNameMap[KEY_KPLEFTPAREN] = "KEY_KPLEFTPAREN";
    this->KeyCodeNameMap[KEY_KPRIGHTPAREN] = "KEY_KPRIGHTPAREN";
    this->KeyCodeNameMap[KEY_NEW] = "KEY_NEW";
    this->KeyCodeNameMap[KEY_REDO] = "KEY_REDO";
    this->KeyCodeNameMap[KEY_F13] = "KEY_F13";
    this->KeyCodeNameMap[KEY_F14] = "KEY_F14";
    this->KeyCodeNameMap[KEY_F15] = "KEY_F15";
    this->KeyCodeNameMap[KEY_F16] = "KEY_F16";
    this->KeyCodeNameMap[KEY_F17] = "KEY_F17";
    this->KeyCodeNameMap[KEY_F18] = "KEY_F18";
    this->KeyCodeNameMap[KEY_F19] = "KEY_F19";
    this->KeyCodeNameMap[KEY_F20] = "KEY_F20";
    this->KeyCodeNameMap[KEY_F21] = "KEY_F21";
    this->KeyCodeNameMap[KEY_F22] = "KEY_F22";
    this->KeyCodeNameMap[KEY_F23] = "KEY_F23";
    this->KeyCodeNameMap[KEY_F24] = "KEY_F24";
    this->KeyCodeNameMap[KEY_PLAYCD] = "KEY_PLAYCD";
    this->KeyCodeNameMap[KEY_PAUSECD] = "KEY_PAUSECD";
    this->KeyCodeNameMap[KEY_PROG3] = "KEY_PROG3";
    this->KeyCodeNameMap[KEY_PROG4] = "KEY_PROG4";
    this->KeyCodeNameMap[KEY_ALL_APPLICATIONS] = "KEY_ALL_APPLICATIONS";
    this->KeyCodeNameMap[KEY_SUSPEND] = "KEY_SUSPEND";
    this->KeyCodeNameMap[KEY_CLOSE] = "KEY_CLOSE";
    this->KeyCodeNameMap[KEY_PLAY] = "KEY_PLAY";
    this->KeyCodeNameMap[KEY_FASTFORWARD] = "KEY_FASTFORWARD";
    this->KeyCodeNameMap[KEY_BASSBOOST] = "KEY_BASSBOOST";
    this->KeyCodeNameMap[KEY_PRINT] = "KEY_PRINT";
    this->KeyCodeNameMap[KEY_HP] = "KEY_HP";
    this->KeyCodeNameMap[KEY_CAMERA] = "KEY_CAMERA";
    this->KeyCodeNameMap[KEY_SOUND] = "KEY_SOUND";
    this->KeyCodeNameMap[KEY_QUESTION] = "KEY_QUESTION";
    this->KeyCodeNameMap[KEY_EMAIL] = "KEY_EMAIL";
    this->KeyCodeNameMap[KEY_CHAT] = "KEY_CHAT";
    this->KeyCodeNameMap[KEY_SEARCH] = "KEY_SEARCH";
    this->KeyCodeNameMap[KEY_CONNECT] = "KEY_CONNECT";
    this->KeyCodeNameMap[KEY_FINANCE] = "KEY_FINANCE";
    this->KeyCodeNameMap[KEY_SPORT] = "KEY_SPORT";
    this->KeyCodeNameMap[KEY_SHOP] = "KEY_SHOP";
    this->KeyCodeNameMap[KEY_ALTERASE] = "KEY_ALTERASE";
    this->KeyCodeNameMap[KEY_CANCEL] = "KEY_CANCEL";
    this->KeyCodeNameMap[KEY_BRIGHTNESSDOWN] = "KEY_BRIGHTNESSDOWN";
    this->KeyCodeNameMap[KEY_BRIGHTNESSUP] = "KEY_BRIGHTNESSUP";
    this->KeyCodeNameMap[KEY_MEDIA] = "KEY_MEDIA";
    this->KeyCodeNameMap[KEY_SWITCHVIDEOMODE] = "KEY_SWITCHVIDEOMODE";
    this->KeyCodeNameMap[KEY_KBDILLUMTOGGLE] = "KEY_KBDILLUMTOGGLE";
    this->KeyCodeNameMap[KEY_KBDILLUMDOWN] = "KEY_KBDILLUMDOWN";
    this->KeyCodeNameMap[KEY_KBDILLUMUP] = "KEY_KBDILLUMUP";
    this->KeyCodeNameMap[KEY_SEND] = "KEY_SEND";
    this->KeyCodeNameMap[KEY_REPLY] = "KEY_REPLY";
    this->KeyCodeNameMap[KEY_FORWARDMAIL] = "KEY_FORWARDMAIL";
    this->KeyCodeNameMap[KEY_SAVE] = "KEY_SAVE";
    this->KeyCodeNameMap[KEY_DOCUMENTS] = "KEY_DOCUMENTS";
    this->KeyCodeNameMap[KEY_BATTERY] = "KEY_BATTERY";
    this->KeyCodeNameMap[KEY_BLUETOOTH] = "KEY_BLUETOOTH";
    this->KeyCodeNameMap[KEY_WLAN] = "KEY_WLAN";
    this->KeyCodeNameMap[KEY_UWB] = "KEY_UWB";
    this->KeyCodeNameMap[KEY_UNKNOWN] = "KEY_UNKNOWN";
    this->KeyCodeNameMap[KEY_VIDEO_NEXT] = "KEY_VIDEO_NEXT";
    this->KeyCodeNameMap[KEY_VIDEO_PREV] = "KEY_VIDEO_PREV";
    this->KeyCodeNameMap[KEY_BRIGHTNESS_CYCLE] = "KEY_BRIGHTNESS_CYCLE";
    this->KeyCodeNameMap[KEY_BRIGHTNESS_AUTO] = "KEY_BRIGHTNESS_AUTO";
    this->KeyCodeNameMap[KEY_DISPLAY_OFF] = "KEY_DISPLAY_OFF";
    this->KeyCodeNameMap[KEY_WWAN] = "KEY_WWAN";
    this->KeyCodeNameMap[KEY_RFKILL] = "KEY_RFKILL";
    this->KeyCodeNameMap[KEY_MICMUTE] = "KEY_MICMUTE";
    this->KeyCodeNameMap[BTN_MISC] = "BTN_MISC";
    this->KeyCodeNameMap[BTN_1] = "BTN_1";
    this->KeyCodeNameMap[BTN_2] = "BTN_2";
    this->KeyCodeNameMap[BTN_3] = "BTN_3";
    this->KeyCodeNameMap[BTN_4] = "BTN_4";
    this->KeyCodeNameMap[BTN_5] = "BTN_5";
    this->KeyCodeNameMap[BTN_6] = "BTN_6";
    this->KeyCodeNameMap[BTN_7] = "BTN_7";
    this->KeyCodeNameMap[BTN_8] = "BTN_8";
    this->KeyCodeNameMap[BTN_9] = "BTN_9";
    this->KeyCodeNameMap[BTN_MOUSE] = "BTN_MOUSE";
    this->KeyCodeNameMap[BTN_RIGHT] = "BTN_RIGHT";
    this->KeyCodeNameMap[BTN_MIDDLE] = "BTN_MIDDLE";
    this->KeyCodeNameMap[BTN_SIDE] = "BTN_SIDE";
    this->KeyCodeNameMap[BTN_EXTRA] = "BTN_EXTRA";
    this->KeyCodeNameMap[BTN_FORWARD] = "BTN_FORWARD";
    this->KeyCodeNameMap[BTN_BACK] = "BTN_BACK";
    this->KeyCodeNameMap[BTN_TASK] = "BTN_TASK";
    this->KeyCodeNameMap[BTN_JOYSTICK] = "BTN_JOYSTICK";
    this->KeyCodeNameMap[BTN_THUMB] = "BTN_THUMB";
    this->KeyCodeNameMap[BTN_THUMB2] = "BTN_THUMB2";
    this->KeyCodeNameMap[BTN_TOP] = "BTN_TOP";
    this->KeyCodeNameMap[BTN_TOP2] = "BTN_TOP2";
    this->KeyCodeNameMap[BTN_PINKIE] = "BTN_PINKIE";
    this->KeyCodeNameMap[BTN_BASE] = "BTN_BASE";
    this->KeyCodeNameMap[BTN_BASE2] = "BTN_BASE2";
    this->KeyCodeNameMap[BTN_BASE3] = "BTN_BASE3";
    this->KeyCodeNameMap[BTN_BASE4] = "BTN_BASE4";
    this->KeyCodeNameMap[BTN_BASE5] = "BTN_BASE5";
    this->KeyCodeNameMap[BTN_BASE6] = "BTN_BASE6";
    this->KeyCodeNameMap[BTN_DEAD] = "BTN_DEAD";
    this->KeyCodeNameMap[BTN_GAMEPAD] = "BTN_GAMEPAD";
    this->KeyCodeNameMap[BTN_EAST] = "BTN_EAST";
    this->KeyCodeNameMap[BTN_C] = "BTN_C";
    this->KeyCodeNameMap[BTN_NORTH] = "BTN_NORTH";
    this->KeyCodeNameMap[BTN_WEST] = "BTN_WEST";
    this->KeyCodeNameMap[BTN_Z] = "BTN_Z";
    this->KeyCodeNameMap[BTN_TL] = "BTN_TL";
    this->KeyCodeNameMap[BTN_TR] = "BTN_TR";
    this->KeyCodeNameMap[BTN_TL2] = "BTN_TL2";
    this->KeyCodeNameMap[BTN_TR2] = "BTN_TR2";
    this->KeyCodeNameMap[BTN_SELECT] = "BTN_SELECT";
    this->KeyCodeNameMap[BTN_START] = "BTN_START";
    this->KeyCodeNameMap[BTN_MODE] = "BTN_MODE";
    this->KeyCodeNameMap[BTN_THUMBL] = "BTN_THUMBL";
    this->KeyCodeNameMap[BTN_THUMBR] = "BTN_THUMBR";
    this->KeyCodeNameMap[BTN_DIGI] = "BTN_DIGI";
    this->KeyCodeNameMap[BTN_TOOL_RUBBER] = "BTN_TOOL_RUBBER";
    this->KeyCodeNameMap[BTN_TOOL_BRUSH] = "BTN_TOOL_BRUSH";
    this->KeyCodeNameMap[BTN_TOOL_PENCIL] = "BTN_TOOL_PENCIL";
    this->KeyCodeNameMap[BTN_TOOL_AIRBRUSH] = "BTN_TOOL_AIRBRUSH";
    this->KeyCodeNameMap[BTN_TOOL_FINGER] = "BTN_TOOL_FINGER";
    this->KeyCodeNameMap[BTN_TOOL_MOUSE] = "BTN_TOOL_MOUSE";
    this->KeyCodeNameMap[BTN_TOOL_LENS] = "BTN_TOOL_LENS";
    this->KeyCodeNameMap[BTN_TOOL_QUINTTAP] = "BTN_TOOL_QUINTTAP";
    this->KeyCodeNameMap[BTN_STYLUS3] = "BTN_STYLUS3";
    this->KeyCodeNameMap[BTN_TOUCH] = "BTN_TOUCH";
    this->KeyCodeNameMap[BTN_STYLUS] = "BTN_STYLUS";
    this->KeyCodeNameMap[BTN_STYLUS2] = "BTN_STYLUS2";
    this->KeyCodeNameMap[BTN_TOOL_DOUBLETAP] = "BTN_TOOL_DOUBLETAP";
    this->KeyCodeNameMap[BTN_TOOL_TRIPLETAP] = "BTN_TOOL_TRIPLETAP";
    this->KeyCodeNameMap[BTN_TOOL_QUADTAP] = "BTN_TOOL_QUADTAP";
    this->KeyCodeNameMap[BTN_WHEEL] = "BTN_WHEEL";
    this->KeyCodeNameMap[BTN_GEAR_UP] = "BTN_GEAR_UP";
    this->KeyCodeNameMap[KEY_OK] = "KEY_OK";
    this->KeyCodeNameMap[KEY_SELECT] = "KEY_SELECT";
    this->KeyCodeNameMap[KEY_GOTO] = "KEY_GOTO";
    this->KeyCodeNameMap[KEY_CLEAR] = "KEY_CLEAR";
    this->KeyCodeNameMap[KEY_POWER2] = "KEY_POWER2";
    this->KeyCodeNameMap[KEY_OPTION] = "KEY_OPTION";
    this->KeyCodeNameMap[KEY_INFO] = "KEY_INFO";
    this->KeyCodeNameMap[KEY_TIME] = "KEY_TIME";
    this->KeyCodeNameMap[KEY_VENDOR] = "KEY_VENDOR";
    this->KeyCodeNameMap[KEY_ARCHIVE] = "KEY_ARCHIVE";
    this->KeyCodeNameMap[KEY_PROGRAM] = "KEY_PROGRAM";
    this->KeyCodeNameMap[KEY_CHANNEL] = "KEY_CHANNEL";
    this->KeyCodeNameMap[KEY_FAVORITES] = "KEY_FAVORITES";
    this->KeyCodeNameMap[KEY_EPG] = "KEY_EPG";
    this->KeyCodeNameMap[KEY_PVR] = "KEY_PVR";
    this->KeyCodeNameMap[KEY_MHP] = "KEY_MHP";
    this->KeyCodeNameMap[KEY_LANGUAGE] = "KEY_LANGUAGE";
    this->KeyCodeNameMap[KEY_TITLE] = "KEY_TITLE";
    this->KeyCodeNameMap[KEY_SUBTITLE] = "KEY_SUBTITLE";
    this->KeyCodeNameMap[KEY_ANGLE] = "KEY_ANGLE";
    this->KeyCodeNameMap[KEY_FULL_SCREEN] = "KEY_FULL_SCREEN";
    this->KeyCodeNameMap[KEY_MODE] = "KEY_MODE";
    this->KeyCodeNameMap[KEY_KEYBOARD] = "KEY_KEYBOARD";
    this->KeyCodeNameMap[KEY_ASPECT_RATIO] = "KEY_ASPECT_RATIO";
    this->KeyCodeNameMap[KEY_PC] = "KEY_PC";
    this->KeyCodeNameMap[KEY_TV] = "KEY_TV";
    this->KeyCodeNameMap[KEY_TV2] = "KEY_TV2";
    this->KeyCodeNameMap[KEY_VCR] = "KEY_VCR";
    this->KeyCodeNameMap[KEY_VCR2] = "KEY_VCR2";
    this->KeyCodeNameMap[KEY_SAT] = "KEY_SAT";
    this->KeyCodeNameMap[KEY_SAT2] = "KEY_SAT2";
    this->KeyCodeNameMap[KEY_CD] = "KEY_CD";
    this->KeyCodeNameMap[KEY_TAPE] = "KEY_TAPE";
    this->KeyCodeNameMap[KEY_RADIO] = "KEY_RADIO";
    this->KeyCodeNameMap[KEY_TUNER] = "KEY_TUNER";
    this->KeyCodeNameMap[KEY_PLAYER] = "KEY_PLAYER";
    this->KeyCodeNameMap[KEY_TEXT] = "KEY_TEXT";
    this->KeyCodeNameMap[KEY_DVD] = "KEY_DVD";
    this->KeyCodeNameMap[KEY_AUX] = "KEY_AUX";
    this->KeyCodeNameMap[KEY_MP3] = "KEY_MP3";
    this->KeyCodeNameMap[KEY_AUDIO] = "KEY_AUDIO";
    this->KeyCodeNameMap[KEY_VIDEO] = "KEY_VIDEO";
    this->KeyCodeNameMap[KEY_DIRECTORY] = "KEY_DIRECTORY";
    this->KeyCodeNameMap[KEY_LIST] = "KEY_LIST";
    this->KeyCodeNameMap[KEY_MEMO] = "KEY_MEMO";
    this->KeyCodeNameMap[KEY_CALENDAR] = "KEY_CALENDAR";
    this->KeyCodeNameMap[KEY_RED] = "KEY_RED";
    this->KeyCodeNameMap[KEY_GREEN] = "KEY_GREEN";
    this->KeyCodeNameMap[KEY_YELLOW] = "KEY_YELLOW";
    this->KeyCodeNameMap[KEY_BLUE] = "KEY_BLUE";
    this->KeyCodeNameMap[KEY_CHANNELUP] = "KEY_CHANNELUP";
    this->KeyCodeNameMap[KEY_CHANNELDOWN] = "KEY_CHANNELDOWN";
    this->KeyCodeNameMap[KEY_FIRST] = "KEY_FIRST";
    this->KeyCodeNameMap[KEY_LAST] = "KEY_LAST";
    this->KeyCodeNameMap[KEY_AB] = "KEY_AB";
    this->KeyCodeNameMap[KEY_NEXT] = "KEY_NEXT";
    this->KeyCodeNameMap[KEY_RESTART] = "KEY_RESTART";
    this->KeyCodeNameMap[KEY_SLOW] = "KEY_SLOW";
    this->KeyCodeNameMap[KEY_SHUFFLE] = "KEY_SHUFFLE";
    this->KeyCodeNameMap[KEY_BREAK] = "KEY_BREAK";
    this->KeyCodeNameMap[KEY_PREVIOUS] = "KEY_PREVIOUS";
    this->KeyCodeNameMap[KEY_DIGITS] = "KEY_DIGITS";
    this->KeyCodeNameMap[KEY_TEEN] = "KEY_TEEN";
    this->KeyCodeNameMap[KEY_TWEN] = "KEY_TWEN";
    this->KeyCodeNameMap[KEY_VIDEOPHONE] = "KEY_VIDEOPHONE";
    this->KeyCodeNameMap[KEY_GAMES] = "KEY_GAMES";
    this->KeyCodeNameMap[KEY_ZOOMIN] = "KEY_ZOOMIN";
    this->KeyCodeNameMap[KEY_ZOOMOUT] = "KEY_ZOOMOUT";
    this->KeyCodeNameMap[KEY_ZOOMRESET] = "KEY_ZOOMRESET";
    this->KeyCodeNameMap[KEY_WORDPROCESSOR] = "KEY_WORDPROCESSOR";
    this->KeyCodeNameMap[KEY_EDITOR] = "KEY_EDITOR";
    this->KeyCodeNameMap[KEY_SPREADSHEET] = "KEY_SPREADSHEET";
    this->KeyCodeNameMap[KEY_GRAPHICSEDITOR] = "KEY_GRAPHICSEDITOR";
    this->KeyCodeNameMap[KEY_PRESENTATION] = "KEY_PRESENTATION";
    this->KeyCodeNameMap[KEY_DATABASE] = "KEY_DATABASE";
    this->KeyCodeNameMap[KEY_NEWS] = "KEY_NEWS";
    this->KeyCodeNameMap[KEY_VOICEMAIL] = "KEY_VOICEMAIL";
    this->KeyCodeNameMap[KEY_ADDRESSBOOK] = "KEY_ADDRESSBOOK";
    this->KeyCodeNameMap[KEY_MESSENGER] = "KEY_MESSENGER";
    this->KeyCodeNameMap[KEY_DISPLAYTOGGLE] = "KEY_DISPLAYTOGGLE";
    this->KeyCodeNameMap[KEY_SPELLCHECK] = "KEY_SPELLCHECK";
    this->KeyCodeNameMap[KEY_LOGOFF] = "KEY_LOGOFF";
    this->KeyCodeNameMap[KEY_DOLLAR] = "KEY_DOLLAR";
    this->KeyCodeNameMap[KEY_EURO] = "KEY_EURO";
    this->KeyCodeNameMap[KEY_FRAMEBACK] = "KEY_FRAMEBACK";
    this->KeyCodeNameMap[KEY_FRAMEFORWARD] = "KEY_FRAMEFORWARD";
    this->KeyCodeNameMap[KEY_CONTEXT_MENU] = "KEY_CONTEXT_MENU";
    this->KeyCodeNameMap[KEY_MEDIA_REPEAT] = "KEY_MEDIA_REPEAT";
    this->KeyCodeNameMap[KEY_10CHANNELSUP] = "KEY_10CHANNELSUP";
    this->KeyCodeNameMap[KEY_10CHANNELSDOWN] = "KEY_10CHANNELSDOWN";
    this->KeyCodeNameMap[KEY_IMAGES] = "KEY_IMAGES";
    this->KeyCodeNameMap[KEY_NOTIFICATION_CENTER] = "KEY_NOTIFICATION_CENTER";
    this->KeyCodeNameMap[KEY_PICKUP_PHONE] = "KEY_PICKUP_PHONE";
    this->KeyCodeNameMap[KEY_HANGUP_PHONE] = "KEY_HANGUP_PHONE";
    this->KeyCodeNameMap[KEY_DEL_EOL] = "KEY_DEL_EOL";
    this->KeyCodeNameMap[KEY_DEL_EOS] = "KEY_DEL_EOS";
    this->KeyCodeNameMap[KEY_INS_LINE] = "KEY_INS_LINE";
    this->KeyCodeNameMap[KEY_DEL_LINE] = "KEY_DEL_LINE";
    this->KeyCodeNameMap[KEY_FN] = "KEY_FN";
    this->KeyCodeNameMap[KEY_FN_ESC] = "KEY_FN_ESC";
    this->KeyCodeNameMap[KEY_FN_F1] = "KEY_FN_F1";
    this->KeyCodeNameMap[KEY_FN_F2] = "KEY_FN_F2";
    this->KeyCodeNameMap[KEY_FN_F3] = "KEY_FN_F3";
    this->KeyCodeNameMap[KEY_FN_F4] = "KEY_FN_F4";
    this->KeyCodeNameMap[KEY_FN_F5] = "KEY_FN_F5";
    this->KeyCodeNameMap[KEY_FN_F6] = "KEY_FN_F6";
    this->KeyCodeNameMap[KEY_FN_F7] = "KEY_FN_F7";
    this->KeyCodeNameMap[KEY_FN_F8] = "KEY_FN_F8";
    this->KeyCodeNameMap[KEY_FN_F9] = "KEY_FN_F9";
    this->KeyCodeNameMap[KEY_FN_F10] = "KEY_FN_F10";
    this->KeyCodeNameMap[KEY_FN_F11] = "KEY_FN_F11";
    this->KeyCodeNameMap[KEY_FN_F12] = "KEY_FN_F12";
    this->KeyCodeNameMap[KEY_FN_1] = "KEY_FN_1";
    this->KeyCodeNameMap[KEY_FN_2] = "KEY_FN_2";
    this->KeyCodeNameMap[KEY_FN_D] = "KEY_FN_D";
    this->KeyCodeNameMap[KEY_FN_E] = "KEY_FN_E";
    this->KeyCodeNameMap[KEY_FN_F] = "KEY_FN_F";
    this->KeyCodeNameMap[KEY_FN_S] = "KEY_FN_S";
    this->KeyCodeNameMap[KEY_FN_B] = "KEY_FN_B";
    this->KeyCodeNameMap[KEY_FN_RIGHT_SHIFT] = "KEY_FN_RIGHT_SHIFT";
    this->KeyCodeNameMap[KEY_BRL_DOT1] = "KEY_BRL_DOT1";
    this->KeyCodeNameMap[KEY_BRL_DOT2] = "KEY_BRL_DOT2";
    this->KeyCodeNameMap[KEY_BRL_DOT3] = "KEY_BRL_DOT3";
    this->KeyCodeNameMap[KEY_BRL_DOT4] = "KEY_BRL_DOT4";
    this->KeyCodeNameMap[KEY_BRL_DOT5] = "KEY_BRL_DOT5";
    this->KeyCodeNameMap[KEY_BRL_DOT6] = "KEY_BRL_DOT6";
    this->KeyCodeNameMap[KEY_BRL_DOT7] = "KEY_BRL_DOT7";
    this->KeyCodeNameMap[KEY_BRL_DOT8] = "KEY_BRL_DOT8";
    this->KeyCodeNameMap[KEY_BRL_DOT9] = "KEY_BRL_DOT9";
    this->KeyCodeNameMap[KEY_BRL_DOT10] = "KEY_BRL_DOT10";
    this->KeyCodeNameMap[KEY_NUMERIC_0] = "KEY_NUMERIC_0";
    this->KeyCodeNameMap[KEY_NUMERIC_1] = "KEY_NUMERIC_1";
    this->KeyCodeNameMap[KEY_NUMERIC_2] = "KEY_NUMERIC_2";
    this->KeyCodeNameMap[KEY_NUMERIC_3] = "KEY_NUMERIC_3";
    this->KeyCodeNameMap[KEY_NUMERIC_4] = "KEY_NUMERIC_4";
    this->KeyCodeNameMap[KEY_NUMERIC_5] = "KEY_NUMERIC_5";
    this->KeyCodeNameMap[KEY_NUMERIC_6] = "KEY_NUMERIC_6";
    this->KeyCodeNameMap[KEY_NUMERIC_7] = "KEY_NUMERIC_7";
    this->KeyCodeNameMap[KEY_NUMERIC_8] = "KEY_NUMERIC_8";
    this->KeyCodeNameMap[KEY_NUMERIC_9] = "KEY_NUMERIC_9";
    this->KeyCodeNameMap[KEY_NUMERIC_STAR] = "KEY_NUMERIC_STAR";
    this->KeyCodeNameMap[KEY_NUMERIC_POUND] = "KEY_NUMERIC_POUND";
    this->KeyCodeNameMap[KEY_NUMERIC_A] = "KEY_NUMERIC_A";
    this->KeyCodeNameMap[KEY_NUMERIC_B] = "KEY_NUMERIC_B";
    this->KeyCodeNameMap[KEY_NUMERIC_C] = "KEY_NUMERIC_C";
    this->KeyCodeNameMap[KEY_NUMERIC_D] = "KEY_NUMERIC_D";
    this->KeyCodeNameMap[KEY_CAMERA_FOCUS] = "KEY_CAMERA_FOCUS";
    this->KeyCodeNameMap[KEY_WPS_BUTTON] = "KEY_WPS_BUTTON";
    this->KeyCodeNameMap[KEY_TOUCHPAD_TOGGLE] = "KEY_TOUCHPAD_TOGGLE";
    this->KeyCodeNameMap[KEY_TOUCHPAD_ON] = "KEY_TOUCHPAD_ON";
    this->KeyCodeNameMap[KEY_TOUCHPAD_OFF] = "KEY_TOUCHPAD_OFF";
    this->KeyCodeNameMap[KEY_CAMERA_ZOOMIN] = "KEY_CAMERA_ZOOMIN";
    this->KeyCodeNameMap[KEY_CAMERA_ZOOMOUT] = "KEY_CAMERA_ZOOMOUT";
    this->KeyCodeNameMap[KEY_CAMERA_UP] = "KEY_CAMERA_UP";
    this->KeyCodeNameMap[KEY_CAMERA_DOWN] = "KEY_CAMERA_DOWN";
    this->KeyCodeNameMap[KEY_CAMERA_LEFT] = "KEY_CAMERA_LEFT";
    this->KeyCodeNameMap[KEY_CAMERA_RIGHT] = "KEY_CAMERA_RIGHT";
    this->KeyCodeNameMap[KEY_ATTENDANT_ON] = "KEY_ATTENDANT_ON";
    this->KeyCodeNameMap[KEY_ATTENDANT_OFF] = "KEY_ATTENDANT_OFF";
    this->KeyCodeNameMap[KEY_ATTENDANT_TOGGLE] = "KEY_ATTENDANT_TOGGLE";
    this->KeyCodeNameMap[KEY_LIGHTS_TOGGLE] = "KEY_LIGHTS_TOGGLE";
    this->KeyCodeNameMap[BTN_DPAD_UP] = "BTN_DPAD_UP";
    this->KeyCodeNameMap[BTN_DPAD_DOWN] = "BTN_DPAD_DOWN";
    this->KeyCodeNameMap[BTN_DPAD_LEFT] = "BTN_DPAD_LEFT";
    this->KeyCodeNameMap[BTN_DPAD_RIGHT] = "BTN_DPAD_RIGHT";
    this->KeyCodeNameMap[KEY_ALS_TOGGLE] = "KEY_ALS_TOGGLE";
    this->KeyCodeNameMap[KEY_ROTATE_LOCK_TOGGLE] = "KEY_ROTATE_LOCK_TOGGLE";
    this->KeyCodeNameMap[KEY_REFRESH_RATE_TOGGLE] = "KEY_REFRESH_RATE_TOGGLE";
    this->KeyCodeNameMap[KEY_BUTTONCONFIG] = "KEY_BUTTONCONFIG";
    this->KeyCodeNameMap[KEY_TASKMANAGER] = "KEY_TASKMANAGER";
    this->KeyCodeNameMap[KEY_JOURNAL] = "KEY_JOURNAL";
    this->KeyCodeNameMap[KEY_CONTROLPANEL] = "KEY_CONTROLPANEL";
    this->KeyCodeNameMap[KEY_APPSELECT] = "KEY_APPSELECT";
    this->KeyCodeNameMap[KEY_SCREENSAVER] = "KEY_SCREENSAVER";
    this->KeyCodeNameMap[KEY_VOICECOMMAND] = "KEY_VOICECOMMAND";
    this->KeyCodeNameMap[KEY_ASSISTANT] = "KEY_ASSISTANT";
    this->KeyCodeNameMap[KEY_KBD_LAYOUT_NEXT] = "KEY_KBD_LAYOUT_NEXT";
    this->KeyCodeNameMap[KEY_EMOJI_PICKER] = "KEY_EMOJI_PICKER";
    this->KeyCodeNameMap[KEY_DICTATE] = "KEY_DICTATE";
    this->KeyCodeNameMap[KEY_CAMERA_ACCESS_ENABLE] = "KEY_CAMERA_ACCESS_ENABLE";
    this->KeyCodeNameMap[KEY_CAMERA_ACCESS_DISABLE] = "KEY_CAMERA_ACCESS_DISABLE";
    this->KeyCodeNameMap[KEY_CAMERA_ACCESS_TOGGLE] = "KEY_CAMERA_ACCESS_TOGGLE";
    this->KeyCodeNameMap[KEY_ACCESSIBILITY] = "KEY_ACCESSIBILITY";
    this->KeyCodeNameMap[KEY_DO_NOT_DISTURB] = "KEY_DO_NOT_DISTURB";
    this->KeyCodeNameMap[KEY_BRIGHTNESS_MIN] = "KEY_BRIGHTNESS_MIN";
    this->KeyCodeNameMap[KEY_BRIGHTNESS_MAX] = "KEY_BRIGHTNESS_MAX";
    this->KeyCodeNameMap[KEY_KBDINPUTASSIST_PREV] = "KEY_KBDINPUTASSIST_PREV";
    this->KeyCodeNameMap[KEY_KBDINPUTASSIST_NEXT] = "KEY_KBDINPUTASSIST_NEXT";
    this->KeyCodeNameMap[KEY_KBDINPUTASSIST_PREVGROUP] = "KEY_KBDINPUTASSIST_PREVGROUP";
    this->KeyCodeNameMap[KEY_KBDINPUTASSIST_NEXTGROUP] = "KEY_KBDINPUTASSIST_NEXTGROUP";
    this->KeyCodeNameMap[KEY_KBDINPUTASSIST_ACCEPT] = "KEY_KBDINPUTASSIST_ACCEPT";
    this->KeyCodeNameMap[KEY_KBDINPUTASSIST_CANCEL] = "KEY_KBDINPUTASSIST_CANCEL";
    this->KeyCodeNameMap[KEY_RIGHT_UP] = "KEY_RIGHT_UP";
    this->KeyCodeNameMap[KEY_RIGHT_DOWN] = "KEY_RIGHT_DOWN";
    this->KeyCodeNameMap[KEY_LEFT_UP] = "KEY_LEFT_UP";
    this->KeyCodeNameMap[KEY_LEFT_DOWN] = "KEY_LEFT_DOWN";
    this->KeyCodeNameMap[KEY_ROOT_MENU] = "KEY_ROOT_MENU";
    this->KeyCodeNameMap[KEY_MEDIA_TOP_MENU] = "KEY_MEDIA_TOP_MENU";
    this->KeyCodeNameMap[KEY_NUMERIC_11] = "KEY_NUMERIC_11";
    this->KeyCodeNameMap[KEY_NUMERIC_12] = "KEY_NUMERIC_12";
    this->KeyCodeNameMap[KEY_AUDIO_DESC] = "KEY_AUDIO_DESC";
    this->KeyCodeNameMap[KEY_3D_MODE] = "KEY_3D_MODE";
    this->KeyCodeNameMap[KEY_NEXT_FAVORITE] = "KEY_NEXT_FAVORITE";
    this->KeyCodeNameMap[KEY_STOP_RECORD] = "KEY_STOP_RECORD";
    this->KeyCodeNameMap[KEY_PAUSE_RECORD] = "KEY_PAUSE_RECORD";
    this->KeyCodeNameMap[KEY_VOD] = "KEY_VOD";
    this->KeyCodeNameMap[KEY_UNMUTE] = "KEY_UNMUTE";
    this->KeyCodeNameMap[KEY_FASTREVERSE] = "KEY_FASTREVERSE";
    this->KeyCodeNameMap[KEY_SLOWREVERSE] = "KEY_SLOWREVERSE";
    this->KeyCodeNameMap[KEY_DATA] = "KEY_DATA";
    this->KeyCodeNameMap[KEY_ONSCREEN_KEYBOARD] = "KEY_ONSCREEN_KEYBOARD";
    this->KeyCodeNameMap[KEY_PRIVACY_SCREEN_TOGGLE] = "KEY_PRIVACY_SCREEN_TOGGLE";
    this->KeyCodeNameMap[KEY_SELECTIVE_SCREENSHOT] = "KEY_SELECTIVE_SCREENSHOT";
    this->KeyCodeNameMap[KEY_NEXT_ELEMENT] = "KEY_NEXT_ELEMENT";
    this->KeyCodeNameMap[KEY_PREVIOUS_ELEMENT] = "KEY_PREVIOUS_ELEMENT";
    this->KeyCodeNameMap[KEY_AUTOPILOT_ENGAGE_TOGGLE] = "KEY_AUTOPILOT_ENGAGE_TOGGLE";
    this->KeyCodeNameMap[KEY_MARK_WAYPOINT] = "KEY_MARK_WAYPOINT";
    this->KeyCodeNameMap[KEY_SOS] = "KEY_SOS";
    this->KeyCodeNameMap[KEY_NAV_CHART] = "KEY_NAV_CHART";
    this->KeyCodeNameMap[KEY_FISHING_CHART] = "KEY_FISHING_CHART";
    this->KeyCodeNameMap[KEY_SINGLE_RANGE_RADAR] = "KEY_SINGLE_RANGE_RADAR";
    this->KeyCodeNameMap[KEY_DUAL_RANGE_RADAR] = "KEY_DUAL_RANGE_RADAR";
    this->KeyCodeNameMap[KEY_RADAR_OVERLAY] = "KEY_RADAR_OVERLAY";
    this->KeyCodeNameMap[KEY_TRADITIONAL_SONAR] = "KEY_TRADITIONAL_SONAR";
    this->KeyCodeNameMap[KEY_CLEARVU_SONAR] = "KEY_CLEARVU_SONAR";
    this->KeyCodeNameMap[KEY_SIDEVU_SONAR] = "KEY_SIDEVU_SONAR";
    this->KeyCodeNameMap[KEY_NAV_INFO] = "KEY_NAV_INFO";
    this->KeyCodeNameMap[KEY_BRIGHTNESS_MENU] = "KEY_BRIGHTNESS_MENU";
    this->KeyCodeNameMap[KEY_MACRO1] = "KEY_MACRO1";
    this->KeyCodeNameMap[KEY_MACRO2] = "KEY_MACRO2";
    this->KeyCodeNameMap[KEY_MACRO3] = "KEY_MACRO3";
    this->KeyCodeNameMap[KEY_MACRO4] = "KEY_MACRO4";
    this->KeyCodeNameMap[KEY_MACRO5] = "KEY_MACRO5";
    this->KeyCodeNameMap[KEY_MACRO6] = "KEY_MACRO6";
    this->KeyCodeNameMap[KEY_MACRO7] = "KEY_MACRO7";
    this->KeyCodeNameMap[KEY_MACRO8] = "KEY_MACRO8";
    this->KeyCodeNameMap[KEY_MACRO9] = "KEY_MACRO9";
    this->KeyCodeNameMap[KEY_MACRO10] = "KEY_MACRO10";
    this->KeyCodeNameMap[KEY_MACRO11] = "KEY_MACRO11";
    this->KeyCodeNameMap[KEY_MACRO12] = "KEY_MACRO12";
    this->KeyCodeNameMap[KEY_MACRO13] = "KEY_MACRO13";
    this->KeyCodeNameMap[KEY_MACRO14] = "KEY_MACRO14";
    this->KeyCodeNameMap[KEY_MACRO15] = "KEY_MACRO15";
    this->KeyCodeNameMap[KEY_MACRO16] = "KEY_MACRO16";
    this->KeyCodeNameMap[KEY_MACRO17] = "KEY_MACRO17";
    this->KeyCodeNameMap[KEY_MACRO18] = "KEY_MACRO18";
    this->KeyCodeNameMap[KEY_MACRO19] = "KEY_MACRO19";
    this->KeyCodeNameMap[KEY_MACRO20] = "KEY_MACRO20";
    this->KeyCodeNameMap[KEY_MACRO21] = "KEY_MACRO21";
    this->KeyCodeNameMap[KEY_MACRO22] = "KEY_MACRO22";
    this->KeyCodeNameMap[KEY_MACRO23] = "KEY_MACRO23";
    this->KeyCodeNameMap[KEY_MACRO24] = "KEY_MACRO24";
    this->KeyCodeNameMap[KEY_MACRO25] = "KEY_MACRO25";
    this->KeyCodeNameMap[KEY_MACRO26] = "KEY_MACRO26";
    this->KeyCodeNameMap[KEY_MACRO27] = "KEY_MACRO27";
    this->KeyCodeNameMap[KEY_MACRO28] = "KEY_MACRO28";
    this->KeyCodeNameMap[KEY_MACRO29] = "KEY_MACRO29";
    this->KeyCodeNameMap[KEY_MACRO30] = "KEY_MACRO30";
    this->KeyCodeNameMap[KEY_MACRO_RECORD_START] = "KEY_MACRO_RECORD_START";
    this->KeyCodeNameMap[KEY_MACRO_RECORD_STOP] = "KEY_MACRO_RECORD_STOP";
    this->KeyCodeNameMap[KEY_MACRO_PRESET_CYCLE] = "KEY_MACRO_PRESET_CYCLE";
    this->KeyCodeNameMap[KEY_MACRO_PRESET1] = "KEY_MACRO_PRESET1";
    this->KeyCodeNameMap[KEY_MACRO_PRESET2] = "KEY_MACRO_PRESET2";
    this->KeyCodeNameMap[KEY_MACRO_PRESET3] = "KEY_MACRO_PRESET3";
    this->KeyCodeNameMap[KEY_KBD_LCD_MENU1] = "KEY_KBD_LCD_MENU1";
    this->KeyCodeNameMap[KEY_KBD_LCD_MENU2] = "KEY_KBD_LCD_MENU2";
    this->KeyCodeNameMap[KEY_KBD_LCD_MENU3] = "KEY_KBD_LCD_MENU3";
    this->KeyCodeNameMap[KEY_KBD_LCD_MENU4] = "KEY_KBD_LCD_MENU4";
    this->KeyCodeNameMap[KEY_KBD_LCD_MENU5] = "KEY_KBD_LCD_MENU5";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY] = "BTN_TRIGGER_HAPPY";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY2] = "BTN_TRIGGER_HAPPY2";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY3] = "BTN_TRIGGER_HAPPY3";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY4] = "BTN_TRIGGER_HAPPY4";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY5] = "BTN_TRIGGER_HAPPY5";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY6] = "BTN_TRIGGER_HAPPY6";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY7] = "BTN_TRIGGER_HAPPY7";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY8] = "BTN_TRIGGER_HAPPY8";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY9] = "BTN_TRIGGER_HAPPY9";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY10] = "BTN_TRIGGER_HAPPY10";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY11] = "BTN_TRIGGER_HAPPY11";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY12] = "BTN_TRIGGER_HAPPY12";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY13] = "BTN_TRIGGER_HAPPY13";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY14] = "BTN_TRIGGER_HAPPY14";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY15] = "BTN_TRIGGER_HAPPY15";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY16] = "BTN_TRIGGER_HAPPY16";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY17] = "BTN_TRIGGER_HAPPY17";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY18] = "BTN_TRIGGER_HAPPY18";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY19] = "BTN_TRIGGER_HAPPY19";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY20] = "BTN_TRIGGER_HAPPY20";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY21] = "BTN_TRIGGER_HAPPY21";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY22] = "BTN_TRIGGER_HAPPY22";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY23] = "BTN_TRIGGER_HAPPY23";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY24] = "BTN_TRIGGER_HAPPY24";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY25] = "BTN_TRIGGER_HAPPY25";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY26] = "BTN_TRIGGER_HAPPY26";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY27] = "BTN_TRIGGER_HAPPY27";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY28] = "BTN_TRIGGER_HAPPY28";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY29] = "BTN_TRIGGER_HAPPY29";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY30] = "BTN_TRIGGER_HAPPY30";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY31] = "BTN_TRIGGER_HAPPY31";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY32] = "BTN_TRIGGER_HAPPY32";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY33] = "BTN_TRIGGER_HAPPY33";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY34] = "BTN_TRIGGER_HAPPY34";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY35] = "BTN_TRIGGER_HAPPY35";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY36] = "BTN_TRIGGER_HAPPY36";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY37] = "BTN_TRIGGER_HAPPY37";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY38] = "BTN_TRIGGER_HAPPY38";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY39] = "BTN_TRIGGER_HAPPY39";
    this->KeyCodeNameMap[BTN_TRIGGER_HAPPY40] = "BTN_TRIGGER_HAPPY40";
}

std::string LinuxEvdev::KeyCodeToString(unsigned short key_code)
{
    if (key_code >= 0 && key_code < KEY_CNT)
    {
        return this->KeyCodeNameMap[key_code];
    }
    else {
        std::cerr << "Key code " << key_code << " not found in lookup table. Cannot convert to string.";
        return std::to_string(key_code);
    }
}

InputEvent LinuxEvdev::ParseKeyEvent(const LinuxEvdevInputDevice &device, const input_event &key_event)
{
    return InputEvent
    {
        device.ID,
        device.Name,
        this->KeyCodeToString(key_event.code),
        InputType::Binary,
        key_event.value == KEY_RELEASE ? 0.0f : 1.0f
    };
}

std::queue<InputEvent> LinuxEvdev::GetInputEvents()
{
    std::queue<InputEvent> input_events;

    for (LinuxEvdevInputDevice &device : this->Devices)
    {
        // Read all input event structures from device.
        bool reading = true;
        while (reading)
        {
            input_event evdev_input_event;
            if (read(device.FileDescriptor, &evdev_input_event, sizeof(input_event)) == -1)
            {
                // Print errors only when input queue was not empty.
                if (errno != EAGAIN)
                {
                    std::cerr << "Failed to read from '" << device.ID << "'. " << strerror(errno) << "." << std::endl;
                }
                reading = false;
            }
            else
            {
                // Parse the event by type.
                switch (evdev_input_event.type)
                {
                    case EV_KEY:
                        // Ignore held repeats.
                        if (evdev_input_event.value != KEY_REPEAT)
                        {
                            input_events.push(this->ParseKeyEvent(device, evdev_input_event));
                        }
                        break;
                }
            }
    
        }
    }

    return input_events;
}

bool LinuxEvdev::IsKeyboard(LinuxEvdevInputDevice device)
{
    // Get supported keycode bitmap array.
    unsigned long supported_keycodes[(KEY_MAX + 1) / BITS_PER_LONG + 1] = {0};
    ioctl(device.FileDescriptor, EVIOCGBIT(EV_KEY, sizeof(supported_keycodes)), supported_keycodes);
    
    // Check support for keycodes KEY_A - KEY_Z, which are sequential. If all are present this is a keyboard.
    for (int keycode = KEY_A; keycode <= KEY_Z; keycode++) 
    {
        if (!(supported_keycodes[keycode / BITS_PER_LONG] & (1UL << (keycode % BITS_PER_LONG))))
        {
            return false;
        }
    }
    return true;
}

bool LinuxEvdev::IsDeviceSupported(LinuxEvdevInputDevice device)
{
    // There is no "device type" specifier, we need to filter devices by capability.
    // As of now this driver supports keyboards.
    return this->IsKeyboard(device);
}

void LinuxEvdev::EnumerateDevices()
{
    // Reset enumeration state.
    this->Devices.clear();

    // Get all input events in '/dev/input'.
    for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator("/dev/input"))
    {
        // Get file path, filter non-event paths.
        std::string file_path = entry.path().string();
        if (file_path.contains("event"))
        {
            // Try to open event path.
            int event_path_descriptor = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);
            if (errno == EACCES)
            {
                std::cerr << "User lacks permissions to open '/dev/input/eventX' files. Add user to 'input' group and restart session." << std::endl;
                break;
            }
            
            // Get human-readable device name.
            char device_name_buffer[256];
            if (ioctl(event_path_descriptor, EVIOCGNAME(sizeof(device_name_buffer)), device_name_buffer) < 0)
            {
                std::cerr << "Unable to get device name for '" << file_path << "'." << std::endl;
                continue;
            }
            else
            {
                // Construct device.
                LinuxEvdevInputDevice device
                {
                    event_path_descriptor,
                    file_path,
                    std::string(device_name_buffer)
                };

                // Insert device if driver supports it.
                if (this->IsDeviceSupported(device))
                {
                    this->Devices.push_back(device);
                }
                else
                {
                    close(event_path_descriptor);
                }
            }
        }
    }
}

void LinuxEvdev::Initialize()
{
    // Initialize key code name map.
    this->InitializeKeyCodeNameMap();

    // Enumerate input devices.
    this->EnumerateDevices();
}

LinuxEvdev::~LinuxEvdev()
{
    // Close all open files.
    for (LinuxEvdevInputDevice &device : this->Devices)
    {
        close(device.FileDescriptor);
    }
}

#endif