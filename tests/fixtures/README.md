# Speech regression fixture

`jfk.wav` is the short JFK speech sample distributed with ggml-org/whisper.cpp. Source URL and file SHA-256 are recorded in `jfk.json`; upstream MIT licence is retained in `WHISPER-LICENSE.txt`.

The test treats this fixed recording as a reference, adds deterministic Gaussian noise at approximately 10 dB SNR, runs the noise processor with other effects off and trims the declared latency. It checks that the default noise setting improves SNR by at least 0.3 dB and preserves correlation gain above 0.85. It also reports lower/higher settings without claiming they are globally optimal.

This single regression does not establish perceptual quality for Thai speech, singing, music, different noise types, or superiority over the original v1 plugin. Use real listening comparisons as well.
