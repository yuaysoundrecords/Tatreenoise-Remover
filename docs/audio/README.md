# Audio example provenance

`quality-speech-before/after.wav`: JFK fixture scaled by 0.5, seeded white noise at 10 dB SNR, 0.5 s noise-only preroll. After uses the 2.0.2/2.0.3 noise kernel at 45%, all other processing off. Full method and binary hashes: `../DSP-QUALITY.md`.

`vocal-gain-before/after.wav`: the same JFK source multiplied by an envelope 0.5 → 0.9 → 0.65 with smooth transitions at 28–32% and 58–62% of its duration. After uses the 2.0.3 Auto Gain candidate with Noise/Ess/Breath/Filter off. Offline latency is trimmed, source length/sample rate preserved. Output peak measured -4.87 dBFS on this example; this is not a general peak guarantee. No normalization was applied after processing. Sample source, SHA-256 and MIT notice are retained in `tests/fixtures/` at repository root. These examples cover one English speaker, not Thai/singing/real breath classification.
