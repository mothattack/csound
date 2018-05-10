//  vst4cs: VST HOST OPCODES FOR CSOUND
//
//  Uses code by Hermann Seib from his VSTHost program and from the vst~
//  object by Thomas Grill (no license), which in turn borrows from the Psycle
//  tracker (also based on VSTHost).
//
//  VST is a trademark of Steinberg Media Technologies GmbH.
//  VST Plug-In Technology by Steinberg.
//
//  Copyright (C) 2004 Andres Cabrera, Michael Gogins
//
//  The vst4cs library is free software; you can redistribute it
//  and/or modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  The vst4cs library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with The vst4cs library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
//  02110-1301 USA
//
//  Linking vst4cs statically or dynamically with other modules is making a
//  combined work based on vst4cs. Thus, the terms and conditions of the GNU
//  Lesser General Public License cover the whole combination.
//
//  In addition, as a special exception, the copyright holders of vst4cs,
//  including the Csound developers and Hermann Seib, the original author of
//  VSTHost, give you permission to combine vst4cs with free software programs
//  or libraries that are released under the GNU LGPL and with code included
//  in the standard release of the VST SDK version 2 under the terms of the
//  license stated in the VST SDK version 2 files. You may copy and distribute
//  such a system following the terms of the GNU LGPL for vst4cs and the
//  licenses of the other code concerned. The source code for the VST SDK
//  version 2 is available in the VST SDK hosted at
//  https://github.com/steinbergmedia/vst3sdk.
//
//  Note that people who make modified versions of vst4cs are not obligated to
//  grant this special exception for their modified versions; it is their
//  choice whether to do so. The GNU Lesser General Public License gives
//  permission to release a modified version without this exception; this
//  exception also makes it possible to release a modified version which
//  carries forward this exception.

#ifndef VSTPLUGIN_HOST_H
#define VSTPLUGIN_HOST_H

#include "csdl.h"

#if !defined(SWIG) && defined(CS_VSTHOST)
#include <audioeffectx.h>
#endif

#include <vector>
#include <map>
#include <string>

/**
 * THE REAL VST INTERFACE IS JUST THIS, USE THE EXACT SAME TYPES:
 *
 * typedef      VstIntPtr (VSTCALLBACK *audioMasterCallback) (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
 * typedef VstIntPtr (VSTCALLBACK *AEffectDispatcherProc) (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
 * typedef void (VSTCALLBACK *AEffectProcessProc) (AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames);
 * typedef void (VSTCALLBACK *AEffectProcessDoubleProc) (AEffect* effect, double** inputs, double** outputs, VstInt32 sampleFrames);
 * typedef void (VSTCALLBACK *AEffectSetParameterProc) (AEffect* effect, VstInt32 index, float parameter);
 * typedef float (VSTCALLBACK *AEffectGetParameterProc) (AEffect* effect, VstInt32 index);
 */

typedef enum {
  MAX_EVENTS = 64,
  MAX_INOUTS = 8,
  VSTINSTANCE_ERR_NO_VALID_FILE = -1,
  VSTINSTANCE_ERR_NO_VST_PLUGIN = -2,
  VSTINSTANCE_ERR_REJECTED = -3,
  VSTINSTANCE_NO_ERROR = 0
} VST4CS_ENUM;

typedef AEffect * (*PVSTMAIN)(audioMasterCallback audioMaster);

class Fl_Window;

class VSTPlugin {
 public:
  CSOUND *csound;
  void *libraryHandle;
  AEffect *aeffect;
  bool hasEditor;
  AEffEditor *editor;
  ERect rect;
  Fl_Window *window;
  void *windowHandle;
  char productName[64];
  char vendorName[64];
  char libraryName[0x100];
  uint64_t pluginVersion;
  bool pluginIsSynth;
  std::vector<float *> inputs;
  std::vector<float *> outputs;
  std::vector< std::vector<float> > inputs_;
  std::vector< std::vector<float> > outputs_;
  std::vector<VstMidiEvent> vstMidiEvents;
  std::vector<char> vstEventsBuffer;
  bool overwrite;
  bool edited;
  bool showParameters;
  VstTimeInfo vstTimeInfo;
  float framesPerSecond;
  size_t framesPerBlock;
  int targetFLpanel; //GAB

