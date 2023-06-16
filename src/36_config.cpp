

#include "36_config.h"
#include "36_audiomanager.h"



ConfigData::ConfigData()
{

}

void ConfigData::saveSettings()
{
    XmlElement xmlSettings(T("ChaoticSettings"));

    xmlSettings.setAttribute(T("MakePeopleFat"), (int)1);
    xmlSettings.setAttribute(T("RememberAll"), (int)1);

    XmlElement* xmlRenderSettings = new XmlElement(T("RenderSettings"));
    xmlRenderSettings->setAttribute(T("Format"), (int)1);
    xmlRenderSettings->setAttribute(T("Quality"), (int)2);
    xmlRenderSettings->setAttribute(T("Q1"), (int)2);
    xmlRenderSettings->setAttribute(T("Q2"), (int)2);
    xmlRenderSettings->setAttribute(T("InBuffSize"), (int)3);
    xmlRenderSettings->setAttribute(T("OutDir"), "123");
    xmlRenderSettings->setAttribute(T("Interpolation"), (int)4);

    xmlSettings.addChildElement(xmlRenderSettings);

    XmlElement* xmlAudio = JAudioManager->createStateXml();

    if(xmlAudio != NULL)
    {
        xmlSettings.addChildElement(xmlAudio);
    }

    String  filePath(".\\settings.xml");
    File    file(filePath);

    xmlSettings.writeToFile(file, String::empty);
}


void ConfigData::loadSettings()
{
    
}




