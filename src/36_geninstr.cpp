
#include "36_geninstr.h"
#include "36_events_triggers.h"
#include "36_note.h"
#include "36_params.h"
#include "36_pattern.h"
#include "36_utils.h"



Generator::Generator()
{
    type = Instr_Generator;

    //solo->SetImages(NULL, img_solo1off_c);
    //mute->SetImages(NULL, img_mute1off_c);
    //autopt->SetImages(NULL, img_autooff_g);
    //pEditButton->SetImages(NULL, img_btwnd_g);
}

Generator::~Generator()
{
}

void Generator::CheckBounds(Note* gnote, Trigger* tg, long num_frames)
{
    if(tg->tgState == TS_Sustain && !tg->previewTrigger && tg->framePhase >= gnote->framelen)
    {
        // Go to release state as note is over

        tg->setState(TS_Release);
    }

    // Finishing can happen both when envelope is out of bounds and when the note length is longer than envelope 
    // length and the note is finished (frame phase exceeds note frame length)

    if(tg->tgState == TS_Release && envVol != NULL)
    {
        bool finished = false;

        finished = envVol->isOutOfBounds(tg->framePhase);

        endframe = num_frames - (tg->framePhase - envVol->framelen);

        if(finished)
        {
            tg->setState(TS_Finished);
        }

        if(endframe < 0)
        {
            endframe = 0;
        }
    }
}


Osc::Osc()
{
    phase_shift = 0;
    note_shift = 0;
    finetune = 0;
}

void Sine::GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2)
{
   *od1 = *od2 = wt_sine[int(wt_pos*wt_coeff*freq)&0x7FFF];
}

void Saw::GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2)
{
    *od1 = *od2 = wt_saw[int(wt_pos*wt_coeff*freq)&0x7FFF];
}

void Triangle::GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2)
{
    *od1 = *od2 = wt_triangle[int(wt_pos*wt_coeff*freq)&0x7FFF];
}

#if 0

