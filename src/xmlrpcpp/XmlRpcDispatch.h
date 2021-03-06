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

#ifndef _XMLRPCDISPATCH_H_
#define _XMLRPCDISPATCH_H_

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include <vector>

namespace XmlRpc {

  // An RPC source represents a file descriptor to monitor
  class XmlRpcSource;

  //! An object which monitors file descriptors for events and performs
  //! callbacks when interesting events happen.
  class XmlRpcDispatch {
  public:
    //! Constructor
    XmlRpcDispatch();
    ~XmlRpcDispatch();

    //! Values indicating the type of events a source is interested in
    enum EventType {
      ReadableEvent = 1,    //!< data available to read
      WritableEvent = 2,    //!< connected/data can be written without blocking
      Exception     = 4     //!< uh oh
    };
    
    //! Monitor this source for the event types specified by the event mask
    //! and call its event handler when any of the events occur.
    //!  @param source The source to monitor
    //!  @param eventMask Which event types to watch for. \see EventType
    void addSource(XmlRpcSource* source, unsigned eventMask);

    //! Stop monitoring this source.
    //!  @param source The source to stop monitoring
    //! The source socket is not closed.
    void removeSource(XmlRpcSource* source);

    //! Modify the types of events to watch for on this source
    void setSourceEvents(XmlRpcSource* source, unsigned eventMask);


    //! Watch current set of sources and process events for the specified
    //! duration (in seconds, -1 implies wait forever, or until exit is called)
    void work(double timeSeconds);

    //! Exit from work routine
    void exit();

    //! Clear all sources from the monitored sources list. Sources are closed.
    void clear();

  protected:

    //! Wait for I/O on any source, timeout, or interrupt signal.
    bool waitForAndProcessEvents(double timeoutSeconds);


    //! Returns current time in seconds since something
    double getTime();

    // A source to monitor and what to monitor it for
    struct MonitoredSource
    {
      MonitoredSource(XmlRpcSource* src, unsigned mask) : _src(src), _mask(mask) {}
      XmlRpcSource* getSource() const { return _src; }
      unsigned& getMask() { return _mask; }
      XmlRpcSource* _src;
      unsigned _mask;
    };

    // A list of sources to monitor
    typedef std::vector< MonitoredSource > SourceList; 

    // Sources being monitored
    SourceList _sources;

    // When work should stop (-1 implies wait forever, or until exit is called)
    double _endTime;

    bool _doClear;
    bool _inWork;

  };
} // namespace XmlRpc

#endif  // _XMLRPCDISPATCH_H_
