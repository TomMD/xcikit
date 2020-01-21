// iocp/FSWatch.h created on 2020-01-21 as part of xcikit project
// https://github.com/rbrich/xcikit
//
// Copyright 2020 Radek Brich
// Licensed under the Apache License, Version 2.0 (see LICENSE file)

#ifndef XCI_CORE_IOCP_FSWATCH_H
#define XCI_CORE_IOCP_FSWATCH_H

#include "EventLoop.h"
#include <string>
#include <list>
#include <map>
#include <functional>

namespace xci::core {


/// Kqueue based filesystem watcher (using EVFILT_VNODE)
class FSWatch: public Watch {
public:

    enum class Event {
        Create,     ///< File was created or moved in
        Delete,     ///< File was deleted or moved away
        Modify,     ///< File content was modified
        Attrib,     ///< File attributes were changed
        Stopped,    ///< The file is no longer watched (containing directory was deleted or moved)
    };

    using Callback = std::function<void(const std::string& pathname, Event event)>;
    using PathCallback = std::function<void(Event event)>;

    /// Watch file system for changes and run a callback when an event occurs.
    /// \param cb       Callback function called for any event.
    explicit FSWatch(EventLoop& loop, Callback cb = nullptr);
    ~FSWatch() override;

    /// Watch file or directory for changes and run a callback when an event occurs.
    /// It's not an error if the file does not exist (yet).
    /// Note that this might add watch for parent directory, which will trigger
    /// events in main Callback.
    /// \param pathname File or directory to be watched.
    /// \param cb       Callback function called for each event.
    bool add(const std::string& pathname, PathCallback cb);

    /// Stop watching file or directory.
    /// \param pathname File or directory to remove. Must be same path as given to `add`.
    bool remove(const std::string& pathname);

    void _notify(const struct kevent& event) override;

private:
    int register_kevent(const std::string& path, uint32_t fflags, bool no_exist_ok=false);
    void unregister_kevent(int fd);

private:
    Callback m_main_cb;

    struct File {
        int fd;
        int dir_fd;
        std::string name;  // filename without dir part
        PathCallback cb;
    };
    std::list<File> m_file;

    struct Dir {
        int fd;
        std::string name;  // watched directory
    };
    std::list<Dir> m_dir;
};


} // namespace xci::core

#endif // include guard