Synth::Synth()
{
    SetName("Chaosynth");

    strcpy(this->preset_path, ".\\Presets\\Chaotic\\");
    strcpy(this->path,"internal://chaotic");
    uniqueID = MAKE_FOURCC('C','S','Y','N');

    fp = 0;
    rate = 0.0001f;
    numUsedVoices = 0;
    maxVoices = 5;

    osc1Fat = new BoolParam(false, "Osc1.3x");
    osc2Fat = new BoolParam(false, "Osc2.3x");
    osc3Fat = new BoolParam(false, "Osc3.3x");
    AddParam(osc1Fat);
    AddParam(osc2Fat);
    AddParam(osc3Fat);

    osc1Saw = new BoolParam(false, "Osc1.saw");
    osc1Sine = new BoolParam(true, "Osc1.sine");
    osc1Tri = new BoolParam(false, "Osc1.triangle");
    osc1Pulse = new BoolParam(false, "Osc1.pulse");
    osc1Noise = new BoolParam(false, "Osc1.noise");
    AddParam(osc1Saw);
    AddParam(osc1Sine);
    AddParam(osc1Tri);
    AddParam(osc1Pulse);
    AddParam(osc1Noise);
 
    osc2Saw = new BoolParam(false, "Osc2.saw");
    osc2Sine = new BoolParam(true, "Osc2.sine");
    osc2Tri = new BoolParam(false, "Osc2.triangle");
    osc2Pulse = new BoolParam(false, "Osc2.pulse");
    osc2Noise = new BoolParam(false, "Osc2.noise");
    AddParam(osc2Saw);
    AddParam(osc2Sine);
    AddParam(osc2Tri);
    AddParam(osc2Pulse);
    AddParam(osc2Noise);

    osc3Saw = new BoolParam(false, "Osc3.saw");
    osc3Sine = new BoolParam(true, "Osc3.sine");
    osc3Tri = new BoolParam(false, "Osc3.triangle");
    osc3Pulse = new BoolParam(false, "Osc3.pulse");
    osc3Noise = new BoolParam(false, "Osc3.noise");
    AddParam(osc3Saw);
    AddParam(osc3Sine);
    AddParam(osc3Tri);
    AddParam(osc3Pulse);
    AddParam(osc3Noise);

// 0.059463094 - single semitone
// 0.029731547 - single semitone
    osc1Level = new Parameter("Osc1.level", 1, 0, 1, Param_Default);
    osc1Level->AddValueString(VStr_Percent);
    osc1Detune = new Parameter("Osc1.detune", 0.0f, -0.029731547f, 0.029731547f*2);
    osc1Detune->AddValueString(VStr_Percent);
    osc1Octave = new Parameter("Osc1.octave", -2, -3, 6);
    osc1Octave->SetInterval(1);
    osc1Octave->AddValueString(VStr_Integer);
    osc1Width = new Parameter("Osc1.pwidth", 0.5f, 0.1f, 0.4f, Param_Default);
    osc1Width->AddValueString(VStr_Default);
    AddParam(osc1Level);
    AddParam(osc1Detune);
    AddParam(osc1Octave);
    AddParam(osc1Width);

    osc2Level = new Parameter("Osc2.level", 1, 0, 1, Param_Default);
    osc2Level->AddValueString(VStr_Percent);
    osc2Detune = new Parameter("Osc2.detune", 0.0f, -0.029731547f, 0.029731547f*2);
    osc2Detune->AddValueString(VStr_Percent);
    osc2Octave = new Parameter("Osc2.octave", -1, -3, 6);
    osc2Octave->SetInterval(1);
    osc2Octave->AddValueString(VStr_Integer);
    osc2Width = new Parameter("Osc2.pwidth", 0.5f, 0.1f, 0.4f, Param_Default);
    osc2Width->AddValueString(VStr_Default);
    AddParam(osc2Level);
    AddParam(osc2Detune);
    AddParam(osc2Octave);
    AddParam(osc2Width);

    osc3Level = new Parameter("Osc3.level", 1, 0, 1, Param_Default);
    osc3Level->AddValueString(VStr_Percent);
    osc3Detune = new Parameter("Osc3.detune", 0.0f, -0.029731547f, 0.029731547f*2);
    osc3Detune->AddValueString(VStr_Percent);
    osc3Octave = new Parameter("Osc3.octave", 0, -3, 6);
    osc3Octave->SetInterval(1);
    osc3Octave->AddValueString(VStr_Integer);
    osc3Width = new Parameter("Osc3.pwidth", 0.5f, 0.1f, 0.4f, Param_Default);
    osc3Width->AddValueString(VStr_Default);
    AddParam(osc3Level);
    AddParam(osc3Detune);
    AddParam(osc3Octave);
    AddParam(osc3Width);

    filt1Osc1 = new BoolParam(true, "Flt1.Osc1");
    filt1Osc2 = new BoolParam(false, "Flt1.Osc2");
    filt1Osc3 = new BoolParam(false, "Flt1.Osc3");
    AddParam(filt1Osc1);
    AddParam(filt1Osc2);
    AddParam(filt1Osc3);

    filt2Osc1 = new BoolParam(false, "Flt2.Osc1");
    filt2Osc2 = new BoolParam(true, "Flt2.Osc2");
    filt2Osc3 = new BoolParam(false, "Flt2.Osc3");
    AddParam(filt2Osc1);
    AddParam(filt2Osc2);
    AddParam(filt2Osc3);

    chorusON = new BoolParam(false, "Chorus ON");
    delayON = new BoolParam(false, "Delay ON");
    reverbON = new BoolParam(false, "Reverb ON");
    AddParam(chorusON);
    AddParam(delayON);
    AddParam(reverbON);

    LFOAmp_Vol1 = new Parameter("LFOAmp.Osc1.level", 0, 0, 1, Param_Default);
    LFOAmp_Vol1->AddValueString(VStr_Percent);
    LFORate_Vol1 = new Parameter("LFORate.Osc1.level", 3.0f, 0, 20, Param_Default);
    LFORate_Vol1->AddValueString(VStr_fHz1);
    LFOAmp_Vol2 = new Parameter("LFOAmp.Osc2.level", 0, 0, 1, Param_Default);
    LFOAmp_Vol2->AddValueString(VStr_Percent);
    LFORate_Vol2 = new Parameter("LFORate.Osc2.level", 3.0f, 0, 20, Param_Default);
    LFORate_Vol2->AddValueString(VStr_fHz1);
    LFOAmp_Vol3 = new Parameter("LFOAmp.Osc3.level", 0, 0, 1, Param_Default);
    LFOAmp_Vol3->AddValueString(VStr_Percent);
    LFORate_Vol3 = new Parameter("LFORate.Osc3.level", 3.0f, 0, 20, Param_Default);
    LFORate_Vol3->AddValueString(VStr_fHz1);
    AddParam(LFOAmp_Vol1);
    AddParam(LFORate_Vol1);
    AddParam(LFOAmp_Vol2);
    AddParam(LFORate_Vol2);
    AddParam(LFOAmp_Vol3);
    AddParam(LFORate_Vol3);

    LFOAmp_Pitch1 = new Parameter("LFOAmp.Osc1.pitch", 0, 0, 0.059463094f, Param_Default);
    LFOAmp_Pitch1->AddValueString(VStr_Percent);
    LFORate_Pitch1 = new Parameter("LFORate.Osc1.pitch", 3.0f, 0, 20, Param_Default);
    LFORate_Pitch1->AddValueString(VStr_fHz1);
    LFOAmp_Pitch2 = new Parameter("LFOAmp.Osc2.pitch", 0, 0, 0.059463094f, Param_Default);
    LFOAmp_Pitch2->AddValueString(VStr_Percent);
    LFORate_Pitch2 = new Parameter("LFORate.Osc2.pitch", 3.0f, 0, 20, Param_Default);
    LFORate_Pitch2->AddValueString(VStr_fHz1);
    LFOAmp_Pitch3 = new Parameter("LFOAmp.Osc3.pitch", 0, 0, 0.059463094f, Param_Default);
    LFOAmp_Pitch3->AddValueString(VStr_Percent);
    LFORate_Pitch3 = new Parameter("LFORate.Osc3.pitch", 3.0f, 0, 20, Param_Default);
    LFORate_Pitch3->AddValueString(VStr_fHz1);
    AddParam(LFOAmp_Pitch1);
    AddParam(LFORate_Pitch1);
    AddParam(LFOAmp_Pitch2);
    AddParam(LFORate_Pitch2);
    AddParam(LFOAmp_Pitch3);
    AddParam(LFORate_Pitch3);

    LFOAmp_Flt1Freq = new Parameter("LFOAmp.Flt1.freq", 0, 0, 1, Param_Default);
    LFOAmp_Flt1Freq->AddValueString(VStr_Percent);
    LFORate_Flt1Freq = new Parameter("LFORate.Flt1.freq", 3.0f, 0, 20, Param_Default);
    LFORate_Flt1Freq->AddValueString(VStr_fHz1);
    LFOAmp_Flt2Freq = new Parameter("LFOAmp.Flt2.freq", 0, 0, 1, Param_Default);
    LFOAmp_Flt2Freq->AddValueString(VStr_Percent);
    LFORate_Flt2Freq = new Parameter("LFORate.Flt2.freq", 3.0f, 0, 20, Param_Default);
    LFORate_Flt2Freq->AddValueString(VStr_fHz1);
    AddParam(LFOAmp_Flt1Freq);
    AddParam(LFORate_Flt1Freq);
    AddParam(LFOAmp_Flt2Freq);
    AddParam(LFORate_Flt2Freq);

    osc1FM2Level = new Parameter("Osc1.FM.Osc2", 0.0f, 0, 1.0f, Param_Default);
    osc1FM2Level->AddValueString(VStr_Percent);
    osc2FM3Level = new Parameter("Osc2.FM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc2FM3Level->AddValueString(VStr_Percent);
    osc1FM3Level = new Parameter("Osc1.FM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc1FM3Level->AddValueString(VStr_Percent);
    AddParam(osc1FM2Level);
    AddParam(osc2FM3Level);
    AddParam(osc1FM3Level);

    osc1RM2Level = new Parameter("Osc1.RM.Osc2", 0.0f, 0, 1.0f, Param_Default);
    osc1RM2Level->AddValueString(VStr_Percent);
    osc2RM3Level = new Parameter("Osc2.RM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc2RM3Level->AddValueString(VStr_Percent);
    osc1RM3Level = new Parameter("Osc1.RM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc1RM3Level->AddValueString(VStr_Percent);
    AddParam(osc1RM2Level);
    AddParam(osc2RM3Level);
    AddParam(osc1RM3Level);

    // Init filters
    for(int fc = 0; fc <= maxVoices; fc++)
    {
        filt1[fc] = new Filter1(NULL, NULL);
        filt1[fc]->scope.instr = this;
        if(fc > 0)
        {
            filt1[fc - 1]->f_next = filt1[fc];
        }

        filt2[fc] = new Filter1(NULL, NULL);
        filt2[fc]->scope.instr = this;
        if(fc > 0)
        {
            filt2[fc - 1]->f_next = filt2[fc];
        }

        if(fc == 0)
        {
            AddChild(filt1[fc]);
            AddChild(filt2[fc]);
        }
    }
    freeVoice = maxVoices;

    filt1[0]->f_master = true;
    filt2[0]->f_master = true;

    filt1[0]->SetName("SimpleFilter");
    filt2[0]->SetName("Filter2");

    // Init other FX
    chorus = new CChorus(NULL, NULL);
    chorus->scope.instr = this;
    delay = new XDelay(NULL, NULL);
    delay->scope.instr = this;
    delay->delay->SetInterval(0.25f);
    reverb = new CReverb(NULL, NULL);
    reverb->scope.instr = this;

    AddChild(chorus);
    AddChild(delay);
    AddChild(reverb);

    osc1fmulStatic = osc2fmulStatic = osc3fmulStatic = 1;

    synthWin = NULL;
    wnd = NULL;
    SynthComp = NULL;

    env1 = CreateEnvelope();
    env2 = CreateEnvelope();
    env3 = CreateEnvelope();
    env4 = CreateEnvelope();
    env5 = CreateEnvelope();

    envV1 = envV2 = envV3 = envV4 = envV5 = 1;

    p_osc1envVal = &envV1;
    p_osc2envVal = &envV2;
    p_osc3envVal = &envV3;

    p_flt1envVal = ebuff4;
    p_flt2envVal = ebuff5;

    rnd = new Random(0);

    Preset preset(NULL);
    strcpy(preset.name, "Square lead");
    strcpy(preset.path, ".\\Presets\\Chaotic\\Square lead.cxml");
    preset.native = true;
    File f(preset.path);
    if(f.exists())
    {
        SetPreset(&preset);
    }

    CreateModuleWindow();
}

Synth::~Synth()
{
}

Envelope* Synth::CreateEnvelope()
{
    Envelope* env = new Envelope(Cmd_ParamEnv);
    env->len = 3;
    env->timebased = true;

    env->AddPoint(0.0f, 1.0f);
    env->AddPoint(0.2f, 1.0f);
    env->AddPoint(0.3f, 0.5f);
    env->SetSustainPoint(env->AddPoint(0.6f, 0.0f));
    env->sustainable = true;

    return env;
}

void Synth::CreateModuleWindow()
{
    synthWin = MainWnd->CreateSynthWindow(this);
    wnd = synthWin;
    SynthComp = (SynthComponent*)synthWin->getContentComponent();
    SynthComp->UpdateEnvButts();
}

void Synth::Save(XmlElement * instrNode)
{
    Instrument::save(instrNode);
    SaveStateData(*instrNode, "Current", true);
}

void Synth::Load(XmlElement * instrNode)
{
    Instrument::load(instrNode);

    XmlElement* stateNode = instrNode->getChildByName(T("Module"));
    if(stateNode != NULL)
    {
        RestoreStateData(*stateNode, true);
    }
}

void Synth::ResetValues()
{
    osc1Fat->SetBoolValue(false);
    osc2Fat->SetBoolValue(false);
    osc3Fat->SetBoolValue(false);

    osc1Sine->SetBoolValue(true);
    osc1Saw->SetBoolValue(false);
    osc1Tri->SetBoolValue(false);
    osc1Pulse->SetBoolValue(false);
    osc1Noise->SetBoolValue(false);

    osc1Detune->SetNormalValue(0);
    osc1Octave->SetNormalValue(0);
    osc1Width->SetNormalValue(0.5f);

    osc2Sine->SetBoolValue(true);
    osc2Saw->SetBoolValue(false);
    osc2Tri->SetBoolValue(false);
    osc2Pulse->SetBoolValue(false);
    osc2Noise->SetBoolValue(false);

    osc2Detune->SetNormalValue(0);
    osc2Octave->SetNormalValue(0);
    osc2Width->SetNormalValue(0.5f);

    osc3Sine->SetBoolValue(true);
    osc3Saw->SetBoolValue(false);
    osc3Tri->SetBoolValue(false);
    osc3Pulse->SetBoolValue(false);
    osc3Noise->SetBoolValue(false);

    osc3Detune->SetNormalValue(0);
    osc3Octave->SetNormalValue(0);
    osc3Width->SetNormalValue(0.5f);

    osc1FM2Level->SetNormalValue(0);
    osc2FM3Level->SetNormalValue(0);
    osc1FM3Level->SetNormalValue(0);

    osc1RM2Level->SetNormalValue(0);
    osc2RM3Level->SetNormalValue(0);
    osc1RM3Level->SetNormalValue(0);

    LFOAmp_Vol1->SetNormalValue(0);
    LFOAmp_Vol2->SetNormalValue(0);
    LFOAmp_Vol3->SetNormalValue(0);
    LFORate_Vol1->SetNormalValue(3);
    LFORate_Vol2->SetNormalValue(3);
    LFORate_Vol3->SetNormalValue(3);

    LFOAmp_Pitch1->SetNormalValue(0);
    LFOAmp_Pitch2->SetNormalValue(0);
    LFOAmp_Pitch3->SetNormalValue(0);
    LFORate_Pitch1->SetNormalValue(3);
    LFORate_Pitch2->SetNormalValue(3);
    LFORate_Pitch3->SetNormalValue(3);

    LFOAmp_Flt1Freq->SetNormalValue(0);
    LFOAmp_Flt2Freq->SetNormalValue(0);
    LFORate_Flt1Freq->SetNormalValue(3);
    LFORate_Flt2Freq->SetNormalValue(3);

    filt1Osc1->SetBoolValue(true);
    filt1Osc2->SetBoolValue(false);
    filt1Osc3->SetBoolValue(false);

    filt2Osc1->SetBoolValue(false);
    filt2Osc2->SetBoolValue(false);
    filt2Osc3->SetBoolValue(false);

    filt1[0]->f1->SetBoolValue(true);
    filt1[0]->x2->SetBoolValue(false);
    filt1[0]->cutoff->SetNormalValue(1);
    filt1[0]->resonance->SetNormalValue(0);

    filt2[0]->f1->SetBoolValue(true);
    filt2[0]->x2->SetBoolValue(false);
    filt2[0]->cutoff->SetNormalValue(1);
    filt2[0]->resonance->SetNormalValue(0);

    chorusON->SetBoolValue(false);
    delayON->SetBoolValue(false);
    reverbON->SetBoolValue(false);

    chorus->drywet->Reset();
    chorus->depth->Reset();
    chorus->delay->Reset();

    delay->ggain->Reset();
    delay->feedback->Reset();
    delay->delay->Reset();

    reverb->drywet->Reset();
    reverb->decay->Reset();
    reverb->lowCut->Reset();

    p_osc1envVal = &envV1;
    p_osc2envVal = &envV2;
    p_osc3envVal = &envV3;

    p_flt1envVal = ebuff4;
    p_flt2envVal = ebuff5;

    if(SynthComp != NULL)
    {
        SynthComp->env1->env->Clean();
        SynthComp->env1->env->SetSustainPoint(SynthComp->env1->env->AddPoint(0, 1));
        SynthComp->env2->env->Clean();
        SynthComp->env2->env->SetSustainPoint(SynthComp->env2->env->AddPoint(0, 1));
        SynthComp->env3->env->Clean();
        SynthComp->env3->env->SetSustainPoint(SynthComp->env3->env->AddPoint(0, 1));
        SynthComp->env4->env->Clean();
        SynthComp->env4->env->SetSustainPoint(SynthComp->env4->env->AddPoint(0, 1));
        SynthComp->env5->env->Clean();
        SynthComp->env5->env->SetSustainPoint(SynthComp->env5->env->AddPoint(0, 1));

        SynthComp->UpdateEnvButts();

        SynthComp->repaint();
    }
}

void Synth::ActivateTrigger(Trigger* tg)
{
    int vnum = 0;
    if(numUsedVoices == maxVoices)
    {
		vnum = tg_first->voicenum;
        if(tg_first->frame_phase > 0)
        {
            FlowTriggers(tg_first, tg);
        }
        else if(tg_first->lcount > 0)
		{
            memcpy(tg->auxbuff, tg_first->auxbuff, (int)(tg_first->lcount*sizeof(float)*2));
			tg->lcount = tg_first->lcount;
			tg_first->lcount = 0;
		}

		if(tg_first != NULL)
            tg_first->Deactivate();
    }
    else
    {
        // Check and loop until first unused number is found
        Trigger* tgc = tg_first;
        while(tgc != NULL)
        {
            if(tgc->voicenum == vnum)
            {
                vnum++;
                tgc = tg_first;
                continue;
            }
            tgc = tgc->loc_act_next;
        }
    }
    tg->voicenum = vnum;
    numUsedVoices++;
    jassert(numUsedVoices <= maxVoices + 1);
    Generator::activateTrigger(tg);

    tg->sec_phase1 = 0;
    tg->sec_phase2 = 0;
    tg->sec_phase3 = 0;

    tg->aaCount = DECLICK_COUNT;
    tg->aaIN = true;

    tg->ep1 = env1->p_first;
    tg->ep2 = env2->p_first;
    tg->ep3 = env3->p_first;
    tg->ep4 = env4->p_first;
    tg->ep5 = env5->p_first;
    tg->envVal1 = tg->ep1->y_norm;
    tg->envVal2 = tg->ep2->y_norm;
    tg->envVal3 = tg->ep3->y_norm;
    tg->envVal4 = tg->ep4->y_norm;
    tg->envVal5 = tg->ep5->y_norm;

    tg->env_phase1 = 0;
    tg->env_phase2 = 0;
    tg->env_phase3 = 0;
    tg->env_phase4 = 0;
    tg->env_phase5 = 0;

    //filt1[vnum]->Reset();
    //filt2[vnum]->Reset();
}

void Synth::DeactivateTrigger(Trigger* tg)
{
    // Reset on deactivate causes aliasing in filters
    //filt1[tg->voicenum]->Reset();
    //filt2[tg->voicenum]->Reset();
    numUsedVoices--;
    jassert(numUsedVoices >= 0);
    Instrument::deactivateTrigger(tg);
}

long Synth::ProcessTrigger(Trigger * tg, long num_frames, long buffframe)
{
    fill = true;
    skip = false;

    tg->freq_incr_active = tg->freq;
    tg->vol_val = tg->vol_base;
    tg->pan_val = tg->pan_base;

    PreProcessTrigger(tg, &skip, &fill, num_frames, buffframe);

    if(!skip)
    {
        memset(fltbuff1, 0, num_frames*(sizeof(float))*2);
        memset(fltbuff2, 0, num_frames*(sizeof(float))*2);

        //memset(fltbuff1_out, 0, num_frames*(sizeof(float))*2);
        //memset(fltbuff2_out, 0, num_frames*(sizeof(float))*2);
        long tcf0 = 0;
        long tc0 = buffframe*2;
        bool f1work = filt1Osc1->outval || filt1Osc2->outval || filt1Osc3->outval;
        bool f2work = filt2Osc1->outval || filt2Osc2->outval || filt2Osc3->outval;
        //bool o1work = filt1Osc1->outval || filt2Osc1->outval;
        //bool o2work = filt1Osc2->outval || filt2Osc2->outval;
        //bool o3work = filt1Osc3->outval || filt2Osc3->outval;
        double inOutL1, inOutL2, inOutR1, inOutR2;
        inOutL1 = inOutL2 = inOutR1 = inOutR2 = 0;
        double f1freqnormval = filt1[0]->cutoff->val;
        double f2freqnormval = filt2[0]->cutoff->val;
        long ic = 0;
        int fqc = 0;

        int bc = buffframe*2;
        GenNote* gnote = (GenNote*)tg->el;
        float d1, d2, d3, v1, v2, v3;
        int vnum = tg->voicenum;
        //if(fill)
        {
            for(int ic = 0; ic < num_frames; ic++)
            {
                envV1 = tg->envVal1;
                envV2 = tg->envVal2;
                envV3 = tg->envVal3;
                envV4 = tg->envVal4;
                envV5 = tg->envVal5;
                d1 = 0;
                if(osc1Tri->outval)
                {
                    d1 = wt_triangle[int(tg->sec_phase1*WAVETABLE_SIZE)&0x7FFF];
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d1 += wt_triangle[int(f1*WAVETABLE_SIZE)&0x7FFF];
                        d1 += wt_triangle[int(f2*WAVETABLE_SIZE)&0x7FFF];
                    }
                }
                else if(osc1Sine->outval)
                {
                    d1 = wt_sine[int(tg->sec_phase1*WAVETABLE_SIZE)&0x7FFF];
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d1 += wt_sine[int(f1*WAVETABLE_SIZE)&0x7FFF];
                        d1 += wt_sine[int(f2*WAVETABLE_SIZE)&0x7FFF];
                    }
                }
                else if(osc1Saw->outval)
                {
                    d1 = float(tg->sec_phase1*2 - 1);
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d1 += float(f1*2 - 1);
                        d1 += float(f2*2 - 1);
                    }
                }
                else if(osc1Pulse->outval)
                {
                    d1 = tg->sec_phase1 < osc1Width->outval ? -1.f : 1.f;
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d1 += f1 < osc1Width->outval ? -1 : 1;
                        d1 += f2 < osc1Width->outval ? -1 : 1;
                    }
                }
                else if(osc1Noise->outval)
                {
                    d1 = rnd->nextFloat()*2 -1;
                }

                v1 = osc1Level->outval;

                v1 *= 1 - LFOAmp_Vol1->outval*(0.5f*(1 + wt_sine[int(LFORate_Vol1->outval*(tg->sec_phase)*WAVETABLE_SIZE)&0x7FFF]));
                v1 *= *p_osc1envVal;

                data_buff[bc] = data_buff[bc + 1] = d1*(v1*v1 + (1 - pow((1 - v1), 0.666666666f)))*0.5f;
                osc1fmulLFO = 1 + LFOAmp_Pitch1->outval*wt_sine[int(tg->sec_phase*LFORate_Pitch1->outval*WAVETABLE_SIZE)&0x7FFF];

                tg->sec_phase1 += one_divided_per_sample_rate*tg->freq_incr_active*osc1fmulStatic*osc1fmulLFO;
                if(tg->sec_phase1 >= 1)
                {
                    tg->sec_phase1 -= 1;
                }

                //////////
                // 2nd
                osc2fmulFM = osc1FM2Level->outval*d1*v1;

                d2 = 0;
                if(osc2Tri->outval)
                {
                    d2 = wt_triangle[int((tg->sec_phase2 + osc2fmulFM)*WAVETABLE_SIZE)&0x7FFF];
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d2 += wt_triangle[int(f1*WAVETABLE_SIZE)&0x7FFF];
                        d2 += wt_triangle[int(f2*WAVETABLE_SIZE)&0x7FFF];
                    }
                }
                else if(osc2Sine->outval)
                {
                    //d1 = sin(tg->sec_phase2*2.0f*PI*osc2fmulStatic*osc2fmuDynamic);
                    //d1 = sin(tg->sec_phase2*2.0f*PI);
                    d2 = wt_sine[int((tg->sec_phase2 + osc2fmulFM)*WAVETABLE_SIZE)&0x7FFF];
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d2 += wt_sine[int(f1*WAVETABLE_SIZE)&0x7FFF];
                        d2 += wt_sine[int(f2*WAVETABLE_SIZE)&0x7FFF];
                    }
                }
                else if(osc2Saw->outval)
                {
                    d2 = float(tg->sec_phase2 + osc2fmulFM)*2 - 1;
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d2 += float(f1*2 - 1);
                        d2 += float(f2*2 - 1);
                    }
                }
                else if(osc2Pulse->outval)
                {
                    d2 = (tg->sec_phase2 + osc2fmulFM) < osc2Width->outval ? -1.f : 1.f;
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d2 += f1 < osc2Width->outval ? -1 : 1;
                        d2 += f2 < osc2Width->outval ? -1 : 1;
                    }
                }
                else if(osc2Noise->outval)
                {
                    d2 = rnd->nextFloat()*2 - 1;
                }

                // Apply RM
                d2 *= 1.f - osc1RM2Level->outval*(1.f - d1*v1);
                // Apply Level
                v2 = osc2Level->outval;
                v2 *= 1 - LFOAmp_Vol2->outval*(0.5f*(1 + wt_sine[int(LFORate_Vol2->outval*(tg->sec_phase)*WAVETABLE_SIZE)&0x7FFF]));
                v2 *= *p_osc2envVal;
                data_buff2[bc] = data_buff2[bc + 1] = d2*(v2*v2 + (1 - pow((1 - v2), 1.f/1.5f)))*0.5f;
                // Increase phase
                osc2fmulLFO = 1 + LFOAmp_Pitch2->outval*wt_sine[int(tg->sec_phase*LFORate_Pitch2->outval*WAVETABLE_SIZE)&0x7FFF];
                tg->sec_phase2 += one_divided_per_sample_rate*tg->freq_incr_active*osc2fmulStatic*osc2fmulLFO;
                if(tg->sec_phase2 >= 1)
                {
                    tg->sec_phase2 -= 1;
                }

                //////////
                // 3rd
                osc3fmulFM = osc2FM3Level->outval*d2*v2;
                osc3fmulFM += osc1FM3Level->outval*d1*v1;

                d3 = 0;
                if(osc3Tri->outval)
                {
                    d3 = wt_triangle[int((tg->sec_phase3 + osc3fmulFM)*WAVETABLE_SIZE)&0x7FFF];
                    if(osc3Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d3 += wt_triangle[int(f1*WAVETABLE_SIZE)&0x7FFF];
                        d3 += wt_triangle[int(f2*WAVETABLE_SIZE)&0x7FFF];
                    }
                }
                else if(osc3Sine->outval)
                {
                    //d1 = sin(tg->sec_phase2*2.0f*PI*osc2fmulStatic*osc2fmuDynamic);
                    //d1 = sin(tg->sec_phase2*2.0f*PI);
                    d3 = wt_sine[int((tg->sec_phase3 + osc3fmulFM)*WAVETABLE_SIZE)&0x7FFF];
                    if(osc3Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d3 += wt_sine[int(f1*WAVETABLE_SIZE)&0x7FFF];
                        d3 += wt_sine[int(f2*WAVETABLE_SIZE)&0x7FFF];
                    }
                }
                else if(osc3Saw->outval)
                {
                    d3 = float(tg->sec_phase3 + osc3fmulFM)*2 - 1;
                    if(osc3Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d3 += float(f1*2 - 1);
                        d3 += float(f2*2 - 1);
                    }
                }
                else if(osc3Pulse->outval)
                {
                    d3 = (tg->sec_phase3 + osc3fmulFM) < osc3Width->outval ? -1.f : 1.f;
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d3 += f1 < osc3Width->outval ? -1 : 1;
                        d3 += f2 < osc3Width->outval ? -1 : 1;
                    }
                }
                else if(osc3Noise->outval)
                {
                    d3 = rnd->nextFloat()*2 - 1;
                }

                d3 *= 1 - osc1RM3Level->outval*(1 - d1*v1);
                d3 *= 1 - osc2RM3Level->outval*(1 - d2*v2);

                v3 = osc3Level->outval;
                v3 *= 1 - LFOAmp_Vol3->outval*(0.5f*(1 + wt_sine[int(LFORate_Vol3->outval*(tg->sec_phase)*WAVETABLE_SIZE)&0x7FFF]));
                v3 *= *p_osc3envVal;
                data_buff3[bc] = data_buff3[bc + 1] = d3*(v3*v3 + (1 - pow((1 - v3), 0.666666666f)))*0.5f;
                osc3fmulLFO = 1;
                osc3fmulLFO = 1 + LFOAmp_Pitch3->outval*wt_sine[int(tg->sec_phase*LFORate_Pitch3->outval*WAVETABLE_SIZE)&0x7FFF];
                tg->sec_phase3 += one_divided_per_sample_rate*tg->freq_incr_active*osc3fmulStatic*osc3fmulLFO;

                if(tg->sec_phase3 >= 1)
                {
                    tg->sec_phase3 -= 1;
                }

                bc += 2;
                tg->wt_pos += tg->freq_incr_active;

                tg->sec_phase += one_divided_per_sample_rate;

                if(tg->tgstate == TS_Sustain && tg->ep1->suspoint)
                {
                    tg->env_phase1 = tg->ep1->x;
                    tg->envVal1 = tg->ep1->y_norm;
                }
                else
                {
                    tg->env_phase1 += one_divided_per_sample_rate;

                    while(tg->ep1->next != NULL && (tg->env_phase1 >= tg->ep1->next->x || tg->ep1->next->x == tg->ep1->x))
                    {
                        tg->ep1 = tg->ep1->next;
                    }

                    if(tg->ep1->next != NULL)
                    {
                        tg->envVal1 += tg->ep1->cff;

                        if(tg->envVal1 > 1)
                        	tg->envVal1 = 1;
                        else if(tg->envVal1 < 0)
                        	tg->envVal1 = 0;

                        //tg->envVal1 = (float)(tg->ep1->y_norm + (tg->env_phase1 - tg->ep1->x)*((tg->ep1->next->y_norm - tg->ep1->y_norm)/(tg->ep1->next->x - tg->ep1->x)));
                    }
                }
                ebuff1[buffframe + ic] = tg->envVal1;

                if(tg->tgstate == TS_Sustain && tg->ep2->suspoint)
                {
                    tg->env_phase2 = tg->ep2->x;
                    tg->envVal2 = tg->ep2->y_norm;
                }
                else
                {
                    tg->env_phase2 += one_divided_per_sample_rate;
                    while(tg->ep2->next != NULL && (tg->env_phase2 >= tg->ep2->next->x || tg->ep2->next->x == tg->ep2->x))
                    {
                        tg->ep2 = tg->ep2->next;
                    }
                    if(tg->ep2->next != NULL)
                    {
                        tg->envVal2 += tg->ep2->cff;
						if(tg->envVal2 > 1)
							tg->envVal2 = 1;
						else if(tg->envVal2 < 0)
							tg->envVal2 = 0;
                        //tg->envVal2 = (float)(tg->ep2->y_norm + (tg->env_phase2 - tg->ep2->x)*((tg->ep2->next->y_norm - tg->ep2->y_norm)/(tg->ep2->next->x - tg->ep2->x)));
                    }
                }
                ebuff2[buffframe + ic] = tg->envVal2;

                if(tg->tgstate == TS_Sustain && tg->ep3->suspoint)
                {
                    tg->env_phase3 = tg->ep3->x;
                    tg->envVal3 = tg->ep3->y_norm;
                }
                else
                {
                    tg->env_phase3 += one_divided_per_sample_rate;
                    while(tg->ep3->next != NULL && (tg->env_phase3 >= tg->ep3->next->x || tg->ep3->next->x == tg->ep3->x))
                    {
                        tg->ep3 = tg->ep3->next;
                    }
                    if(tg->ep3->next != NULL)
                    {
                        tg->envVal3 += tg->ep3->cff;
						if(tg->envVal3 > 1)
							tg->envVal3 = 1;
						else if(tg->envVal3 < 0)
							tg->envVal3 = 0;
                        //tg->envVal3 = (float)(tg->ep3->y_norm + (tg->env_phase3 - tg->ep3->x)*((tg->ep3->next->y_norm - tg->ep3->y_norm)/(tg->ep3->next->x - tg->ep3->x)));
                    }
                }
                ebuff3[buffframe + ic] = tg->envVal3;

                if(tg->tgstate == TS_Sustain && tg->ep4->suspoint)
                {
                    tg->env_phase4 = tg->ep4->x;
                    tg->envVal4 = tg->ep4->y_norm;
                }
                else
                {
                    tg->env_phase4 += one_divided_per_sample_rate;
                    while(tg->ep4->next != NULL && (tg->env_phase4 >= tg->ep4->next->x || tg->ep4->next->x == tg->ep4->x))
                    {
                        tg->ep4 = tg->ep4->next;
                    }
                    if(tg->ep4->next != NULL)
                    {
                        tg->envVal4 += tg->ep4->cff;
						if(tg->envVal4 > 1)
							tg->envVal4 = 1;
						else if(tg->envVal4 < 0)
							tg->envVal4 = 0;
                        //tg->envVal4 = (float)(tg->ep4->y_norm + (tg->env_phase4 - tg->ep4->x)*((tg->ep4->next->y_norm - tg->ep4->y_norm)/(tg->ep4->next->x - tg->ep4->x)));
                    }
                }
                ebuff4[buffframe + ic] = tg->envVal4;

                if(tg->tgstate == TS_Sustain && tg->ep5->suspoint)
                {
                    tg->env_phase5 = tg->ep5->x;
                    tg->envVal5 = tg->ep5->y_norm;
                }
                else
                {
                    tg->env_phase5 += one_divided_per_sample_rate;
                    while(tg->ep5->next != NULL && (tg->env_phase5 >= tg->ep5->next->x || tg->ep5->next->x == tg->ep5->x))
                    {
                        tg->ep5 = tg->ep5->next;
                    }
                    if(tg->ep5->next != NULL)
                    {
                        tg->envVal5 += tg->ep5->cff;
						if(tg->envVal5 > 1)
							tg->envVal5 = 1;
						else if(tg->envVal5 < 0)
							tg->envVal5 = 0;
                        //tg->envVal5 = (float)(tg->ep5->y_norm + (tg->env_phase5 - tg->ep5->x)*((tg->ep5->next->y_norm - tg->ep5->y_norm)/(tg->ep5->next->x - tg->ep5->x)));
                    }
                }
                ebuff5[buffframe + ic] = tg->envVal5;


                if(f1work)
                {
                    fltlfo1[buffframe + ic] = 1 + LFOAmp_Flt1Freq->outval*(wt_sine[int(LFORate_Flt1Freq->outval*(tg->sec_phase)*WAVETABLE_SIZE)&0x7FFF]);

                    if(filt1Osc1->outval)
                    {
                        fltbuff1[tcf0] += data_buff[tc0];
                        fltbuff1[tcf0 + 1] += data_buff[tc0 + 1];
                    }

                    if(filt1Osc2->outval)
                    {
                        fltbuff1[tcf0] += data_buff2[tc0];
                        fltbuff1[tcf0 + 1] += data_buff2[tc0 + 1];
                    }

                    if(filt1Osc3->outval)
                    {
                        fltbuff1[tcf0] += data_buff3[tc0];
                        fltbuff1[tcf0 + 1] += data_buff3[tc0 + 1];
                    }

                    if(fqc == 0)
                    {
                        //flt1eV[buffframe + ic] = 1;  // debug stuff
                        filt1[vnum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f1freqnormval*p_flt1envVal[buffframe + ic]*fltlfo1[buffframe + ic])));
                    }
                    inOutL1 = (double)fltbuff1[tcf0];
                    inOutR1 = (double)fltbuff1[tcf0 + 1];
                    filt1[vnum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL1, &inOutR1);
                }

                if(f2work)
                {
                    fltlfo2[buffframe + ic] = 1 + LFOAmp_Flt2Freq->outval*(wt_sine[int(LFORate_Flt2Freq->outval*(tg->sec_phase)*WAVETABLE_SIZE)&0x7FFF]);

                    if(filt2Osc1->outval)
                    {
                        fltbuff2[tcf0] += data_buff[tc0];
                        fltbuff2[tcf0 + 1] += data_buff[tc0 + 1];
                    }

                    if(filt2Osc2->outval)
                    {
                        fltbuff2[tcf0] += data_buff2[tc0];
                        fltbuff2[tcf0 + 1] += data_buff2[tc0 + 1];
                    }

                    if(filt2Osc3->outval)
                    {
                        fltbuff2[tcf0] += data_buff3[tc0];
                        fltbuff2[tcf0 + 1] += data_buff3[tc0 + 1];
                    }

                    if(fqc == 0)
                    {
                        //flt2eV[buffframe + ic] = 1;  // debug stuff
                        filt2[vnum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f2freqnormval*p_flt2envVal[buffframe + ic]*fltlfo2[buffframe + ic])));
                    }
                    inOutL2 = (double)fltbuff2[tcf0];
                    inOutR2 = (double)fltbuff2[tcf0 + 1];
                    filt2[vnum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL2, &inOutR2);
                }

				if(fill)
				{
					in_buff[tc0] = (float)inOutL1 + (float)inOutL2;
					in_buff[tc0 + 1] = (float)inOutR1 + (float)inOutR2;
				}
				else
				{
					in_buff[tc0] = 0;
					in_buff[tc0 + 1] = 0;
				}

                tc0++;
                tc0++;

                tcf0++;
                tcf0++;

                fqc++;
                if(fqc == 77)
                    fqc = 0;

                //if(tg->lcount > 0)
                //    tg->lcount--;
            }
        }
        /*
        else
        {
            tg->wt_pos += tg->freq_incr_active*num_frames;
            //while(tg->wt_pos >= 1.0f)
            //{
            //    tg->wt_pos -= 1.0f;
            //}
            tg->sec_phase += one_divided_per_sample_rate*num_frames;
            //while(tg->sec_phase >= 1.0f)
            //{
            //    tg->sec_phase -= 1.0f;
            //}
        }
        */

        tg->frame_phase += num_frames;
        CheckBounds(gnote, tg, num_frames);
    }

    return num_frames;
}
void Synth::FillMixChannel(long num_frames, long buffframe, long mixbuffframe)
{
    if(chorusON->outval == true)
    {
        chorus->ProcessData(&out_buff[buffframe*2], &out_buff[buffframe*2], num_frames);
    }

    if(delayON->outval == true)
    {
        delay->ProcessData(&out_buff[buffframe*2], &out_buff[buffframe*2], num_frames);
    }

    if(reverbON->outval == true)
    {
        reverb->ProcessData(&out_buff[buffframe*2], &out_buff[buffframe*2], num_frames);
    }

    Instrument::fillMixChannel(num_frames, buffframe, mixbuffframe);
}

