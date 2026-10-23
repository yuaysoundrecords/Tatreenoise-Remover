# Third-party components and assets

- JUCE 8.0.8, commit `d6181bde38d858c283c3b7bf699ce6340c050b5d`: fetched at configure time. JUCE is dual licensed under AGPLv3 / commercial terms. Its licence requirements must be satisfied before distributing binaries; the original v1 binary licence does not grant a JUCE licence for this new implementation. See https://juce.com/legal/juce-8-licence/ and the dependency's LICENSE.md.
- Anton (display font), SIL Open Font License 1.1. Licence in `assets/OFL-Anton.txt`.
- Noto Sans Thai (UI/Thai font), SIL Open Font License 1.1. Licence in `assets/OFL-NotoSansThai.txt`.
- `assets/lawyer-colour.png`, `assets/lawyer-deess.png`, `assets/lawyer-hot.png`: generated with Image Gen using the repository's original promotional image as the user-directed identity/pose reference. It is a generated reconstruction, not recovery of the original source photo. `assets/paper.png`: generated paper texture. The user selected the reference visual and requested this use.
- Original v1.02 ZIP, documents and promotional JPG remain unchanged with their original terms. No reverse engineering of the binary was performed.

- Development-test fixture `tests/fixtures/jfk.wav` comes from ggml-org/whisper.cpp; source/digest metadata and upstream MIT licence are retained in that directory. It is not embedded in the plugin binary.
