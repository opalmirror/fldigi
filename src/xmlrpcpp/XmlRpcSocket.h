// ----------------------------------------------------------------------------
//
// XmlRpc++ Copyright (c) 2002-2008 by Chris Morley
//
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of fldigi
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _XMLRPCSOCKET_H_
#define _XMLRPCSOCKET_H_

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include <string>


namespace XmlRpc {

  //! A platform-independent socket API.
  class XmlRpcSocket {
  public:

    // On windows, a socket is an unsigned int large enough to hold a ptr
    // This should match the definition of SOCKET in winsock2.h
#if defined(_WINDOWS)
# if defined(_WIN64)
    typedef unsigned __int64 Socket;
# else
    typedef unsigned int Socket;
# endif
#else
    typedef int Socket;
#endif

    //! An invalid socket constant.
    static const Socket Invalid = (Socket) -1;

    //! Creates a stream (TCP) socket. Returns XmlRpcSocket::Invalid on failure.
    static Socket socket();

    //! Closes a socket.
    static void close(Socket socket);


    //! Sets a stream (TCP) socket to perform non-blocking IO. Returns false on failure.
    static bool setNonBlocking(Socket socket);


    // The next four methods are appropriate for servers.

    //! Allow the port the specified socket is bound to to be re-bound immediately so 
    //! server re-starts are not delayed. Returns false on failure.
    static bool setReuseAddr(Socket socket);

    //! Bind to a specified port
    static bool bind(Socket socket, int port);

    //! Set socket in listen mode
    static bool listen(Socket socket, int backlog);

    //! Accept a client connection request
    static Socket accept(Socket socket);

    //! Connect a socket to a server (from a client)
    static bool connect(Socket socket, std::string& host, int port);

    //! Get the port of a bound socket
    static int getPort(Socket socket);

    //! Returns true if the last error was not a fatal one (eg, EWOULDBLOCK)
    static bool nonFatalError();

    //! Returns last errno
    static int getError();

    //! Returns message corresponding to last error
    static std::string getErrorMsg();

    //! Returns message corresponding to error
    static std::string getErrorMsg(int error);
  };

} // namespace XmlRpc

#endif
