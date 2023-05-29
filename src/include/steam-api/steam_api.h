/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef STEAM_API_C_H
#define STEAM_API_C_H

#include <atheneum/atheneum.h>
#include <stdint.h>

extern Atheneum *g_steamApiAtheneum;

#if defined(_WIN32) && !defined(_X360)
#define S_API
#define S_CALLTYPE __stdcall
#elif defined(GNUC) || defined(__clang__)
#if __x86_64__
#define S_CALLTYPE
#else
#define S_CALLTYPE __attribute__((stdcall))
#endif
#else
#define S_CALLTYPE __attribute__((stdcall))
#endif

typedef int SteamBool;
typedef int SteamInt;
typedef uint64_t SteamAPICall_t;
typedef uint64_t SteamU64;
typedef const char *SteamConstantString;
typedef uint64_t SteamId;

typedef void (*CallbackRunFn)(void *_self, void *pvParam);
typedef void (*CallbackResultFn)(void *_self, void *pvParam,
                                 SteamBool bIOFailure,
                                 SteamAPICall_t handleSteamApiCall);
typedef SteamInt (*CallbackGetCallbackSizeBytes)(void);

// Do not modify order
typedef struct CallbackVTable {
  CallbackRunFn runFn;
  CallbackResultFn resultFn;
  CallbackGetCallbackSizeBytes getCallbackSizeBytesFn;
} CallbackVTable;

typedef struct CCallbackBase {
  void *vtable;
  uint8_t callbackFlags;
  SteamInt iCallback;
} CCallbackBase;

typedef SteamBool(S_CALLTYPE *SteamAPI_Init)(void);

typedef void(S_CALLTYPE *SteamAPI_RunCallbacks)(void);

typedef void(S_CALLTYPE *SteamAPI_RegisterCallback)(CCallbackBase *pCallback,
                                                    SteamInt iCallback);
typedef void(S_CALLTYPE *SteamAPI_UnregisterCallback)(CCallbackBase *pCallback);
typedef void(S_CALLTYPE *SteamAPI_RegisterCallResult)(CCallbackBase *pCallback,
                                                      SteamAPICall_t hAPICall);
typedef void(S_CALLTYPE *SteamAPI_UnregisterCallResult)(
    CCallbackBase *pCallback, SteamAPICall_t hAPICall);

typedef void* ISteamUtils;

typedef ISteamUtils*(S_CALLTYPE* SteamAPI_SteamUtils_v010)(void);

typedef void (S_CALLTYPE *WarningCallbackFn)(SteamInt severity, const char* message);

typedef void (S_CALLTYPE* SteamAPI_ISteamUtils_SetWarningMessageHook)(ISteamUtils* utils, WarningCallbackFn fn);

#include <stdint.h>

typedef struct SteamApi {
  Atheneum atheneum;
  SteamAPI_RunCallbacks runCallbacks;
  SteamAPI_RegisterCallback registerCallback;
  SteamAPI_RegisterCallResult registerCallResult;
  SteamAPI_SteamUtils_v010 createSteamUtilsInstance;
  SteamAPI_ISteamUtils_SetWarningMessageHook setWarningMessageHook;
  ISteamUtils* utils;
} SteamApi;

int steamApiInit(SteamApi *self);
int steamApiUpdate(SteamApi *self);
void steamApiRegisterCallback(SteamApi *self, CCallbackBase *pCallback,
                              SteamInt iCallback);

void steamApiRegisterCallResult(SteamApi *self, CCallbackBase *pCallback,
                                SteamAPICall_t hAPICall);

#endif
