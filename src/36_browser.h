
#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 
//#include <experimental/filesystem>

#include <list>
#include <string>
#include <vector>

#include "36_globals.h"
#include "36_objects.h"
#include "36_paramobject.h"





class Browser : public ParamObject
{
public:

            Browser(std::string dirpath);
            void                drawSelf(Graphics& g);
            void                handleChildEvent(Gobj * obj, InputEvent& ev);
            void                remap();

protected:

            Instrument*         ipreview;

            BrowserList*        fileBox;
            BrowserList*        internalList1;
            BrowserList*        projectsList1;
            BrowserList*        plugList;
            BrowserList*        sampleList1;
            BrowserList*        vstList1;
            BrowserList*        vstList2;

private:
            
};

