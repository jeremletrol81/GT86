// Includes
#ifndef __SAM3X8E__
#include <avr/wdt.h> // Watchdog security
#endif
#include "./CANHelperLibrary/Connector/MCPCAN/CMCPCANConnector.h"
#include "./CANHelperLibrary/Modules/TOYOTA/FRS/CCloseDoors.h"
#include "./CANHelperLibrary/Modules/TOYOTA/FRS/COilDisplay.h"

#ifdef __SAM3X8E__
// workaround
void watchdogSetup(void) {}
#endif

// Declare the specific connector we want to use
CAN::CMCPCANConnector S_CAN ;

// A generic message used for read
CAN::CReadCANFrame F_READ_DATA ;

// Modules to use
FRS::CCloseDoors S_DOORS_LOCK(S_CAN, F_READ_DATA) ;
FRS::COilDisplay S_OIL_DISPLAY(S_CAN, F_READ_DATA) ;

/*****************************************************************************/
void setup()
{
  // Wait some time for initialization to be always correct (if powered with ACC or IGN only)
  DELAY(3000) ;
  
  // Open log
  OPEN_LOG() ;

  // While CAN initialization fails
  while (!CAN::InitializeCAN(S_CAN, &FRS::FiltersAndMasksConfiguration))
  {
    // Tell user
    PRINTLN_STR("CAN initialization failed") ;

    // Wait some time before trying again
    DELAY(100) ;
  }
  
  // Ok, everything seems good
  PRINTLN_STR("CAN initialization OK") ;

#ifdef __SAM3X8E__
  watchdogEnable(1000);
#endif
}

/*****************************************************************************/
void loop()
{  
#ifndef __SAM3X8E__
  // Setup the watchdog
  wdt_enable(WDTO_8S) ;
#endif

  // Update doors lock module
  S_DOORS_LOCK.Update(MILLIS()) ;

  // Update oil show module
  S_OIL_DISPLAY.Update(MILLIS()) ;

  // No errors occured on CAN ?
  if (!S_CAN.HasError())
  {
    // Reset watchdog
#ifdef __SAM3X8E__
    watchdogReset();
#else
    wdt_reset() ;
#endif
  }
  else
  {
#ifndef __SAM3X8E__
    // Reset watchdog
    wdt_reset() ;
#endif

    // Tell user which error it is
    PRINTLN_STR("Error in CAN, restarting the whole thing") ;
    PRINTLNHEX(S_CAN.GetError()) ;

#ifndef __SAM3X8E__
    // Restart in 15ms
    wdt_enable(WDTO_15MS) ;
#endif
    
    // Wait :(
    while (true) ;
  }
}