// data_buff >> in_buff
void Synth::ApplyDSP(Trigger* tg, long buffframe, long num_frames)
{
    memset(fltbuff1, 0, num_frames*(sizeof(float))*2);
    memset(fltbuff2, 0, num_frames*(sizeof(float))*2);
    memset(fltbuff1_out, 0, num_frames*(sizeof(float))*2);
    memset(fltbuff2_out, 0, num_frames*(sizeof(float))*2);

    long tcf0 = 0;
    long tc0 = buffframe*2;
    bool f1work = filt1Osc1->outval || filt1Osc2->outval || filt1Osc3->outval;
    bool f2work = filt2Osc1->outval || filt2Osc2->outval || filt2Osc3->outval;
    double inOutL;
    double inOutR;
    double f1freqnormval = filt1[0]->cutoff->val;
    double f2freqnormval = filt2[0]->cutoff->val;
    long ic = 0;
    int fqc = 0;
    for(long cc = 0; cc < num_frames; cc++)
    {
        if(filt1Osc1->outval)
        {
            fltbuff1[tcf0] += data_buff[tc0];
            fltbuff1[tcf0 + 1] += data_buff[tc0 + 1];
        }

        if(filt1Osc2->outval)
        {
            fltbuff1[tcf0] += data_buff2[tc0];
            fltbuff1[tcf0 + 1] += data_buff2[tc0 + 1];
        }

        if(filt1Osc3->outval)
        {
            fltbuff1[tcf0] += data_buff3[tc0];
            fltbuff1[tcf0 + 1] += data_buff3[tc0 + 1];
        }

        if(f1work)
        {
            if(fqc == 0)
                filt1[tg->voicenum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f1freqnormval*p_flt1envVal[buffframe + cc]*fltlfo1[buffframe + cc])));
            inOutL = (double)fltbuff1[tcf0];
            inOutR = (double)fltbuff1[tcf0 + 1];
            filt1[tg->voicenum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL, &inOutR);
            fltbuff1_out[tcf0] = (float)inOutL;
            fltbuff1_out[tcf0 + 1] = (float)inOutR;

            in_buff[tc0] = fltbuff1_out[tcf0];
            in_buff[tc0 + 1] = fltbuff1_out[tcf0 + 1];
        }

        if(filt2Osc1->outval)
        {
            fltbuff2[tcf0] += data_buff[tc0];
            fltbuff2[tcf0 + 1] += data_buff[tc0 + 1];
        }

        if(filt2Osc2->outval)
        {
            fltbuff2[tcf0] += data_buff2[tc0];
            fltbuff2[tcf0 + 1] += data_buff2[tc0 + 1];
        }

        if(filt2Osc3->outval)
        {
            fltbuff2[tcf0] += data_buff3[tc0];
            fltbuff2[tcf0 + 1] += data_buff3[tc0 + 1];
        }

        if(f2work)
        {
            if(fqc == 0)
                filt2[tg->voicenum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f2freqnormval*p_flt2envVal[buffframe + cc]*fltlfo2[buffframe + cc])));
            inOutL = (double)fltbuff2[tcf0];
            inOutR = (double)fltbuff2[tcf0 + 1];
            filt2[tg->voicenum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL, &inOutR);
            fltbuff2_out[tcf0] = (float)inOutL;
            fltbuff2_out[tcf0 + 1] = (float)inOutR;

            in_buff[tc0] += fltbuff2_out[tcf0];
            in_buff[tc0 + 1] += fltbuff2_out[tcf0 + 1];
        }

        tc0++;
        tc0++;

        tcf0++;
        tcf0++;
    }
}