  VSTPlugin(CSOUND *csound);
  virtual ~VSTPlugin();
  virtual void StopEditing();
  virtual int GetNumCategories();
  virtual bool GetProgramName(int cat, int p, char *buf);
  virtual bool ShowParams();
  virtual void SetShowParameters(bool s);
  virtual void OnEditorClose();
  virtual void SetEditWindow(void *h);
  virtual ERect GetEditorRect();
  virtual void EditorIdle();
  virtual bool replace();
  virtual void Free();
  virtual int Instantiate(const char *libraryPathname);
  virtual void Info();
  virtual void Init();
  virtual int GetNumParams(void);
  virtual void GetParamName(int param, char *name);
  virtual float GetParamValue(int param);
  virtual int getNumInputs(void);
  virtual int getNumOutputs(void);
  virtual char* GetName(void);
  virtual uint64_t GetVersion();
  virtual char* GetVendorName(void);
  virtual char* GetDllName(void);
  virtual long NumParameters(void);
  virtual float GetParameter(long parameter);
  virtual bool DescribeValue(int parameter,char* psTxt);
  virtual bool SetParameter(int parameter, float value);
  virtual bool SetParameter(int parameter, int value);
  virtual void SetCurrentProgram(int prg);
  virtual int GetCurrentProgram();
  virtual int NumPrograms();
  virtual bool IsSynth();
  virtual bool AddMIDI(int data, int deltaFrames, int detune);
  virtual void SendMidi();
  virtual void processReplacing(float **inputs,
                                float **outputs,
                                long sampleframes);
  virtual void process(float **inputs,
                       float **outputs,
                       long sampleframes);
  // typedef VstIntPtr (VSTCALLBACK *AEffectDispatcherProc) (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);

  virtual VstIntPtr Dispatch(VstInt32 opCode,
                        VstInt32 index=0,
                        VstIntPtr value=0,
                        void *ptr=0,
                        float opt=0);
  virtual void Log(const char *format, ...);
  virtual void Debug(const char *format, ...);
  virtual void OpenEditor();
  virtual void CloseEditor();
  virtual VstTimeInfo *GetTime();
  static bool OnInputConnected(AEffect *effect, long input);
  static bool OnOutputConnected(AEffect *effect, long output);
  static long OnGetVersion(AEffect *effect);
  static bool OnCanDo(const char *ptr);

  //  typedef   VstIntPtr (VSTCALLBACK *audioMasterCallback) (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);

  static VstIntPtr Master(AEffect *effect,
                     VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr,
                     float opt);
  static void initializeOpcodes();
  long EffGetChunk(void **ptr, bool isPreset = false) // GAB
  {
    return Dispatch(effGetChunk, isPreset, 0, ptr);
  }
  long EffGetProgram()
  {
    return Dispatch(effGetProgram);
  }
  void EffSetProgram(long lValue)
  {
    Dispatch(effSetProgram, 0, lValue);
  }
  void EffGetProgramName(char *ptr)
  {
    Dispatch(effGetProgramName, 0, 0, ptr);
  }
  static std::map<long, std::string> &masterOpcodes()
  {
    static std::map<long, std::string> masterOpcodes_;
    return masterOpcodes_;
  }
  static std::map<long, std::string> &dispatchOpcodes()
  {
    static std::map<long, std::string> dispatchOpcodes_;
    return dispatchOpcodes_;
  }
};

inline VstIntPtr VSTPlugin::Dispatch(VstInt32 opCode,
                        VstInt32 index,
                        VstIntPtr value,
                        void *ptr,
                        float opt)
{
  if (aeffect)
    return aeffect->dispatcher(aeffect, opCode, index, value, ptr, opt);
  else
    return 0;
}

inline void VSTPlugin::processReplacing(float **ins, float **outs, long frames)
{
  if (aeffect) {
    SendMidi();
    aeffect->processReplacing(aeffect, ins, outs, frames);
  }
}

inline void VSTPlugin::process(float **ins, float **outs, long frames)
{
  if (aeffect) {
    SendMidi();
    aeffect->processReplacing(aeffect, ins, outs, frames);
  }
}

#endif
