

#include "36_globals.h"
#include "36_project.h"
#include "36_objects.h"
#include "36_vst.h"
#include "36_params.h"
#include "36_sampleinstr.h"
#include "36_utils.h"
#include "36_pattern.h"
#include "36_env.h"
#include "36_audio_dev.h"
#include "36_juce_windows.h"
#include "36_juce_components.h"
#include "36_text.h"


#include "Binarysrc/images.h"






JuceVstParamWindow::JuceVstParamWindow(Vst2Plugin* vst)
{
    setName(vst->getObjName().data());

    //loadpresetbt->setBounds(getWidth() - 57, 2, 20, 19);

    VSTParamComponent* const contentComponent = new VSTParamComponent(vst);

    setContentComponent(contentComponent, true, true);

    //centreWithSize(getWidth(), getHeight());
}

ParamWindow::ParamWindow(Device36* dev)
{
    ParamComponent* const contentComponent = new ParamComponent(dev);

    setContentComponent(contentComponent, true, true);

    setName(contentComponent->getName());

    centreWithSize(getWidth(), getHeight());
}

#if(RELEASEBUILD == FALSE)

#define PRIV_KEY "395e5a740a571602e702c52067c8823221f164b224378e0cad901a4fb0f4dab5,8f6be22219d9b7074186ecd10375457ed6268a222ce74c2ec38602c2632869e1"

LicenseComponent::LicenseComponent()
{
    setSize(293, 191);

    addAndMakeVisible(userName = new TextEditor(T("UserName")));
    addAndMakeVisible(userEmail = new TextEditor(T("UserEmail")));

    userName->setFont(*ins);
    userName->setColour(TextEditor::textColourId, Colour(244, 244, 244));
    userName->setColour(TextEditor::backgroundColourId, Colour(4, 53, 46));
    userName->setColour(TextEditor::focusedOutlineColourId, Colour((uint8)208, (uint8)228, (uint8)218, (uint8)192));
    userName->setColour(TextEditor::outlineColourId, Colour(36, 158, 133));
    userName->setColour(TextEditor::shadowColourId, Colour(19, 84, 72));
    userName->setIndents(4, 4);
    userName->setText(T("UserName"));

    userEmail->setFont(*ins);
    userEmail->setColour(TextEditor::textColourId, Colour(244, 244, 244));
    userEmail->setColour(TextEditor::backgroundColourId, Colour(4, 53, 46));
    userEmail->setColour(TextEditor::focusedOutlineColourId, Colour((uint8)208, (uint8)228, (uint8)218, (uint8)192));
    userEmail->setColour(TextEditor::outlineColourId, Colour(36, 158, 133));
    userEmail->setColour(TextEditor::shadowColourId, Colour(19, 84, 72));
    userEmail->setIndents(4, 4);
    userEmail->setText(T("UserEmail"));

    addAndMakeVisible(generateButton = new ATextButton(T("Generate!")));
    generateButton->addButtonListener(this);

    userName->setBounds(10, 11, 211, 20);
    userEmail->setBounds(10, 41, 211, 20);
    generateButton->setBounds(10, 71, 73, 33);
}

void LicenseComponent::buttonClicked(Button* button)
{
    if(button == generateButton)
    {
        XmlElement xmlLicense(T("CMMSingleUserLicenseData"));
        xmlLicense.setAttribute(T("UserName"), userName->getText());
        xmlLicense.setAttribute(T("UserEmail"), userEmail->getText());

        
        Time time = Time::getCurrentTime();
        xmlLicense.setAttribute(T("Day"), time.getDayOfMonth());
        xmlLicense.setAttribute(T("Month"), time.getMonth());
        xmlLicense.setAttribute(T("Year"), time.getYear());

		xmlLicense.setAttribute(T("M36"), (int)0);

        BitArray val;

        const String s(xmlLicense.createDocument (String::empty, true));
        char buff[1024];
        s.copyToBuffer(buff, s.length());
        const MemoryBlock mb(buff, s.length());

        val.loadFromMemoryBlock (mb);

        RSAKey privkey(PRIV_KEY);

        privkey.applyToValue(val);

        File licfile(userName->getText() + T("/") + T("CMMSingleUserLicenseKey.license"));

        licfile.deleteFile();
        licfile.create();
        licfile.appendText(val.toString (16));
    }
}

#endif

