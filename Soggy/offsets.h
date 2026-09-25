#ifndef OFFSETS_H
#define OFFSETS_H

#ifdef __aarch64__

constexpr uintptr_t OFF_SettingsTabCreate      = 0x13569B4;
constexpr uintptr_t OFF_SettingsAddWidget      = 0x1356C70;
constexpr uintptr_t OFF_CheckboxCreate         = 0x1357180;
constexpr uintptr_t OFF_SettingsDispatch       = 0x135956C;
constexpr uintptr_t OFF_SetChecked             = 0x1DD97CC;

constexpr uintptr_t SETTINGS_PAGE_CONTAINER    = 240;
constexpr uintptr_t WIDGET_STATE_OFFSET        = 0x180;
constexpr uint32_t  SETTINGS_VIEW_ANGLE_ID     = 32;

constexpr uintptr_t OFF_BoardChangeState       = 0x13E8F04;

constexpr uintptr_t BOARD_270                  = 1064;
constexpr uintptr_t BOARD_280                  = 1104;
constexpr uintptr_t BOARD_283                  = 1116;
constexpr uintptr_t BOARD_284                  = 1120;
constexpr uintptr_t BOARD_285                  = 1124;
constexpr uintptr_t BOARD_286                  = 1128;

#endif
#endif