# Third-party components and assets

- JUCE 8.0.8, commit `d6181bde38d858c283c3b7bf699ce6340c050b5d`: fetched at configure time. JUCE is dual licensed under AGPLv3 / commercial terms. Its licence requirements must be satisfied before distributing binaries; the original v1 binary licence does not grant a JUCE licence for this new implementation. See https://juce.com/legal/juce-8-licence/ and the dependency's LICENSE.md.
- Anton (display font), SIL Open Font License 1.1. Licence in `assets/OFL-Anton.txt`.
- Noto Sans Thai (UI/Thai font), SIL Open Font License 1.1. Licence in `assets/OFL-NotoSansThai.txt`.
- `assets/lawyer-colour.png`, `assets/lawyer-deess.png`, `assets/lawyer-hot.png`: generated with Image Gen using the repository's original promotional image as the user-directed identity/pose reference. It is a generated reconstruction, not recovery of the original source photo. `assets/paper.png`: generated paper texture. The user selected the reference visual and requested this use.
- Original v1.02 ZIP, documents and promotional JPG are preserved unchanged in Git history and the verified external recovery archive, with their original terms; they are not needed in the current source kit. No reverse engineering of the binary was performed.

- Development-test fixture `tests/fixtures/jfk.wav` comes from ggml-org/whisper.cpp; source/digest metadata and upstream MIT licence are retained in that directory. It is not embedded in the plugin binary.

## DSP reference attribution

The local C++ minimum-statistics noise estimator is informed by the SpeexDSP preprocessor (Xiph.org / Jean-Marc Valin) and the Cohen–Berdugo noise-estimation principles it cites. It uses JUCE FFT processing; the SpeexDSP library is not linked or bundled. The upstream BSD licence and credits are retained below for this adaptation/reference. Airwindows DeEss (MIT), RNNoise and WebRTC VAD were inspected as design references; their source and models are not shipped. Pinned references and measured comparison scope are in `docs/DSP-QUALITY.md`. The external SpeexDSP 1.2.1 comparison library is a development tool only.

### SpeexDSP BSD notice

```text
Copyright 2002-2008 	Xiph.org Foundation
Copyright 2002-2008 	Jean-Marc Valin
Copyright 2005-2007	Analog Devices Inc.
Copyright 2005-2008	Commonwealth Scientific and Industrial Research
                        Organisation (CSIRO)
Copyright 1993, 2002, 2006 David Rowe
Copyright 2003 		EpicGames
Copyright 1992-1994	Jutta Degener, Carsten Bormann

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

```
