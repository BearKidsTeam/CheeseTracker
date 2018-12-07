/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
                          interface_help_text.h  -  description
                             -------------------
    begin                : Tue Apr 9 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


static const char* help_pattern = "\n\
\n\
Pattern Edit Effects:\n\
-------------\n\
\n\
 Summary of Effects.\n\
\n\
  Volume Column effects.\n\
   Ax Fine volume slide up by x\n\
   Bx Fine volume slide down by x\n\
   Cx Volume slide up by x\n\
   Dx Volume slide down by x\n\
   Ex Pitch slide down by x\n\
   Fx Pitch slide up by x\n\
   Gx Slide to note with speed x\n\
   Hx Vibrato with depth x\n\
\n\
  General effects.\n\
   Axx Set song speed (hex)\n\
   Bxx Jump to Order (hex)\n\
   Cxx Break to row xx (hex) of next pattern\n\
   D0x Volume slide down by x\n\
   Dx0 Volume slide up by x\n\
   DFx Fine volume slide down by x\n\
   DxF Fine volume slide up by x\n\
   Exx Pitch slide down by xx\n\
   EFx Fine pitch slide down by x\n\
   EEx Extra fine pitch slide down by x\n\
   Fxx Pitch slide up by xx\n\
   FFx Fine pitch slide up by x\n\
   FEx Extra fine pitch slide up by x\n\
   Gxx Slide to note with speed xx\n\
   Hxy Vibrato with speed x, depth y\n\
   Ixy Tremor with ontime x and offtime y\n\
   Jxy Arpeggio with halftones x and y\n\
   Kxx Dual Command: H00 & Dxx\n\
   Lxx Dual Command: G00 & Dxx\n\
   Mxx Set channel volume to xx (0->40h)\n\
   N0x Channel volume slide down by x\n\
   Nx0 Channel volume slide up by x\n\
   NFx Fine channel volume slide down by x\n\
   NxF Fine channel volume slide up by x\n\
   Oxx Set sample offset to yxx00h, y set with SAy\n\
   P0x Panning slide to right by x\n\
   Px0 Panning slide to left by x\n\
   PFx Fine panning slide to right by x\n\
   PxF Fine panning slide to left by x\n\
   Qxy Retrigger note every y ticks with volume modifier x\n\
     Values for x:\n\
       0: No volume change         8: Not used\n\
       1: -1                       9: +1\n\
       2: -2                       A: +2\n\
       3: -4                       B: +4\n\
       4: -8                       C: +8\n\
       5: -16                      D: +16\n\
       6: *2/3                     E: *3/2\n\
       7: *1/2                     F: *2\n\
   Rxy Tremelo with speed x, depth y\n\
   S0x Unused\n\
   S1x Set chorus send level\n\
   S2x Unused\n\
   S3x Set vibrato waveform to type x\n\
   S4x Set tremelo waveform to type x\n\
   S5x Set panbrello waveform to type x\n\
     Waveforms for commands S3x, S4x and S5x:\n\
       0: Sine wave\n\
       1: Ramp down\n\
       2: Square wave\n\
       3: Random wave\n\
   S6x Pattern delay for x ticks\n\
   S70 Past note cut\n\
   S71 Past note off\n\
   S72 Past note fade\n\
   S73 Set NNA to note cut\n\
   S74 Set NNA to continue\n\
   S75 Set NNA to note off\n\
   S76 Set NNA to note fade\n\
   S77 Turn off volume envelope\n\
   S78 Turn on volume envelope\n\
   S79 Turn off panning envelope\n\
   S7A Turn on panning envelope\n\
   S7B Turn off pitch envelope\n\
   S7C Turn on pitch envelope\n\
   S8x Set panning position\n\
   S91 Set surround sound\n\
   SAy Set high value of sample offset yxx00h\n\
   SB0 Set loopback point\n\
   SBx Loop x times to loopback point\n\
   SCx Note cut after x ticks\n\
   SDx Note delay for x ticks\n\
   SEx Pattern delay for x rows\n\
   SFx Set parameterised MIDI Macro\n\
   T0x Tempo slide down by x\n\
   T1x Tempo slide up by x\n\
   Txx Set Tempo to xx (20h->0FFh)\n\
   Uxy Fine vibrato with speed x, depth y\n\
   Vxx Set global volume to xx (0->80h)\n\
   W0x Global volume slide down by x\n\
   Wx0 Global volume slide up by x\n\
   WFx Fine global volume slide down by x\n\
   WxF Fine global volume slide up by x\n\
   Xxx Set panning position (0->0FFh)\n\
   Yxy Panbrello with speed x, depth y\n\
   Zxx (0->7Fh) Frequency cutoff, (80h->8Fh) Q (Resonance)\n\
       (90h->FFh) Reverb send level.\n\
";

