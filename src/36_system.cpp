


#include "36_init.h"
#include "36_globals.h"


/** This is the application object that is started up when Juce starts. It handles
    the initialisation and shutdown of the whole application.
*/
class App36 : public JUCEApplication
{
    /* Important! NEVER embed objects directly inside your JUCEApplication class! Use
            ONLY pointers to objects, which you should create during the initialise() method
           (NOT in the constructor!) and delete in the shutdown() method (NOT in the
            destructor!)

           This is because the application object gets created before Juce has been properly
           initialised, so any embedded objects would also get constructed too soon.  */


public:

    App36()
    {
        // NEVER do anything in here that could involve any Juce function being called
        // - leave all your startup tasks until the initialise() method.
    }

    ~App36()
    {
        // Your shutdown() method should already have done all the things necessary to
        // clean up this app object, so you should never need to put anything in
        // the destructor.

        // Making any Juce calls in here could be very dangerous...
    }


    void handleCmdLine()
    {
        String arg = getCommandLineParameters();

        if(arg.length() > 0)
        {
            HandleCommandLine(arg);
        }
    }

    void initialise (const String& commandLine)
    {
        InitializeAndStartProgram();

        // Command line support disabled now
        // 
        // handleCmdLine();

        /*  ..and now return, which will fall into to the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().
        */
    }

    void shutdown()
    {
        // clear up..

        ExitProgram();
    }

    //
    const String getApplicationName()
    {
        return T("M");
    }

    const String getApplicationVersion()
    {
        return T("0.01");
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted(const String& commandLine)
    {
        HandleAnotherInstance(commandLine);
    }

    void systemRequestedQuit()
    {
        HandleSystremQuitRequest();
    }
};

// This macro creates the application's main() function..
//
START_JUCE_APPLICATION(App36);