void Synth::UpdateOsc1Mul()
{
    osc1fmulStatic = pow(2, osc1Octave->outval)*(1 + osc1Detune->outval);
}

void Synth::UpdateOsc2Mul()
{
    osc2fmulStatic = pow(2, osc2Octave->outval)*(1 + osc2Detune->outval);
}

void Synth::UpdateOsc3Mul()
{
    osc3fmulStatic = pow(2, osc3Octave->outval)*(1 + osc3Detune->outval);
}

bool Synth::IsTriggerAtTheEnd(Trigger* tg)
{
    // Check all three oscillators whether they are sounding and still under envelope. If not, then finish.
    bool attheend = true;
    if(filt1Osc1->outval || filt2Osc1->outval)
    {
        if((p_osc1envVal == &envV1 && tg->ep1->next != NULL) ||
           (p_osc1envVal == &envV2 && tg->ep2->next != NULL) || 
           (p_osc1envVal == &envV3 && tg->ep3->next != NULL) ||
           (p_osc1envVal == &envV4 && tg->ep4->next != NULL) ||
           (p_osc1envVal == &envV5 && tg->ep5->next != NULL))
        {
            attheend = false;
        }
    }

    if(attheend && (filt1Osc2->outval || filt2Osc2->outval))
    {
        if((p_osc2envVal == &envV1 && tg->ep1->next != NULL) ||
           (p_osc2envVal == &envV2 && tg->ep2->next != NULL) || 
           (p_osc2envVal == &envV3 && tg->ep3->next != NULL) ||
           (p_osc2envVal == &envV4 && tg->ep4->next != NULL) ||
           (p_osc2envVal == &envV5 && tg->ep5->next != NULL))
        {
            attheend = false;
        }
    }

    if(attheend && (filt1Osc3->outval || filt2Osc3->outval))
    {
        if((p_osc3envVal == &envV1 && tg->ep1->next != NULL) ||
           (p_osc3envVal == &envV2 && tg->ep2->next != NULL) || 
           (p_osc3envVal == &envV3 && tg->ep3->next != NULL) ||
           (p_osc3envVal == &envV4 && tg->ep4->next != NULL) ||
           (p_osc3envVal == &envV5 && tg->ep5->next != NULL))
        {
            attheend = false;
        }
    }

    return attheend;
}