HelpComponent::HelpComponent()
{
    setSize(375, 377);

    hotkeysimg = ImageFileFormat::loadFrom(images::hotkeys_png, images::hotkeys_pngSize);

    //addAndMakeVisible(group = new AGroupComponent(T("Group"), T("")));
    //group->setBounds(0, 1, getWidth(), getHeight() - 1);

    int x1 = 5;
    int y1 = 5;
    Colour clr = Colour(120, 185, 135);
    PlaceBigLabel("Left mouse button over grid = Start selection", 
                    x1, y1, clr);
    PlaceBigLabel("Alt + Left mouse button over grid = Brush with current instrument or element", 
                    x1, y1 + 12, clr);
    PlaceBigLabel("Shift + Left mouse button over grid = Draw slide-note", 
                    x1, y1 + 12 + 12, clr);
    PlaceBigLabel("Shift + drag element = Clone element", 
                    x1, y1 + 12 + 12 + 12, clr);
    PlaceBigLabel("Shift + Ctrl + drag pattern = Copy pattern to new one", 
                    x1, y1 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + MouseCursor wheel = Zoom at mouse position", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Shift + Left mouse button on any instrument = reassign selected notes to this instrument", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    PlaceBigLabel("Ctrl + 'C' = Copy selected elements", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'X = Cut selected elements", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'V' = Paste elements at mouse position", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'Z' = Undo", 
					x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'Y' = Redo", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'A' = Select all", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'D' = Mute/Unmute pattern(s)", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'Q' = Make pattern(s) unique", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    PlaceBigLabel("'////' = Slide", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'####' = Mute", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'~~~~' = Vibrato", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'%%%%' = Transpose", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'|' = Break", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'<' = Reverse", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'>' = No reverse (only when cursor is on sample)", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'{' = Place bookmark", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    PlaceBigLabel("Space = Start/stop playback on the main MPattern", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Esc = Switch note mode ON/OFF", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Tab/Shift + Tab = Advance cursor forward/backward BPB times", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("F3 = Button mixer", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
}

void HelpComponent::buttonClicked(Button* button)
{

}

void HelpComponent::paint(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(0, 4, getWidth() - 1, getHeight() - 5);
    g.drawImageAt(hotkeysimg, getWidth() - hotkeysimg->getWidth(), 2, false);
    g.restoreState();
}

AboutComponent::AboutComponent()
{
    setSize(393, 257);

    //AGroupComponent* group = new AGroupComponent(T("Group"), T(""));
    //addAndMakeVisible(group);
    //group->setBounds(0, 2, getWidth() - 1, getHeight() - 2);

    keysimg = ImageFileFormat::loadFrom(images::keysabout_png, images::keysabout_pngSize);

    int x1 = 3;
    int y1 = 6;
    Colour clr = Colour(120, 185, 135);
    PlaceTxtLabel1("DAW36 is a music sequencer, aimed to provide the fastest possible workflow", x1, y1, Colour(170, 225, 185));

    PlaceTxtLabel1("Visit program website at www.chaoticdaw.com", 
                    x1, y1 + 24, clr);

    PlaceTxtLabel1("", 
                    x1, y1 + 36, clr);

    PlaceTxtLabel1("DSP algorithms by Robin Schmidt (www.rs-met.com)", 
                    x1, y1 + 36 + 12, clr);

    PlaceTxtLabel1("VST and ASIO are registered trademarks of Steinberg GmbH", 
                    x1, y1 + 36 + 12 + 12 + 12, clr);

    PlaceTxtLabel1("DAW36 also uses the following additional libraries:", 
                    x1, y1 + 36 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceTxtLabel1("    - Libsndfile library � by Eric de Castro Lopo;", 
                    x1, y1 + 36 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceTxtLabel1("    - JUCE library � by Raw Material Software", 
                    x1, y1 + 36 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    HyperlinkButton* hbutt = new HyperlinkButton(T("www.rs-met.com"), URL(T("http://rs-met.com")));
    hbutt->setFont(*rox, false, Justification::left);
    addAndMakeVisible(hbutt);
    hbutt->setBounds(169, y1 + 50, 92, 15);

    hbutt = new HyperlinkButton(T("www.chaoticdaw.com"), URL(T("http://chaoticdaw.com")));
    hbutt->setFont(*rox, false, Justification::left);
    addAndMakeVisible(hbutt);
    hbutt->setBounds(236, y1 + 26, 112, 15);
}

void AboutComponent::buttonClicked(Button* button)
{

}

void AboutComponent::paint(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(0, 4, getWidth(), getHeight() - 5);
    g.drawImageAt(keysimg, 0, 2, false);
    g.restoreState();
}

ParamComponent::ParamComponent(Device36* dev)
{
    mdev = dev;

    setName(mdev->getObjName().data());

    if(mdev->getParams().size() > 0)
    {
        int x = 2;
        int y = 2;
        int my = y;

        ALabel* label;
        //AGroupComponent* tgroup = NULL;
        bool tgroup = false;
        int gx, gy, gw, gh;
        int radgroup = 100;

        char paramname[MAX_NAME_LENGTH];

        for(Param* p : mdev->getParams())
        {
            Parameter* param = dynamic_cast<Parameter*>(p);

            if (param == NULL)
                continue;

            strcpy(paramname, param->getName().data());

            if(param->getType() == Param_Bool)
            {
                if(tgroup == NULL)
                {
                    y += 0;
                    tgroup = true;
                    //tgroup = new AGroupComponent(T("Group"), T(""));
                    //addAndMakeVisible(tgroup);
                    gx = x + 1;
                    gy = y - 2;
                    gw = 140;
                    gh = 17;
                }
                else
                {
                    gh += 12;
                }

                BoolParam* bp = (BoolParam*)param;

                AToggleButton* tg = new AToggleButton((char*)bp->getValString().data());
                addAndMakeVisible(tg);
                tg->setToggleState(bp->outval, false);

                if(bp->grouped == true)
                {
                    tg->setRadioGroupId(radgroup);
                }
                else if(y > 6) // if there's already a param above
                {
                    y += 4;
                }

                tg->setBounds(x + 3, y + 8, ins->getStringWidth(bp->getValString().data()) + 45, 22);
                tg->setName(paramname);
                bp->atoggle = tg;
                tg->addButtonListener(this);

                y += 12;
            }
            else
            {
                // End of group of toggles
                if(tgroup == true)
                {
                    radgroup++;
                    y += 5;
                    gh += 10;
                    //tgroup->setBounds(gx, gy, gw, gh);
                    tgroup = false;
                }

                strcat(paramname, ":");

                ASlider* slider;
                addAndMakeVisible(slider = new ASlider(T("")));

                //slider->setRange(param->offset, param->range + param->offset);
                slider->setPopupMenuEnabled (true);
                //pBufferSlider->setValue(Random::getSystemRandom().nextDouble() * 100, false, false);
                //slider->setValue(param->value, false, false);

                slider->setSliderStyle(ASlider::LinearHorizontal);
                slider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 150, 20);
                slider->setTextBoxIsEditable(false);

                String str1(param->getName().data());
                str1.trim();
                str1.append(T(":"), 1);

                String str(param->getValString().data());
                str.trim();
                slider->setText(str, false);
                //slider->setTextColour(Colour(155, 255, 155));
                slider->setTextColour(Colour(170, 185, 195));
                slider->setTextBoxXY(ti->getStringWidth(str1) + 3, -2);
                slider->setBounds(x + 4, y + 5, 115, 22);
                slider->setParamIndex(param->getIndex());
                slider->setParameter(param);
                slider->addListener(this);
                //param->aslider = slider;

                addAndMakeVisible(label = new ALabel("", str1));
                label->setFont(*ti);
                label->setColour(ALabel::textColourId, Colour(140, 155, 162));
                label->setBounds(x + 4, y, ti->getStringWidth(str1) + 10, 17);

                y += 24;
            }

            if(y > my)
            {
                my = y;
            }

            //if(0 && param->prmNext != NULL)
            {
                x += 155;
                y = 10;
            }
        }

        if(tgroup)
        {
            y += 7;
            gh += 10;

            //tgroup->setBounds(gx, gy, gw, gh);

            tgroup = false;
        }
        else
        {
            y += 4;
        }

        setSize(x + 145, y + 6);
    }
    else
    {
        setSize(155, 25);
    }
}

void ParamComponent::buttonClicked(Button * button)
{
    for(Param* p : mdev->getParams())
    {
        Parameter* param = dynamic_cast<Parameter*>(p);

        if (param == NULL)
            continue;

        if(param->getType() == Param_Bool)
        {
            BoolParam* bp = (BoolParam*)param;

            if(bp->atoggle == button)
            {
                AToggleButton* atg = (AToggleButton*)button;

                bp->SetBoolValue(atg->getToggleState());
            }
        }
    }
}

void ParamComponent::sliderValueChanged(ASlider* slider)
{
    int idx = slider->getParamIndex();

    for(Param* p : mdev->getParams())
    {
        Parameter* param = dynamic_cast<Parameter*>(p);

        if (param == NULL)
            continue;

        /*
        if(param->index == idx)
        {
            float val = (float)slider->getValue();

            if(param->value != val)
            {
                param->setDirectValueFromControl(val);
            }

            slider->setText(String(param->getValString().data()), false);

            break;
        }*/
    }
}

VSTParamComponent::VSTParamComponent(Vst2Plugin* vst)
{
    dev = (Device36*)vst;

    if(dev->getParams().size() > 0)
    {
        int x = 2;
        int y = 2;
        int cnt = 0;

        ASlider* slider;
        ALabel* label;

        //Paramcell* pcell;

        int num = dev->getParams().size();

        int cnum = num;
        int hgt = cnum*25;
        int rnum = num/cnum;

        while(cnum > 0 && (cnum*25)/(rnum*155) > 0.6f)
        {
            rnum = num/cnum;
            cnum--;
        }

        char paramname[MAX_NAME_LENGTH];

        for(Param* p : dev->getParams())
        {
            Parameter* param = dynamic_cast<Parameter*>(p);

            if (param == NULL)
                continue;

            strcpy(paramname, param->getName().data());

            strcat(paramname, ":");

            addAndMakeVisible(slider = new ASlider(T("BufferSlider")));

            slider->setRange(0, 1);
            slider->setValue(param->getOutVal(), false, false);
            slider->setSliderStyle(ASlider::LinearHorizontal);
            slider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 150, 20);
            slider->setTextBoxIsEditable(false);
            slider->setPopupMenuEnabled(true);

            String str1(param->getName().data());

            str1 = str1.trim();
            str1.append(T(":"), 1);

            String str(param->getValString().data());

            str = str.trim();
            slider->setText(str, false);

            //slider->setTextColour(Colour(155, 255, 155));
            slider->setTextColour(Colour(170, 185, 195));
            slider->setTextBoxXY(ti->getStringWidth(str1) + 3, -2);
            slider->setBounds(x + 4, y + 5, 125, 22);
            slider->setParamIndex(param->getIndex());
            slider->setParameter(param);
            slider->addListener(this);
            //param->aslider = slider;

            addAndMakeVisible(label = new ALabel("", str1));

            label->setFont(*ti);
            label->setColour(ALabel::textColourId, Colour(140, 155, 162));
            label->setBounds(x + 4, y, ti->getStringWidth(str1) + 10, 17);

            y += 24;
            cnt++;

            if(cnt == cnum)
            {
                cnt = 0;
                x += 130;
                y = 2;
            }
        }

        setSize(x + 136, 24*cnum + 12);
    }
    else
    {
        setSize(135, 25);
    }
}

void VSTParamComponent::sliderValueChanged(ASlider* slider)
{
    int idx = slider->getParamIndex();

    for(Param* p : dev->getParams())
    {
        Parameter* param = dynamic_cast<Parameter*>(p);

        if (param == NULL)
            continue;

        /*
        if(param->index == idx)
        {
            float val = (float)slider->getValue();

            if(param->value != val)
            {
                param->setDirectValueFromControl(val);
            }

            slider->setText(String(param->getValString().data()).trim(), false);

            break;
        }*/
    }
}

AToggleButton* CComponent::PlaceToggleWithLabel(char * txt, BoolParam * bparam, int tgtype, int group, int x, int y, int w, int h)
{
    AToggleButton* tgbutt;
    addAndMakeVisible(tgbutt = new AToggleButton(txt));
    tgbutt->setType(tgtype);
    tgbutt->setRadioGroupId(group);
    tgbutt->setBounds(x, y, w, h);
    tgbutt->addButtonListener(this);
    if(bparam != NULL)
    {
        bparam->atoggle = tgbutt;
        tgbutt->setToggleState(bparam->outval, false);
    }
    return tgbutt;
}

ASlider* CComponent::PlaceSliderWithLabel(char* txt, Parameter* param, int x, int y, int w, int h)
{
    ASlider* slider;
    addAndMakeVisible(slider = new ASlider(txt));
    slider->setRange(0, 1);
    slider->setSliderStyle(ASlider::LinearHorizontal);
    slider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 150, 20);
    slider->setTextBoxIsEditable(false);
    slider->setPopupMenuEnabled(true);

    String str1(txt);
    //slider->setTextColour(Colour(135, 235, 135));
    slider->setTextColour(Colour(160, 185, 195));
    slider->setTextBoxXY(ti->getStringWidth(str1) + 2, -2);
    slider->addListener(this);
    slider->setBounds(x, y, w, h);

    if(param != NULL)
    {
        /*
        //param->aslider = slider;
        if(param->interval <= 0)
        {
            slider->setRange(param->offset, param->range + param->offset);
        }
        else
        {
            slider->setRange(param->offset, param->range + param->offset, param->interval);
        }

        slider->setValue(param->value, false, false);
        slider->setParamIndex(param->index);
        slider->setParameter(param);

        slider->setText(param->getValString().data(), false);*/
    }

    ALabel* label;
    addAndMakeVisible(label = new ALabel(txt, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, Colour(140, 155, 162));
    label->setBounds(x, y - 5, ti->getStringWidth(txt) + 10, 17);

    return slider;
}

ALabel* CComponent::PlaceSmallLabel(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, ti->getStringWidth(str1) + 10, 17);

    return label;
}

ALabel* CComponent::PlaceBigLabel(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, ins->getStringWidth(str1) + 10, 17);

    return label;
}

ALabel* CComponent::PlaceTxtLabel(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, prj->getStringWidth(str1) + 10, 17);

    return label;
}

ALabel* CComponent::PlaceTxtLabel1(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, rox->getStringWidth(str1) + 10, 17);

    return label;
}



