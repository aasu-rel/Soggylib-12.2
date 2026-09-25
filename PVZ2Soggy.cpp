#include <cstdint>
#include <cstring>
#include <string>
#include <atomic>
#include <thread>
#include <jni.h>
#include <unistd.h>

#include "logging.h"
#include "memUtils.h"
#include "offsets.h"

static std::atomic<uintptr_t> g_cachedBase{0};

static inline uintptr_t getBase() {
    uintptr_t b = g_cachedBase.load(std::memory_order_acquire);
    if (!b) {
        b = getLibraryAddress("libPVZ2.so");
        g_cachedBase.store(b, std::memory_order_release);
    }
    return b;
}

static std::atomic<bool> g_highView{true};
static std::atomic<bool> g_loaded{false};

static bool getHighView() {
    if (!g_loaded.load(std::memory_order_acquire)) {
        g_highView.store(true, std::memory_order_release);
        g_loaded.store(true, std::memory_order_release);
    }
    return g_highView.load(std::memory_order_acquire);
}

static void setHighView(bool v) {
    g_highView.store(v, std::memory_order_release);
}

static std::string g_viewAngleLabel = "Full lawn (wide view)";

static uintptr_t g_lastSettingsPage   = 0;
static uintptr_t g_viewAngleContainer = 0;
static uintptr_t g_viewAngleChild     = 0;
static bool      g_injected           = false;
static uintptr_t g_captureChild       = 0;

typedef uintptr_t (*CreateTab)(uintptr_t, uint32_t, uintptr_t, uintptr_t, uintptr_t,
                               uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                               uintptr_t, uintptr_t, uintptr_t);
typedef long      (*AddWidget)(uintptr_t, uintptr_t, uint8_t, float);
typedef uintptr_t (*CreateCB)(uintptr_t, uint32_t, uintptr_t, char, int);

typedef long (*Dispatch)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                         uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                         uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                         uintptr_t, uintptr_t, uintptr_t, uintptr_t);

typedef uintptr_t (*SetChecked_t)(uintptr_t, uintptr_t, uintptr_t, char, int, int);

typedef long (*BoardChangeState_t)(uintptr_t, uintptr_t);

static CreateTab          oCreateTab         = nullptr;
static AddWidget          oAddWidget         = nullptr;
static CreateCB           oCreateCB          = nullptr;
static Dispatch           oDispatch          = nullptr;
static SetChecked_t       oSetChecked        = nullptr;
static BoardChangeState_t oBoardChangeState  = nullptr;

static uintptr_t hkCreateTab(uintptr_t a1, uint32_t a2,
                             uintptr_t a3, uintptr_t a4, uintptr_t a5,
                             uintptr_t a6, uintptr_t a7, uintptr_t a8,
                             uintptr_t a9, uintptr_t a10,
                             uintptr_t a11, uintptr_t a12, uintptr_t a13)
{
    if (a1 != g_lastSettingsPage) {
        LOGI("New settings page: %p", (void*)a1);
        g_lastSettingsPage   = a1;
        g_viewAngleContainer = 0;
        g_viewAngleChild     = 0;
        g_injected           = false;
    }

    if (!g_injected && a2 == 6 && oCreateCB && oAddWidget) {
        g_injected = true;

        g_captureChild = (uintptr_t)-1;

        uintptr_t cb = oCreateCB(a1, SETTINGS_VIEW_ANGLE_ID,
                                 (uintptr_t)&g_viewAngleLabel,
                                 getHighView() ? 1 : 0, 0);

        g_viewAngleContainer = cb;
        g_viewAngleChild = (g_captureChild == (uintptr_t)-1) ? 0 : g_captureChild;
        g_captureChild = 0;

        LOGI("  INJECT: container=%p child=%p",
             (void*)g_viewAngleContainer, (void*)g_viewAngleChild);

        if (cb) {
            uintptr_t cont = *(uintptr_t*)(a1 + SETTINGS_PAGE_CONTAINER);
            if (cont) oAddWidget(cont, cb, 0, 0.0f);
        }
    }

    return oCreateTab(a1, a2, a3, a4, a5,
                      a6, a7, a8, a9, a10,
                      a11, a12, a13);
}

