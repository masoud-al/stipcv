
#include <juce.h>


//==============================================================================
class HelloWorldWindow  : public Component
{
public:
    //==============================================================================
    HelloWorldWindow()
        : Component (T("Hello World"))
    {
        setOpaque (true);
        setVisible (true);
        addToDesktop (ComponentPeer::windowAppearsOnTaskbar | ComponentPeer::windowHasDropShadow);
        centreWithSize (400, 200);
    }

    ~HelloWorldWindow()
    {
    }

    //==============================================================================
    void paint (Graphics& g)
    {
        GradientBrush gradientBrush (Colours::lightblue, 0, 0,
                                     Colours::lightblue.brighter (0.3f), 200.0f, 200.0f,
                                     true);
        g.setBrush (&gradientBrush);
        g.fillAll();

        g.setColour (Colours::black);

        g.setFont (20.0f, Font::bold);
        g.drawText (T("Hello World!"),
                    0, 0, getWidth(), getHeight(),
                    Justification::centred, false);

        g.setFont (13.0f, Font::italic);
        g.drawText (T("click on this window to quit..."),
                    0, 0, getWidth(), getHeight() - 10,
                    Justification::centredBottom, false);

        g.setColour (Colours::black.withAlpha (0.25f));
        g.drawRect (0, 0, getWidth(), getHeight());
    }

    void mouseDown (const MouseEvent& e)
    {
        JUCEApplication::quit();
    }
};


//==============================================================================
class JUCEHelloWorldApplication : public JUCEApplication
{
    HelloWorldWindow* helloWorldWindow;

public:
    //==============================================================================
    JUCEHelloWorldApplication()
        : helloWorldWindow (0)
    {
        // avoid doing anything complicated in the constructor - use initialise() instead.
    }

    ~JUCEHelloWorldApplication()
    {
        // avoid doing anything complicated in the destructor - use shutdown() instead.
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        // just create the main window...
        helloWorldWindow = new HelloWorldWindow();

        /*  ..and now return, which will fall into to the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().

            In this case, JUCEAppliction::quit() will be called by the
            hello world window being clicked.
        */
    }

    void shutdown()
    {
        // clear up..

        if (helloWorldWindow != 0)
            delete helloWorldWindow;
    }

    //==============================================================================
    const String getApplicationName()
    {
        return T("Hello World for JUCE");
    }

    const String getApplicationVersion()
    {
        return T("1.0");
    }

    bool moreThanOneInstanceAllowed()
    {
        return false;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
    }
};


//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION (JUCEHelloWorldApplication)
