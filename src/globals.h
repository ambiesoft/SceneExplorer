#ifndef GLOBALS_H
#define GLOBALS_H


extern volatile bool gPaused;
extern volatile bool gStop;
class Sql;
extern Sql* gpSQL;
extern volatile int gLoopId;
#endif // GLOBALS_H
