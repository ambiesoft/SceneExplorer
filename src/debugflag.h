#ifndef DEBUGFLAG_H
#define DEBUGFLAG_H

#pragma once



#ifdef QT_DEBUG
// Uncomment to activate debug trace
//#define _DEBUG_ACTIVATED
//#define _DEBUG_AUTOCLOSE
//#define _DEBUG_BROWSER
//#define _DEBUG_CHROMIUM
//#define _DEBUG_CHROMIUMDOWNLOAD
//#define _DEBUG_DICSHORTCUT
//#define _DEBUG_FOCUS
//#define _DEBUG_GROUP
//#define _DEBUG_IDLING
//#define _DEBUG_LEAK
//#define _DEBUG_MOUSEGESTURE
//#define _DEBUG_MOUSEWHEEL
//#define _DEBUG_RESOURCE
//#define _DEBUG_SHORTCUT
//#define _DEBUG_SQLITE
//#define _DEBUG_SUBWINWNDPROC
//#define _DEBUG_TABPAINT
//#define _DEBUG_TABREORDER
//#define _DEBUG_THREAD
//#define _DEBUG_TRIDENT
//#define _DEBUG_WNDPROC
#endif


// spcial debug flag, can be enabled even if release mode
// be carefull not to ship with this flag
// #define TEMPORAL_RELEASE_DEBUG


// TODO: These macros should be rewritten as macro

#ifdef _DEBUG_ACTIVATED
    #define DTRACE_ACTIVATED(s) DTRACE(gcnew String(L"DebugActivated (") + (gDebugSerial++) + L"): " + (s))
    #pragma message("_DEBUG_ACTIVATED")
#else
    #define DTRACE_ACTIVATED(s) ((void)0)
#endif



#ifdef _DEBUG_AUTOCLOSE
    #define DTRACE_AUTOCLOSE(s) DTRACE_WITH_FUNCLINE(L"DebugAutoClose: " + (s))
    #pragma message("_DEBUG_AUTOCLOSE")
#else
    #define DTRACE_AUTOCLOSE(s) ((void)0)
#endif



#ifdef _DEBUG_AUTOCOMPLETE
    #define DTRACE_AUTOCOMPLETE(s) DTRACE(gcnew String(L"DebugAutoComplete: ") + (s))
    #pragma message("_DEBUG_AUTOCOMPLETE")
#else
    #define DTRACE_AUTOCOMPLETE(s) ((void)0)
#endif



#ifdef _DEBUG_BROWSER
    #define DTRACE_BROWSER(s) DTRACE_WITH_FUNCLINE(L"DebugBrowser: " + (s))
    #pragma message("_DEBUG_BROWSER")
#else
    #define DTRACE_BROWSER(s) ((void)0)
#endif



#ifdef _DEBUG_CHROMIUM
#define DTRACE_CHROMIUM(s) DTRACE_WITH_FUNCLINE(L"DebugChromium: " + (s))
    #pragma message("_DEBUG_CHROMIUM")
#else
    #define DTRACE_CHROMIUM(s) ((void)0)
#endif



#ifdef _DEBUG_CHROMIUMDOWNLOAD
#define DTRACE_CHROMIUMDOWNLOAD(s) DTRACE_WITH_FUNCLINE(L"DebugChromiumDownload: " + (s))
    #pragma message("_DEBUG_CHROMIUMDOWNLOAD")
#else
    #define DTRACE_CHROMIUMDOWNLOAD(s) ((void)0)
#endif



#ifdef _DEBUG_CLIPBOARD
    #define DTRACE_CLIPBOARD(s) DTRACE(gcnew String(L"DebugClipboard: ") + (s))
    #pragma message("_DEBUG_CLIPBOARD")
#else
    #define DTRACE_CLIPBOARD(s) ((void)0)
#endif



#ifdef _DEBUG_DICSHORTCUT
#define DTRACE_DICSHORTCUT(s) DTRACE_WITH_FUNCLINE(L"DicShortcut: " + (gDebugSerial++) + L": " + (s))
    #pragma message("_DEBUG_DICSHORTCUT")
#else
    #define DTRACE_DICSHORTCUT(s) ((void)0)
#endif



#ifdef _DEBUG_DRAGDROP
    #define DTRACE_DRAGDROP(s) DTRACE(gcnew String(L"DebugDragDrop: ") + (s))
    #pragma message("_DEBUG_DRAGDROP")
#else
    #define DTRACE_DRAGDROP(s) ((void)0)
#endif



#ifdef _DEBUG_FOCUS
    #define DTRACE_FOCUS(s) DTRACE_WITH_FUNCLINE(L"DebugFocus: " +  (gDebugSerial++) + L": " + (s))
    #pragma message("_DEBUG_FOCUS")
#else
    #define DTRACE_FOCUS(s) ((void)0)
#endif



#ifdef _DEBUG_GROUP
    #define DTRACE_GROUP(s) DTRACE_WITH_FUNCLINE(L"DebugGroup: " + (gDebugSerial++) + L": " + (s))
    #pragma message("_DEBUG_GROUP")
