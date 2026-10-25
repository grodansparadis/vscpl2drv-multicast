// Copyright (c) 2000-2026 Ake Hedman and contributors, the VSCP Project
// SPDX-License-Identifier: LGPL-2.0-or-later
#pragma once
#include <vscp.h>

#if defined(_WIN32)
#if defined(VSCPL2DRV_MULTICAST_EXPORTS)
#define VSCPL2DRV_MULTICAST_API __declspec(dllexport)
#else
  #define VSCPL2DRV_MULTICAST_API __declspec(dllimport)
#endif
#elif defined(__GNUC__)
#define VSCPL2DRV_MULTICAST_API __attribute__((visibility("default")))
#else
#define VSCPL2DRV_MULTICAST_API
#endif

extern "C" {
VSCPL2DRV_MULTICAST_API long
VSCPOpen(const char *path, const char *guid);
VSCPL2DRV_MULTICAST_API int
VSCPClose(long handle);
VSCPL2DRV_MULTICAST_API int
VSCPWrite(long handle, const vscpEvent *event, unsigned long timeout);
VSCPL2DRV_MULTICAST_API int
VSCPRead(long handle, vscpEvent *event, unsigned long timeout);
VSCPL2DRV_MULTICAST_API unsigned long
VSCPGetVersion();
}
