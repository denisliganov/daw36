
#pragma once

//
// VST3 wrapper
// Only the most useful stuff is supported
//
// Author       Date            Major Changes
//
// DenisL       11/19/2020      Initial version     (Ice storm came, winter starting)
//

#ifdef VST3

//#define INIT_CLASS_IID

//#include <string>

#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstattributes.h>
#include <pluginterfaces/vst/ivstmessage.h>
#include <pluginterfaces/vst/ivstcontextmenu.h>
#include <pluginterfaces/vst/ivstunits.h>
#include <pluginterfaces/vst/ivsthostapplication.h>
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/parameterchanges.h"

#include <map>

#include "36_globals.h"
#include "36_objects.h"
#include "36_device.h"



using namespace Steinberg;
using namespace Vst;


class Vst3Module  : 
                        public Vst::IComponent,
                        public Vst::IComponentHandler,  // VST V3.0.0
                        public Vst::IComponentHandler2, // VST V3.1.0
                        public Vst::IComponentHandler3, // VST V3.5.0
                        public Vst::IUnitInfo,
                        public Vst::IUnitHandler,
                        public Vst::IConnectionPoint,
                        public Vst::IAudioProcessor,
                        public Vst::IEditController,
                        public Vst::IEditController2
{
public:

            Vst3Module();
            ~Vst3Module();
            bool LoadModule(const char* path);      //(const std::string& inPath);

            IPtr<Vst::IComponent>           component;
            IPtr<Vst::IAudioProcessor>      processor;
            IPtr<Vst::IEditController>      controller;

            //Vst::EventList                  eventList;
            //Vst::ParameterChanges           inputParameterChanges;
            //Vst::ParameterChangeTransfer    paramTransferrer;

            Vst::ProcessContext             timingInfo;     // For use in process() only

/*
    // IComponentHandler
    tresult PLUGIN_API      beginEdit (Vst::ParamID paramID);
    tresult PLUGIN_API      performEdit (Vst::ParamID paramID, Vst::ParamValue valueNormalized);
    tresult PLUGIN_API      endEdit (Vst::ParamID paramID);
    tresult PLUGIN_API      restartComponent (Steinberg::int32 flags);
    // IComponentHandler2
    tresult PLUGIN_API      setDirty (TBool);
    tresult PLUGIN_API      requestOpenEditor (FIDString name);
    tresult PLUGIN_API      startGroupEdit();
    tresult PLUGIN_API      finishGroupEdit();
    // IUnitHandler
    tresult PLUGIN_API      notifyUnitSelection (Vst::UnitID);
    tresult PLUGIN_API      notifyProgramListChange (Vst::ProgramListID, Steinberg::int32);*/

private:

};

class Vst3Host : 
                    public Vst::IHostApplication
{
public:

            Vst3Host();
    virtual ~Vst3Host() {}

            // IHostApplication
            tresult PLUGIN_API      getName (Vst::String128 name);
            tresult PLUGIN_API      createInstance (TUID cid, TUID iid, void** obj);

            DECLARE_FUNKNOWN_METHODS

protected:

};

class HostAttr
{
public:
    enum Type
    {
        kInteger,
        kFloat,
        kString,
        kBinary
    };

    HostAttr(int64 value) : size(0), type(kInteger) { v.intValue = value; }
    HostAttr(double value) : size(0), type(kFloat) { v.floatValue = value; }
    /** size is in code unit (count of TChar) */
    HostAttr(const TChar* value, Steinberg::uint32 size) : size(size), type(kString)
    {
        v.stringValue = new TChar[size];
        memcpy(v.stringValue, value, size * sizeof(TChar));
    }
    HostAttr(const void* value, Steinberg::uint32 size) : size(size), type(kBinary)
    {
        v.binaryValue = new char[size];
        memcpy(v.binaryValue, value, size);
    }
    ~HostAttr()
    {
        if (size)
            delete[] v.binaryValue;
    }

    int64 intValue() const { return v.intValue; }
    double floatValue() const { return v.floatValue; }
    const TChar* stringValue(Steinberg::uint32& stringSize)
    {
        stringSize = size;
        return v.stringValue;
    }
    const void* binaryValue(Steinberg::uint32& binarySize)
    {
        binarySize = size;
        return v.binaryValue;
    }

    Type getType() const { return type; }

protected:
    union v
    {
        int64 intValue;
        double floatValue;
        TChar* stringValue;
        char* binaryValue;
    } v;
    Steinberg::uint32 size;
    Type type;
};


class AttrList : public IAttributeList
{
public:

            AttrList ();
    virtual ~AttrList ();

            tresult PLUGIN_API setInt (AttrID aid, int64 value);
            tresult PLUGIN_API getInt (AttrID aid, int64& value);
            tresult PLUGIN_API setFloat (AttrID aid, double value);
            tresult PLUGIN_API getFloat (AttrID aid, double& value);
            tresult PLUGIN_API setString (AttrID aid, const TChar* string) ;
            tresult PLUGIN_API getString (AttrID aid, TChar* string, Steinberg::uint32 sizeInBytes);
            tresult PLUGIN_API setBinary (AttrID aid, const void* data, Steinberg::uint32 sizeInBytes);
            tresult PLUGIN_API getBinary (AttrID aid, const void*& data, Steinberg::uint32& sizeInBytes);

            DECLARE_FUNKNOWN_METHODS

protected:

            void removeAttrID (AttrID aid);
            std::map<String, HostAttr*> list;
};

class Message : public IMessage
{
public:

            Message ();
            virtual ~Message ();

            const char*     PLUGIN_API  setMessageID ();
            void            PLUGIN_API  setMessageID (const char* messageID);
            IAttributeList* PLUGIN_API  getAttributes ();

            DECLARE_FUNKNOWN_METHODS

protected:

            char* messageId;
            AttrList* attributeList;
};

#endif