#else
    #define DTRACE_GROUP(s) ((void)0)
#endif



#ifdef _DEBUG_IDLING
    #define DTRACE_IDLING(s) DTRACE_WITH_FUNCLINE(gcnew String(L"DebugIdling: ") + (s))
    #pragma message("_DEBUG_IDLING")
#else
    #define DTRACE_IDLING(s) ((void)0)
#endif



#ifdef _DEBUG_LEAK
    #define DTRACE_LEAK(s) DTRACE_WITH_FUNCLINE(gcnew String(L"DebugLeak: ") + (s))
    #pragma message("_DEBUG_LEAK")
#else
    #define DTRACE_LEAK(s) ((void)0)
#endif



#ifdef _DEBUG_MOUSEGESTURE
    #define DTRACE_MOUSEGESTURE(s) DTRACE_WITH_FUNCLINE(L"DebugMouseGesture: " + (s))
    #pragma message("_DEBUG_MOUSEGESTURE")
#else
    #define DTRACE_MOUSEGESTURE(s) ((void)0)
#endif



#ifdef _DEBUG_MOUSEWHEEL
    #define DTRACE_MOUSEWHEEL(s) DTRACE_WITH_FUNCLINE(L"DebugMouseWheel: " + (s))
    #pragma message("_DEBUG_MOUSEWHEEL")
#else
    #define DTRACE_MOUSEWHEEL(s) ((void)0)
#endif



#ifdef _DEBUG_RESOURCE
    #define DTRACE_RESOURCE(s) DTRACE_WITH_FUNCLINE(L"DebugResource: " + (s))
    #pragma message("_DEBUG_RESOURCE")
#else
    #define DTRACE_RESOURCE(s) ((void)0)
#endif



#ifdef _DEBUG_SHORTCUT
    #define DTRACE_SHORTCUT(s) DTRACE_WITH_FUNCLINE(L"DebugShortCut: " + (s))
    #pragma message("_DEBUG_SHORTCUT")
#else
    #define DTRACE_SHORTCUT(s) ((void)0)
#endif



#ifdef _DEBUG_SQLITE
    #define DTRACE_SQLITE(s) DTRACE_WITH_FUNCLINE(L"DebugSqlite: " + (s))
    #pragma message("_DEBUG_SQLITE")
#else
    #define DTRACE_SQLITE(s) ((void)0)
#endif



#ifdef _DEBUG_SUBWINWNDPROC
    #define DTRACE_SUBWINWNDPROC(s) DTRACE_WITH_FUNCLINE(L"DebugSubwinWndProc: " + (s))
    #pragma message("_DEBUG_SUBWINWNDPROC")
#else
    #define DTRACE_SUBWINWNDPROC(s) ((void)0)
#endif



#ifdef _DEBUG_TABPAINT
    #define DTRACE_TABPAINT(s) DTRACE_WITH_FUNCLINE(L"DebugTabpaint: " + (s))
    #pragma message("_DEBUG_TABPAINT")
#else
    #define DTRACE_TABPAINT(s) ((void)0)
#endif



#ifdef _DEBUG_TABREORDER
    #define DTRACE_TABREORDER(s) DTRACE_WITH_FUNCLINE(L"DebugReorder: " + (s))
    #pragma message("_DEBUG_TABREORDER")
#else
    #define DTRACE_TABREORDER(s) ((void)0)
#endif



#ifdef _DEBUG_THREAD
    #define DTRACE_THREAD(s) DTRACE_WITH_FUNCLINE(L"DebugThread: " + (s))
    #pragma message("_DEBUG_THREAD")
#else
    #define DTRACE_THREAD(s) ((void)0)
#endif



#ifdef _DEBUG_TRIDENT
    #define DTRACE_TRIDENT(s) DTRACE_WITH_FUNCLINE(L"DebugTrident: " + (s))
    #pragma message("_DEBUG_TRIDENT")
#else
    #define DTRACE_TRIDENT(s) ((void)0)
#endif



#ifdef _DEBUG_WINDOWS
    #define DTRACE_WINDOWS(s) DTRACE_WITH_FUNCLINE(L"DebugWindows: " + (s))
    #pragma message("_DEBUG_WINDOWS")
#else
    #define DTRACE_WINDOWS(s) ((void)0)
#endif



#ifdef _DEBUG_WNDPROC
    #define DTRACE_WNDPROC(s) DTRACE_WITH_FUNCLINE(L"DebugWndProc: " + (s))
    #pragma message("_DEBUG_WNDPROC")
#else
    #define DTRACE_WNDPROC(s) ((void)0)
#endif






#ifdef TEMPORAL_RELEASE_DEBUG
#pragma message("=================================")
#pragma message("TEMPORAL_RELEASE_DEBUG is enabled")
#pragma message("=================================")
#endif


#endif // DEBUGFLAG_H
