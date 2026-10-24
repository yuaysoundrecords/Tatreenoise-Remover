# Native editor design QA

**final result: passed**

Scope: native audio-plugin UI, not a website. The user selected the cream/cobalt reference and explicitly requested the same Thai indie/meme character, a colour lawyer, and audio-driven Motion FX. Native component capture and interaction tests are the relevant evidence; there is no browser implementation or browser console.

## Evidence and normalization

- Source truth: `docs/evidence/selected-reference.png`, 1509×1042 pixels.
- Final implementation: `docs/evidence/input-hot.png`, `idle.png`, `noise.png`, `de-esser.png`, `bypass.png`, `reduced-motion.png`, `stopped.png`.
- Native viewport: 1080×760, density 1. Responsive captures: `responsive-864.png` (864×608) and `responsive-1620.png` (1620×1140).
- `comparison-full.png`: both artifacts in one comparison input. The source is proportionally contained in a 1080×760 cream frame, not stretched. Implementation is unchanged at 1080×760. Source says Input Hot; final comparison uses the corresponding hot fixture. Numeric detector values and poses intentionally differ because the implementation is processing a test tone and has the requested new expression.
- Focused comparisons opened and inspected: `comparison-meters.png` and `comparison-lawyer.png`.
- Actual native-frame recording: `motion.mp4` / `motion.gif` (idle → noise → de-ess → hot → bypass → reduced motion).

## Findings and comparison history

- [Resolved P1] Thai labels were incorrectly decoded from C++ narrow literals in the first native capture. Added explicit UTF-8 conversion and a bundled Thai font. Recaptured and visually confirmed “เงียบไว้น้อง!” and all dynamic Thai captions. MSVC source encoding is set to UTF-8.
- [Resolved P1] Colour lawyer inherited a translucent drawing alpha in the first capture. Reset image opacity before drawing. Final captures show full colour, opaque face/robe and a deliberate cream sticker edge.
- [Resolved P2] Status card overlapped the face. Moved it to the right edge outside the facial region. Final idle/de-ess/hot captures keep the face and gesture readable.
- [Resolved P2] Main controls/meter band had drifted down and tick labels were too small/crowded. Moved controls upward, enlarged the meter region and typography, removed the crowded −3 dB tick, added dBFS / actual GR units. Focused meter comparison confirms readable, ordered scales.
- [Resolved P2] The subtitle overlapped the black robe. Moved it below the image onto cream paper. Checked final states and 80% layout.
- [Resolved P2] A single transformed image did not fully express the requested different reactions. Added dedicated colour hush and hot poses with the same character/style. Detector-state assertions and captured frames verify pose switching.
- [Resolved P2] A stopped host could leave detector-driven motion and peaks active. Added callback-age decay. The native integration harness stops calling DSP and verifies a return to idle; `stopped.png` is the post-fix evidence.

No actionable P0/P1/P2 findings remain within the selected native test-build scope.

## Required fidelity surfaces

- **Typography:** bundled Anton for heavy condensed Latin headings and Noto Sans Thai for body/Thai. Correct Thai shaping, no visible mojibake/truncation. Clear main/reduced controls, percentage values and units. Source's printed/distressed heading texture is simplified to clean native type for scaling.
- **Spacing/layout:** retained large left noise control, two smaller supporting controls, right lawyer, three-part dark meter strip, bottom filter/listen/bypass. Status sits off the face. All controls remain present at 80% and 150%.
- **Colours/tokens:** cobalt `#123d91`, navy `#102a4c`, warm cream `#eee9df`; mint/yellow signal meters, cyan GR, red hot warning. The source's neon segments were intentionally restrained and made semantically consistent.
- **Images:** generated cream paper texture and transparent full-colour lawyer pose assets are compiled into the plugin. Inspected source and all three actual raster poses. The requested colour/reaction changes are intentional; no anime replacement or generic illustration. The warning is a native UI status label; no placeholder icon art is used.
- **Copy/content:** product/brand names and “เงียบไว้น้อง!” retained; meaningful real FILTER/LISTEN/BYPASS/MOTION FX controls. Corrected the reference's GR axis from an input-level scale to 0–24 dB attenuation. Hot warning text remains visible with reduced motion enabled.

## Functional checks

Native integration harness exercises all knob attachments and all four button attachments against processor parameters. Audio test fixtures drive each expected motion state. Reduced motion stops transforms, bypass uses its own state unless input is hot, and stopping audio returns motion to idle. pluginval also opens the actual VST3 editor during processing and automates it successfully.

## Follow-up polish / test gaps

- [P3] Reference's torn blue typography/handwritten side note and chrome-heavy control faces are simplified; theme and hierarchy are retained. More distressing/hand-lettering can be an aesthetic iteration after the user tries this version.
- Perceptual voice quality and native macOS/Windows/DAW behaviour require their own tests, as documented in `VALIDATION.md`. This visual pass does not claim those unexecuted checks passed.

## Windows-first iteration

The selected visual target and palette remain unchanged. Additional implementation evidence is in `docs/evidence/windows-first/`: exact editable percentages, held-peak markers/readouts, separate Breath/Listen/Filter feedback, de-esser/input-hot colour poses and paused-transport idle state. Native Linux integration now opens and commits the actual numeric editor, rejects invalid input and checks all ten state captures. The actual Windows/MSVC/DPI/audio/DAW surface is still **unverified**; the visual pass above does not certify Windows.


## 2.0.1 gain update

Preserved selected cream/cobalt composition and colour lawyer poses. Added a dedicated Input / Auto Gain / Output row under the meters; main controls retain their previous sizes. Canvas is now 1080×860, resizable 80–150%. Inspected native captures for all three requested banner captions: no clipping, correct Thai glyphs, consistent poses. Local integration passes all gain/control bindings, 12 DSP-driven scenes and stopped transport. Windows MSVC/native UI/pluginval pipeline also passed (run 40055383919); Windows artifact download into this environment is currently blocked by its storage hostname, so these checked-in captures are explicitly Linux evidence.
