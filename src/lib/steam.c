/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <steam-api/steam_api.h>

Atheneum *g_steamApiAtheneum;

static void steamAPIDebugTextHook(int nSeverity, const char *pchDebugText) {
  if (nSeverity >= 1) {
    CLOG_ERROR("%s", pchDebugText)
  } else {
    CLOG_INFO("%s", pchDebugText)
  }
}

int steamApiInit(SteamApi *self) {

#if defined TORNADO_OS_WINDOWS
#define STEAM_LIBRARY_NAME "steam_api64.dll"
#elif TORNADO_OS_LINUX
#define STEAM_LIBRARY_NAME "./libsteam_api.so"
#elif TORNADO_OS_MACOS
#define STEAM_LIBRARY_NAME "./libsteam_api.dylib"
#else
#error "steamApi: Unsupported platform"
#endif
  int openResult = atheneumInit(&self->atheneum, STEAM_LIBRARY_NAME);
  if (openResult < 0) {
    CLOG_SOFT_ERROR("could not load steam library %s", STEAM_LIBRARY_NAME)
    return openResult;
  }

  g_steamApiAtheneum = &self->atheneum;
  SteamAPI_Init steamApiInit =
      (SteamAPI_Init)atheneumAddress(&self->atheneum, "SteamAPI_Init");

  SteamBool initWorked = steamApiInit();
  if (!initWorked) {
    CLOG_SOFT_ERROR("could not initialize steam. Is Steam running?")
    return -2;
  }

  self->runCallbacks = (SteamAPI_RunCallbacks)atheneumAddress(
      &self->atheneum, "SteamAPI_RunCallbacks");
  self->registerCallback = (SteamAPI_RegisterCallback)atheneumAddress(
      &self->atheneum, "SteamAPI_RegisterCallback");
  self->registerCallResult = (SteamAPI_RegisterCallResult)atheneumAddress(
      &self->atheneum, "SteamAPI_RegisterCallResult");

  self->createSteamUtilsInstance = (SteamAPI_SteamUtils_v010)atheneumAddress(
      &self->atheneum, "SteamAPI_SteamUtils_v010");

  self->setWarningMessageHook =
      (SteamAPI_ISteamUtils_SetWarningMessageHook)atheneumAddress(
          &self->atheneum, "SteamAPI_ISteamUtils_SetWarningMessageHook");

  CLOG_DEBUG("creating utils")
  self->utils = self->createSteamUtilsInstance();

  CLOG_DEBUG("setWarningMessageHook")
  self->setWarningMessageHook(self->utils, steamAPIDebugTextHook);

  return 0;
}

int steamApiUpdate(SteamApi *self) {
  self->runCallbacks();
  return 0;
}

void steamApiRegisterCallback(SteamApi *self, CCallbackBase *pCallback,
                              SteamInt iCallback) {

  self->registerCallback(pCallback, iCallback);
}

void steamApiRegisterCallResult(SteamApi *self, CCallbackBase *pCallback,
                                SteamAPICall_t hAPICall) {

  self->registerCallResult(pCallback, hAPICall);
}