SampleWindow::SampleWindow()
{
    sample = NULL;
    SampleComponent* const contentComponent = new SampleComponent();
    setContentComponent(contentComponent, true, true);
    centreWithSize(getWidth(), getHeight());
}

SampleWindow::~SampleWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}

void SampleWindow::closeButtonPressed()
{
    if(sample != NULL)
    {
        sample->redraw();
    }

    SubWindow::closeButtonPressed();
}

void SampleWindow::SetSample(Sample* smp)
{
    sample = smp;

    SampleComponent* scomp = (SampleComponent*)getContentComponent();

    scomp->SetSample(smp);

    if(smp != NULL)
    {
        setName(String(smp->getObjName().data()));
    }
    else
    {
        setName("");
    }
}

void SampleWindow::moved()
{
    //int a = 1;
}

SampleComponent::SampleComponent()      //: quitButton(0)
{
    sample = NULL;

    //AGroupComponent* group1 = new AGroupComponent(T("LFO Flt2 Freq"), T(""));
    //addAndMakeVisible(group1);
    //group1->setBounds(0, 0, 349, 349);

    addAndMakeVisible(wave = new SampleWave(sample));

    int tgx = 0;

    tgnorm = new AToggleButton(T("Normalize"));
    addAndMakeVisible(tgnorm);
    tgnorm->setRadioGroupId(0);
    tgnorm->setBounds(tgx, 171, 70, 22);
    tgnorm->addButtonListener(this);

    tgNoLoop = new AToggleButton(T("No loop"));
    addAndMakeVisible(tgNoLoop);
    tgNoLoop->setRadioGroupId(1);
    tgNoLoop->setBounds(tgx, 188, 70, 22);
    tgNoLoop->addButtonListener(this);

    tgFwdLoop = new AToggleButton(T("Forward loop"));
    addAndMakeVisible(tgFwdLoop);
    tgFwdLoop->setRadioGroupId(1);
    tgFwdLoop->setBounds(tgx, 188 + 12, 90, 22);
    tgFwdLoop->addButtonListener(this);

    tgPPongLoop = new AToggleButton(T("Ping Pong loop"));
    addAndMakeVisible(tgPPongLoop);
    tgPPongLoop->setRadioGroupId(1);
    tgPPongLoop->setBounds(tgx, 188 + 12 + 12, 110, 22);
    tgPPongLoop->addButtonListener(this);

    tgSustain = new AToggleButton(T("Sustain envelope"));
    addAndMakeVisible(tgSustain);
    tgSustain->setRadioGroupId(0);
    tgSustain->setBounds(tgx, 244, 120, 22);
    tgSustain->addButtonListener(this);

    slTime = PlaceSliderWithLabel("Env. Scale", NULL, 249, 233, 98);
    slTime->setText("", false);

    addAndMakeVisible(btAlign = new ATextButton(String::empty));
    btAlign->setButtonText(T("Align scale to sample"));
    btAlign->addButtonListener(this);
    btAlign->setBounds(214, 215, 133, 18);

    setSize(350, 350);
}


void SampleComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if(buttonThatWasClicked == tgnorm)
    {
        if(sample != NULL)
        {
            sample->toggleNormalize();
            wave->repaint();
        }
    }
}

void SampleComponent::sliderValueChanged(ASlider* slider)
{
///
}

void SampleComponent::resized()
{
    wave->setBounds(3, 6, getWidth() - 6, 160);

    //butt_normalize->setBounds(getWidth() - 60, 163, 55, 22);
}

void SampleComponent::SetSample(Sample* smp)
{
    sample = smp;

    wave->SetSample(smp);

    if(sample != NULL)
    {
        tgnorm->setToggleState(smp->normalized, false);

        if(smp->looptype == LoopType_NoLoop)
        {
            tgNoLoop->setToggleState(true, false);

            tgSustain->setVisible(false);
        }
        else if(smp->looptype == LoopType_ForwardLoop)
        {
            tgFwdLoop->setToggleState(true, false);

            tgSustain->setVisible(true);
        }
        else if(smp->looptype == LoopType_PingPongLoop)
        {
            tgPPongLoop->setToggleState(true, false);

            tgSustain->setVisible(true);
        }

        float max = 10;

        if(smp->timelen > max)
        {
            max = smp->timelen;
        }

        float min = 1;

        //if(sample->timelen < min)
        //    min = sample->timelen;

        slTime->setRange(min, max);
        slTime->setValue(smp->envVol->ticklen);

        //if(sample->timelen > 2)
        //    slTime->setValue(sample->timelen);
        //else
        //    slTime->setValue(2);

        slTime->setText("", false);
    }
}

