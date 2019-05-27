//
// Created by SVR00003 on 2017/9/26.
//

#ifndef XRSDK_XRAPISYSTEMUTILS_H
#define XRSDK_XRAPISYSTEMUTILS_H

#include "XrApiConfig.h"
#include "XrApiTypes.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef enum
{
    XRAPI_SYS_UI_GLOBAL_MENU,					// Display the Universal Menu.
    XRAPI_SYS_UI_CONFIRM_QUIT_MENU,				// Display the 'Confirm Quit' Menu.

    XRAPI_SYS_UI_KEYBOARD_MENU,					// Display a Keyboard Menu for editing a single string.
    XRAPI_SYS_UI_FILE_DIALOG_MENU,				// Display a Folder Browser Menu for selecting the path to a file or folder.

} xrSystemUIType;

// Display a specific System UI.
XRAPI_EXPORT bool xrapiShowSystemUI( const xrJava * java, const xrSystemUIType type );
//set performance level
XRAPI_EXPORT void xrapiSetVrSystemPerformance(const int gpuLevel, const int cpuLevel);
//release performance to lower
XRAPI_EXPORT void xrapiReleaseVrSystemPerformance();
//set sched fifo
XRAPI_EXPORT void xrapiSetSchedFifo(const int tid, const int cpu, const int priority, const int schedPolicy);
// ----DEPRECATED
// This function is DEPRECATED. Please do not write any new code which
// relies on it's use.
// Display a specific System UI and pass extra JSON text data.
XRAPI_EXPORT bool xrapiShowSystemUIWithExtra( const xrJava * java, const xrSystemUIType type, const char * extraJsonText );

// Launch the Oculus Home application.
// NOTE: This exits the current application by executing a finishAffinity.
XRAPI_EXPORT void xrapiReturnToHome( const xrJava * java );

// Display a Fatal Error Message using the System UI.
XRAPI_EXPORT void xrapiShowFatalError( const xrJava * java, const char * title, const char * message,
                                            const char * fileName, const unsigned int lineNumber );

#if defined( __cplusplus )
}	// extern "C"
#endif

#endif //XRSDK_XRAPISYSTEMUTILS_H
