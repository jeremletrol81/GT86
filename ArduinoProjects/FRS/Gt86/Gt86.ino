// Includes
#include "./CANHelperLibrary/Connector/MCPCAN/CMCPCANConnector.h"
#include "./CANHelperLibrary/Modules/TOYOTA/FRS/CCloseDoors.h"
#include "./CANHelperLibrary/Modules/TOYOTA/FRS/COilDisplay.h"

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
}

/*****************************************************************************/
void loop()
{
	// Setup the watchdog
  WDT_Enable(WDT, WDT_MR_WDV(8000)|WDT_MR_WDRSTEN|WDT_MR_WDRPROC);
	
	// Update doors lock module
	S_DOORS_LOCK.Update(MILLIS()) ;

  // Update oil show module
  S_OIL_DISPLAY.Update(MILLIS()) ;

	// No errors occured on CAN ?
	if (!S_CAN.HasError())
	{
		// Reset watchdog
		WDT_Restart(WDT);
	}
	else
	{
		// Reset watchdog
    WDT_Restart(WDT);
		
		// Tell user which error it is
		PRINTLN_STR("Error in CAN, restarting the whole thing") ;
		PRINTLNHEX(S_CAN.GetError()) ;
		
		// Restart in 15ms
    WDT_Enable(WDT, WDT_MR_WDV(15)|WDT_MR_WDRSTEN|WDT_MR_WDRPROC);
		
		// Wait :(
		while (true) ;
	}
}
