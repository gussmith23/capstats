#define OTL_CPP_11_ON
#define OTL_ODBC // Compile OTL 4.0/ODBC
//#define ODBCVER 0x0250 // ODBC Version # needs to be downgraded
// to 2.5 because the SQLite ODBC driver seems
// to run slower when ODBC 3.x functions
// used (performance is not as good as with
// ODBC 2.5 function calls)
#include <otlv4.h> // include the OTL 4.0 header file