void Synth::CheckBounds(GenNote* gnote, Trigger* tg, long num_frames)
{
	int vnum = tg->voicenum;
	if(tg->tgstate == TS_Sustain && gnote->preview == false && tg->frame_phase >= gnote->frame_length)
    {
        // Go to release state as note is over
        tg->Release();
    }

    // Finishing can happen both when envelope is out of bounds and when the note length is longer than envelope 
    // length and the note is finished (frame phase exceeds note frame length)
	if(tg->tgstate == TS_Release)
    {

        if(IsTriggerAtTheEnd(tg))
        {
            tg->tgstate = TS_SoftFinish;
            //if(end_frame < 0)
            {
                end_frame = 0;
            }
        }
    }
}

void Synth::ParamUpdate(Parameter* param)
{
    if(param == delayON)
    {
        delay->Reset();
    }
    else if(param == reverbON)
    {
        reverb->Reset();
    }
    else if(param == osc1Level)
    {
        osc1Level->vstring->SetValue(int(osc1Level->outval*100));
    }
    else if(param == osc1Octave)
    {
        osc1Octave->vstring->SetValue(int(osc1Octave->outval));
        UpdateOsc1Mul();
    }
    else if(param == osc1Detune)
    {
        osc1Detune->vstring->SetValue(int(osc1Detune->outval/0.029731547f*100));
        UpdateOsc1Mul();
    }
    else if(param == osc2Level)
    {
        osc2Level->vstring->SetValue(int(osc2Level->outval*100));
    }
    else if(param == osc2Octave)
    {
        osc2Octave->vstring->SetValue(int(osc2Octave->outval));
        UpdateOsc2Mul();
    }
    else if(param == osc2Detune)
    {
        osc2Detune->vstring->SetValue(int(osc2Detune->outval/0.029731547f*100));
        UpdateOsc2Mul();
    }
    else if(param == osc3Level)
    {
        osc3Level->vstring->SetValue(int(osc3Level->outval*100));
    }
    else if(param == osc3Octave)
    {
        osc3Octave->vstring->SetValue(int(osc3Octave->outval));
        UpdateOsc3Mul();
    }
    else if(param == osc3Detune)
    {
        osc3Detune->vstring->SetValue(int(osc3Detune->outval/0.029731547f*100));
        UpdateOsc3Mul();
    }
    else if(param == LFOAmp_Vol1)
    {
        LFOAmp_Vol1->vstring->SetValue(int(LFOAmp_Vol1->outval*100));
    }
    else if(param == LFOAmp_Vol2)
    {
        LFOAmp_Vol2->vstring->SetValue(int(LFOAmp_Vol2->outval*100));
    }
    else if(param == LFOAmp_Vol3)
    {
        LFOAmp_Vol3->vstring->SetValue(int(LFOAmp_Vol3->outval*100));
    }
    else if(param == LFOAmp_Pitch1)
    {
        LFOAmp_Pitch1->vstring->SetValue(int(LFOAmp_Pitch1->outval/0.059463094f*100));
    }
    else if(param == LFOAmp_Pitch2)
    {
        LFOAmp_Pitch2->vstring->SetValue(int(LFOAmp_Pitch2->outval/0.059463094f*100));
    }
    else if(param == LFOAmp_Pitch3)
    {
        LFOAmp_Pitch3->vstring->SetValue(int(LFOAmp_Pitch3->outval/0.059463094f*100));
    }
    else if(param == LFORate_Vol1)
    {
        LFORate_Vol1->vstring->SetValue(LFORate_Vol1->outval);
    }
    else if(param == LFORate_Vol2)
    {
        LFORate_Vol2->vstring->SetValue(LFORate_Vol2->outval);
    }
    else if(param == LFORate_Vol3)
    {
        LFORate_Vol3->vstring->SetValue(LFORate_Vol3->outval);
    }
    else if(param == LFORate_Pitch1)
    {
        LFORate_Pitch1->vstring->SetValue(LFORate_Pitch1->outval);
    }
    else if(param == LFORate_Pitch2)
    {
        LFORate_Pitch2->vstring->SetValue(LFORate_Pitch2->outval);
    }
    else if(param == LFORate_Pitch3)
    {
        LFORate_Pitch3->vstring->SetValue(LFORate_Pitch3->outval);
    }
    else if(param == LFOAmp_Flt1Freq)
    {
        LFOAmp_Flt1Freq->vstring->SetValue(int(LFOAmp_Flt1Freq->outval*100));
    }
    else if(param == LFOAmp_Flt2Freq)
    {
        LFOAmp_Flt2Freq->vstring->SetValue(int(LFOAmp_Flt2Freq->outval*100));
    }
    else if(param == LFORate_Flt1Freq)
    {
        LFORate_Flt1Freq->vstring->SetValue(LFORate_Flt1Freq->outval);
    }
    else if(param == LFORate_Flt2Freq)
    {
        LFORate_Flt2Freq->vstring->SetValue(LFORate_Flt2Freq->outval);
    }
    else if(param == osc1FM2Level)
    {
        osc1FM2Level->vstring->SetValue(int(osc1FM2Level->outval*100));
    }
    else if(param == osc2FM3Level)
    {
        osc2FM3Level->vstring->SetValue(int(osc2FM3Level->outval*100));
    }
    else if(param == osc1FM3Level)
    {
        osc1FM3Level->vstring->SetValue(int(osc1FM3Level->outval*100));
    }
    else if(param == osc1RM2Level)
    {
        osc1RM2Level->vstring->SetValue(int(osc1RM2Level->outval*100));
    }
    else if(param == osc2RM3Level)
    {
        osc2RM3Level->vstring->SetValue(int(osc2RM3Level->outval*100));
    }
    else if(param == osc1RM3Level)
    {
        osc1RM3Level->vstring->SetValue(int(osc1RM3Level->outval*100));
    }
    else if(param == osc1Width)
    {
        osc1Width->vstring->SetValue(osc1Width->outval);
    }
    else if(param == osc2Width)
    {
        osc2Width->vstring->SetValue(osc2Width->outval);
    }
    else if(param == osc3Width)
    {
        osc3Width->vstring->SetValue(osc3Width->outval);
    }
    /* // This stuff gets updated in the top of this function, when we call SetNormalVaule for the param.
    else if(param == filt1[0]->cutoff)
    {
        filt1[0]->ParamUpdate(filt1[0]->cutoff);
    }
    else if(param == filt2[0]->cutoff)
    {
        filt2[0]->ParamUpdate(filt2[0]->cutoff);
    }
    else if(param == filt1[0]->resonance)
    {
        filt1[0]->ParamUpdate(filt1[0]->resonance);
    }
    else if(param == filt2[0]->resonance)
    {
        filt2[0]->ParamUpdate(filt2[0]->resonance);
    }
    */

	//if(param->vstring != NULL)
	//{
	//	aslider->setText(String(param->vstring->outstr), false);
	//}
}

