/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 * Hacked up to no good end by twostars (if you're looking for someone to blame!).
 *
 * SocketMgr - epoll manager for Linux.
 *
 */


#pragma once

#ifdef CONFIG_USE_EPOLL

#define EPOLL_WAIT_TIMEOUT	5000

#define SOCKET_HOLDER_SIZE 65536    // You don't want this number to be too big, otherwise you're gonna be eating
                                    // memory. 65536 = 256KB, so thats no big issue for now, and I really can't
                                    // see anyone wanting to have more than 65536 concurrent connections.

#define THREAD_EVENT_SIZE 4096      // This is the number of socket events each thread can receieve at once.
                                    // This default value should be more than enough.

#endif
