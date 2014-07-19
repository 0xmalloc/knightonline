#pragma once

#include "CircularBuffer.h"
#include "SocketDefines.h"
#include "SocketOps.h"
#include "Socket.h"
#include "SocketMgr.h"

#if defined(CONFIG_USE_IOCP)
#	include "SocketMgrWin32.h"
#	include "ListenSocketWin32.h"
#elif defined(CONFIG_USE_EPOLL)
#	include "SocketMgrLinux.h"
#	include "ListenSocketLinux.h"
#elif defined(CONFIG_USE_KQUEUE)
#	include "SocketMgrBSD.h"
#	include "ListenSocketBSD.h" 
#endif

#include "JvCryption.h"
#include "KOSocket.h"