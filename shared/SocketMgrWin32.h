/*
* Multiplatform Async Network Library
* Copyright (c) 2007 Burlex
* Hacked up to no good end by twostars (if you're looking for someone to blame!).
*
* SocketMgr - iocp-based SocketMgr for windows.
*
*/

#pragma once

#ifdef CONFIG_USE_IOCP

typedef void(*OperationHandler)(Socket * s, uint32 len);

void HandleReadComplete(Socket * s, uint32 len);
void HandleWriteComplete(Socket * s, uint32 len);
void HandleShutdown(Socket * s, uint32 len);

static OperationHandler ophandlers[] =
{
	&HandleReadComplete,
	&HandleWriteComplete,
	&HandleShutdown
};

#endif