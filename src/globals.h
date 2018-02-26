#ifndef GLOBALS_H
#define GLOBALS_H


extern volatile bool gPaused;
extern volatile bool gStop;
class Sql;
extern Sql* gpSQL;
extern volatile int gLoopId;
extern volatile bool gReboot;

#if !defined(VERIFY)
# if !defined(QT_NO_DEBUG)
#  define VERIFY Q_ASSERT
# else
#  define VERIFY(expr)  \
    do                    \
    {                     \
        (void) (expr);    \
    } while (0)
# endif
#endif

enum ImageCacheType {
    IC_NEVER,
    IC_PERFOLDER,
    IC_ALWAYS,
};


enum SORTCOLUMN{
    SORT_NONE,
    SORT_FILENAME,
    SORT_SIZE,
    SORT_WTIME,
    SORT_RESOLUTION,
    SORT_DURATION,
    SORT_BITRATE,
    SORT_OPENCOUNT,

	COUNT_SORTCOLUMN,
} ;


#endif // GLOBALS_H