int Synth::MapEnvPointerToNum(float * eV)
{
    if(eV == &envV1)
        return 1;
    else if(eV == &envV2)
        return 2;
    else if(eV == &envV3)
        return 3;
    else if(eV == &envV4)
        return 4;
    else if(eV == &envV5)
        return 5;
    else if(eV == ebuff1)
        return 1;
    else if(eV == ebuff2)
        return 2;
    else if(eV == ebuff3)
        return 3;
    else if(eV == ebuff4)
        return 4;
    else if(eV == ebuff5)
        return 5;
	else
        return 0;
}

float* Synth::MapNumToEnvPointer(int num)
{
    switch(num)
    {
        case 1:
            return &envV1;
        case 2:
            return &envV2;
        case 3:
            return &envV3;
        case 4:
            return &envV4;
        case 5:
            return &envV5;
        default:
            return NULL;
    }
}

float* Synth::MapNumToEBuffPointer(int num)
{
    switch(num)
    {
        case 1:
            return ebuff1;
        case 2:
            return ebuff2;
        case 3:
            return ebuff3;
        case 4:
            return ebuff4;
        case 5:
            return ebuff5;
        default:
            return NULL;
    }
}

void Synth::SaveCustomStateData(XmlElement & xmlParentNode)
{
    XmlElement * xmlCustomData = new XmlElement(T("EnvIndex"));
    xmlCustomData->setAttribute(T("Osc1"), MapEnvPointerToNum(p_osc1envVal));
    xmlCustomData->setAttribute(T("Osc2"), MapEnvPointerToNum(p_osc2envVal));
    xmlCustomData->setAttribute(T("Osc3"), MapEnvPointerToNum(p_osc3envVal));
    xmlCustomData->setAttribute(T("Filt1"), MapEnvPointerToNum(p_flt1envVal));
    xmlCustomData->setAttribute(T("Filt2"), MapEnvPointerToNum(p_flt2envVal));

    xmlParentNode.addChildElement(xmlCustomData);

    xmlParentNode.addChildElement(env1->Save("Envelope1"));
    xmlParentNode.addChildElement(env2->Save("Envelope2"));
    xmlParentNode.addChildElement(env3->Save("Envelope3"));
    xmlParentNode.addChildElement(env4->Save("Envelope4"));
    xmlParentNode.addChildElement(env5->Save("Envelope5"));

    //xmlEnv1Data->setAttribute(T("Osc1"), MapEnvPointerToNum(p_osc1envVal));
}