Looper::Looper(bool lside, SampleWave * wv)
{
    leftside = lside;
    wave = wv;
}

void Looper::setWave(SampleWave * wv)
{
    wave = wv;
}

void Looper::paint(Graphics & g)
{
    if(isMouseOverOrDragging())
    {
        g.setColour(Colour(0xffFFFF00));
    }
    else
    {
        g.setColour(Colour(0xffFF2020));
    }

    g.setFont(*ti);

    if(leftside)
    {
        g.drawVerticalLine(0, 0, float(getHeight()));
        g.drawHorizontalLine(0, 0, float(getWidth()));
        g.drawHorizontalLine(1, 0, float(getWidth()) - 1);
        g.drawHorizontalLine(2, 0, float(getWidth()) - 2);
        //g.drawHorizontalLine(getHeight() - 1, 0, float(getWidth()));
    }
    else
    {
        g.drawVerticalLine(getWidth() - 1, 0, float(getHeight()));
        //g.drawHorizontalLine(0, 0, float(getWidth()));
        g.drawHorizontalLine(getHeight() - 1, 0, float(getWidth()));
        g.drawHorizontalLine(getHeight() - 2, 1, float(getWidth()));
        g.drawHorizontalLine(getHeight() - 3, 2, float(getWidth()));
    }
}

