//
// VST3 wrapper
// Only the most useful stuff is supported
//
// Author               Date          Major Changes
//
// DenisL               11/19/2020      Initial version 
//

#include <direct.h>
#include <windows.h>

#ifdef VST3

#define INIT_CLASS_IID
//#include "3rd party/vstsdk3/pluginterfaces/base/smartpointer.h"
#include "3rd party/vstsdk3/pluginterfaces/base/ibstream.h"
#include "3rd party/vstsdk3/pluginterfaces/base/ipluginbase.h"
#include "3rd party/vstsdk3/pluginterfaces/vst/ivstunits.h"
//#include "3rd party/vstsdk3/public.sdk/source/vst/hosting/module.h"
//#include "3rd party/vstsdk3/public.sdk/source/vst/utility/stringconvert.h"


#include "36_globals.h"
#include "36_vst3.h"


extern "C" 
{
    using InitModuleFunc = bool (PLUGIN_API*)();
    using ExitModuleFunc = bool (PLUGIN_API*)();
}

// The <experimental/filesystem> header is deprecated. It is superseded by the C++17 <filesystem> header.
// You can define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING to silence the warning,
// otherwise the build will fail in VS2020 16.3.0
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 
#include <experimental/filesystem>

//using namespace std::experimental;
//using namespace VST3;
//using namespace VST3::Hosting;
using namespace Steinberg;


template <typename T>
T GetFunc(const char* name, HMODULE module)
{
    return reinterpret_cast<T>(GetProcAddress(module, name));
}

Vst3Module::~Vst3Module()
{
}

bool Vst3Module::LoadModule(const char *path)       // (const std::string& inPath)
{
    //auto wideStr = StringConvert::convert(inPath);
    //HMODULE module = LoadLibraryW(reinterpret_cast<LPCWSTR>(wideStr.data()));

    HMODULE module = LoadLibrary(path);

    if (!module)
    {
        // LoadLibray failed. Is there an API to get more information about the failure ?

        return false;
    }

    auto dllEntry = GetFunc<InitModuleFunc>("InitDll", module);
    auto factoryProc = GetFunc<GetFactoryProc>("GetPluginFactory", module);
    auto dllExit = GetFunc<ExitModuleFunc>("ExitDll", module);

    if (!factoryProc || !dllEntry)
    {
        // dll does not export required functions

        FreeLibrary(module);
        return false;
    }

    if (dllEntry && !dllEntry())
    {
        // InitDll() failed

        FreeLibrary(module);
        return false;
    }

        /** FUnknownPtr - automatic interface conversion and smart pointer in one.
            This template class can be used for interface conversion like this:
         \code{.cpp}
        IPtr<IPath> path = owned (FHostCreate (IPath, hostClasses));
        FUnknownPtr<IPath2> path2 (path); // does a query interface for IPath2
        if (path2)
            ...
         \endcode
        */

    IPtr<IPluginFactory>factory = (owned(factoryProc()));

    if (factory)
    {
        for (Steinberg::int32 i = 0; i < factory->countClasses(); i++)
        {
            PClassInfo classInfo;

            factory->getClassInfo(i, &classInfo);

            if (strcmp(classInfo.category, kVstAudioEffectClass) != 0)
            {
                factory->createInstance(classInfo.cid, Vst::IComponent::iid, (void**)&component);
            }
            else if (strcmp(classInfo.category, kVstComponentControllerClass) != 0)
            {
                factory->createInstance(classInfo.cid, Vst::IEditController::iid, (void**)&controller);
            }
            else
            {
                continue;
            }

            FUnknownPtr<IPluginFactory2> factory2(factory);

            if(factory2)
            {
                PClassInfo2 classInfo2;
                factory2->getClassInfo2(i, &classInfo2);
            }

            FUnknownPtr<IPluginFactory3> factory3(factory);

            if(factory3)
            {
                PClassInfoW classInfoW;
                factory3->getClassInfoUnicode(i, &classInfoW);
            }
        }

        factory->release();
    }

    if(dllExit) dllExit();

    FreeLibrary(module);

    return true;
}


#if 0

void LoadVst3()
{
    HMODULE hModule = LoadLibrary("SomePlugin.dll");

    if (hModule)
    {
        InitModuleFunc initProc = (InitModuleFunc)GetProcAddress(hModule, "InitDll");

        if (!initProc || !initProc())
        {
            FreeLibrary (hModule);
            return;
        }

        GetFactoryProc factProc = (GetFactoryProc)GetProcAddress(hModule, "GetPluginFactory");

        Steinberg::IPluginFactory* factory = factProc ? factProc() : NULL;

        if (factory)
        {
            for (int32 i = 0; i < factory->countClasses(); i++)
            {
                Steinberg::PClassInfo classInfo;

                factory->getClassInfo(i, &classInfo);

                Steinberg::FUnknown* obj;

                factory->createInstance(classInfo.cid, Steinberg::FUnknown::iid, (void**)&obj);

                //...

                obj->release();
            }

            factory->release();
        }

        ExitModuleFunc exitProc = (ExitModuleFunc)GetProcAddress(hModule, "ExitDll");

        if (exitProc)  exitProc();

        FreeLibrary (hModule);
    }
}


///////
/////////////////
/////////////
////////////////////////////////
///////////////////////

#endif

#endif