void Synth::RestoreCustomStateData(XmlElement & xmlStateNode)
{
    int idx;
    if(xmlStateNode.hasTagName(T("EnvIndex")))
    {
        idx = xmlStateNode.getIntAttribute(T("Osc1"));
        p_osc1envVal = MapNumToEnvPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Osc2"));
        p_osc2envVal = MapNumToEnvPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Osc3"));
        p_osc3envVal = MapNumToEnvPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Filt1"));
        p_flt1envVal = MapNumToEBuffPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Filt2"));
        p_flt2envVal = MapNumToEBuffPointer(idx);

        if(SynthComp != NULL)
            SynthComp->UpdateEnvButts();
    }
    else if(xmlStateNode.hasTagName(T("Envelope1")))
    {
        env1->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope2")))
    {
        env2->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope3")))
    {
        env3->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope4")))
    {
        env4->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope5")))
    {
        env5->Load(&xmlStateNode);
    }

    if(SynthComp != NULL)
        SynthComp->repaint();
}

void Synth::CopyDataToClonedInstrument(Instrument * instr)
{
    Instrument::CopyDataToClonedInstrument(instr);

    Synth* nsynth = (Synth*)instr;

    XmlElement  xmlPreset(T("ClonePreset"));
    this->SaveStateData(xmlPreset, current_preset_name);

    XmlElement* xmlModule = xmlPreset.getChildByName(T("Module"));
    if(xmlModule != NULL)
        nsynth->RestoreStateData(*xmlModule);

    strcpy(nsynth->current_preset_name, current_preset_name);
    strcpy(nsynth->preset_path, preset_path);
}

#endif