void Looper::mouseDown(const MouseEvent &e)
{
    dragger.startDraggingComponent(this, 0);
}

void Looper::mouseEnter(const MouseEvent &e)
{
    repaint();
}

void Looper::mouseExit(const MouseEvent &e)
{
    repaint();
}

void Looper::mouseDrag(const MouseEvent &e)
{
    dragger.dragComponent(this, e);
    if(wave != NULL)
    {
        wave->ConstrainLooper(this);
    }
}

SampleWave::SampleWave(Sample* smp)
{
    addChildComponent(right = new Looper(false, this));
    addChildComponent(left = new Looper(true, this));
    setBufferedToImage(true);
    SetSample(smp);
}

void SampleWave::resized()
{
    left->setBounds(3, 0, 4, getHeight());
    right->setBounds(23, 0, 4, getHeight());
}

void SampleWave::Loopers()
{
    if(sample != NULL)
    {
        if(sample->looptype == LoopType_NoLoop)
        {
            left->setVisible(false);
            right->setVisible(false);
        }
        else
        {
            left->setVisible(true);
            right->setVisible(true);
        }
    }
}

void SampleWave::SetSample(Sample* smp)
{
    sample = smp;

    if(sample != NULL)
    {
        wratio = double(sample->sample_info.frames)/getWidth();
        wratio1 = double(sample->sample_info.frames - 1)/getWidth();

        SetLoopPoints(sample->lp_start, sample->lp_end);

        Loopers();
    }

    repaint();
}

