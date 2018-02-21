#include "sql.h"
#include "globals.h"


volatile bool gPaused;
volatile bool gStop;
Sql* gpSQL = nullptr;
volatile int gLoopId;


volatile bool gReboot;

