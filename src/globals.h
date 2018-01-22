#ifndef GLOBALS_H
#define GLOBALS_H


extern volatile bool gPaused;
extern volatile bool gStop;
class Sql;
extern Sql* gpSQL;
extern volatile int gLoopId;

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



#endif // GLOBALS_H