void SampleWave::SetLoopPoints(long start, long end)
{
    loopstart = start;
    loopend = end;
    int xl = RoundDouble(loopstart/wratio1);
    int xr = RoundDouble(loopend/wratio1 - 4);
    left->setBounds(xl, left->getY(), left->getWidth(), left->getHeight());
    right->setBounds(xr, right->getY(), right->getWidth(), right->getHeight());
}

void SampleWave::ConstrainLooper(Looper * looper)
{
    if(looper == left)
    {
        int x = left->getX();
        int y = left->getY();
        if(x < 0)
            x = 0;
        else if(x > right->getX() - 1)
            x = right->getX() - 1;

        if(y != 0)
            y = 0;

        left->setBounds(x, y, left->getWidth(), left->getHeight());

        sample->setLoopStart(long(x*wratio1));
    }
    else if(looper == right)
    {
        int x = right->getX();
        int y = right->getY();
        if(x < left->getX() + 1)
            x = left->getX() + 1;
        else if(x > getWidth() - 4)
            x = getWidth() - 4;

        if(y != 0)
            y = 0;

        right->setBounds(x, y, right->getWidth(), right->getHeight());

        sample->setLoopEnd(RoundDoubleLong((x + 4)*wratio1));
    }
}

void SampleWave::paint(Graphics & g)
{
    g.fillAll(Colour(46, 62, 68));
    g.setColour(Colour(36, 44, 50));
    g.drawRect(0, 0, getWidth(), getHeight());

    if(sample != NULL)
    {
        if(sample != NULL && sample->sample_info.frames > 0)
        {
            int w = getWidth() - 1;
            wratio = double(sample->sample_info.frames)/w;
            long nframes = long(sample->sample_info.frames);
            float h2 = float(getHeight() - 1)/2;
            float* sdata = sample->sampleData;
            int step = sample->sample_info.channels;

            g.setColour(Colour(125, 125, 255));
            g.drawHorizontalLine((int)h2, 1, (float)w - 1);
            g.setColour(Colour(165, 185, 200));

            if(wratio > 1)
            {
                double fc = 0;
                int x = 1;
                long fc1, fc2;

                fc1 = fc2 = 0;

                float top, bottom;
                float tmpdata;

                while(x < w && (long)fc < nframes)
                {
                    fc += wratio;

                    if((long)fc >= nframes)
                    {
                        fc = nframes - 1;
                    }

                    fc1 = fc2;
                    fc2 = (long)fc;
                    top = bottom = sdata[fc1*step];

                    while(fc1 < fc2)
                    {
                        fc1++;

                        tmpdata = sdata[fc1*step];

                        if(tmpdata > top)
                        {
                            top = tmpdata;
                        }
                        else if(tmpdata < bottom)
                        {
                            bottom = tmpdata;
                        }
                    }

                    top = float(int(h2 - top*h2));
                    bottom = float(int(h2 - bottom*h2));

                    if(top != bottom)
                    {
                        g.drawVerticalLine(x, top, bottom);
                    }
                    else
                    {
                        g.setPixel(x, int(top));
                    }

                    x++;
                }

            }
            else
            {
                float xlen = float(1.0f/wratio);
                float x = 0;
                float y;
                long fc = 0;

                while(fc < nframes)
                {
                    y = h2 - sdata[fc*step]*h2;
                    g.drawHorizontalLine((int)y, x, x + xlen);
                    fc++;
                    x += xlen;
                }
            }
        }
    }
}

ConfigWindow::ConfigWindow()
{
    ConfigComponent* const contentComponent = new ConfigComponent();
    setContentComponent (contentComponent, true, true);
    centreWithSize(getWidth(), getHeight());

    setName("Preferences");
}

ConfigWindow::~ConfigWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}

RenderWindow::RenderWindow()
{
    RenderComponent* const contentComponent = new RenderComponent();

    this->RC = contentComponent;

    setContentComponent(contentComponent, true, true);

    centreWithSize(getWidth(), getHeight());

    setName("Render");
}

void RenderWindow::ResetToStart()
{
    MessageManagerLock ml;

    this->RC->slider->setValue(0, false, false);
    this->RC->startButt->setButtonText(T("Start"));
}

RenderWindow::~RenderWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}