static uintptr_t hkCreateCB(uintptr_t page, uint32_t id,
                            uintptr_t label, char init, int p5)
{
    return oCreateCB(page, id, label, init, p5);
}

static long hkAddWidget(uintptr_t cont, uintptr_t w,
                        uint8_t centered, float s)
{
    if (!w) return 0;
    return oAddWidget(cont, w, centered, s);
}

static uintptr_t hkSetChecked(uintptr_t widget, uintptr_t a2, uintptr_t a3,
                              char state, int a5, int a6)
{
    if (g_captureChild == (uintptr_t)-1) {
        g_captureChild = widget;
    }
    return oSetChecked ? oSetChecked(widget, a2, a3, state, a5, a6) : widget;
}

static long hkDispatch(uintptr_t a1, uintptr_t a2,
                       uintptr_t a3, uintptr_t a4, uintptr_t a5,
                       uintptr_t a6, uintptr_t a7, uintptr_t a8,
                       uintptr_t a9, uintptr_t a10,
                       uintptr_t a11, uintptr_t a12,
                       uintptr_t a13, uintptr_t a14,
                       uintptr_t a15, uintptr_t a16,
                       uintptr_t a17, uintptr_t a18,
                       uintptr_t a19)
{
    const uint32_t id = (uint32_t)a2;

    if (id == SETTINGS_VIEW_ANGLE_ID) {
        if (g_viewAngleChild) {
            uint8_t state = *(uint8_t*)(g_viewAngleChild + WIDGET_STATE_OFFSET);
            bool newVal = (state != 0);
            if (getHighView() != newVal) {
                setHighView(newVal);
                LOGI(">>> Full lawn %s", newVal ? "ON" : "OFF");
            }
        }
        return 0;
    }

    if (id == 4) return 0;

    return oDispatch ? oDispatch(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                                 a11, a12, a13, a14, a15, a16, a17, a18, a19)
                     : 0;
}

static constexpr uintptr_t BOARD_STATE_PLAYING = 1;

static long hkBoardChangeState(uintptr_t board, uintptr_t newState) {
    LOGI("=== BoardChangeState board=%p state=%lu ===",
         (void*)board, (unsigned long)newState);

    long ret = oBoardChangeState(board, newState);

    if ((uint32_t)newState == BOARD_STATE_PLAYING && getHighView()) {
        *(float*)(board + BOARD_280) = 1.0f;

        int b283 = *(int*)(board + BOARD_283);
        int b286 = *(int*)(board + BOARD_286);
        *(int*)(board + BOARD_270) = -b283;
        *(int*)(board + BOARD_284) = -b283;
        *(int*)(board + BOARD_285) = (b283 + b286) / 2;

        LOGI(">>> Board camera modded: scale=%.2f camX=%d rect.mY=%d rect.mW=%d",
             *(float*)(board + BOARD_280),
             *(int*)(board + BOARD_270),
             *(int*)(board + BOARD_284),
             *(int*)(board + BOARD_285));
    }

    return ret;
}

static void ApplyHooks() {
    uintptr_t base = 0;
    while ((base = getLibraryAddress("libPVZ2.so")) == 0) usleep(100000);
    g_cachedBase.store(base, std::memory_order_release);
    LOGI("base = %p", (void*)base);

    // Settings
    PVZ2HookFunction(OFF_SettingsTabCreate, (void*)hkCreateTab, (void**)&oCreateTab);
    PVZ2HookFunction(OFF_SettingsAddWidget, (void*)hkAddWidget, (void**)&oAddWidget);
    PVZ2HookFunction(OFF_SettingsDispatch,  (void*)hkDispatch,  (void**)&oDispatch);
    PVZ2HookFunction(OFF_CheckboxCreate,    (void*)hkCreateCB,  (void**)&oCreateCB);
    PVZ2HookFunction(OFF_SetChecked,        (void*)hkSetChecked,(void**)&oSetChecked);

    // Board state change
    PVZ2HookFunction(OFF_BoardChangeState,  (void*)hkBoardChangeState, (void**)&oBoardChangeState);

    LOGI("Soggylib hookde");
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    std::thread(ApplyHooks).detach();
    return JNI_VERSION_1_6;
}