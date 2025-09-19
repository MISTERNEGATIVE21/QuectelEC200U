Voice call audio wiring (EC200U)

Important
- Always verify against the latest Quectel EC200U Hardware Design manual for your sub-variant. Pin names and capabilities can differ.
- Use differential routing, short traces, good analog grounding, and ESD protection near external connectors.

Common audio pins (typical)
- Microphone (handset/headset): MIC_P, MIC_N (sometimes MIC1P/MIC1N or MIC2P/MIC2N)
- Microphone bias: MIC_BIAS (provides bias for electret mics)
- Earpiece/receiver (handset): EAR_P, EAR_N (a small 32 Ω receiver)
- Loudspeaker: SPK_P, SPK_N (often needs an external differential Class‑D amplifier for >0.5 W)
- Audio channel select: via AT+QAUDCH, interface/routing via AT+QDAI

Wiring examples (typical)
- Electret microphone (mono):
  - MIC_P -> series DC‑block capacitor (e.g., 100 nF–1 µF) -> mic capsule (+)
  - MIC_N -> mic capsule (−)
  - MIC_BIAS -> bias resistor (e.g., 2.2 kΩ) -> mic capsule (+)
  - Add RC low‑pass (e.g., 100 Ω + 1 nF) close to the module if recommended by the HW guide.
- Handset earpiece (receiver):
  - EAR_P/EAR_N -> 32 Ω receiver (differential). Keep leads short and twisted if possible.
- Loudspeaker (hands‑free):
  - SPK_P/SPK_N -> differential input of external audio amplifier -> speaker.
  - If directly driving a small speaker, check the permissible load and power in the HW guide.

Headset TRRS (3.5 mm) quick notes
- CTIA wiring: Tip=L, Ring1=R, Ring2=GND, Sleeve=MIC.
- OMTP swaps MIC and GND. Use CTIA jacks/adapters if possible.
- Map MIC to MIC_P (through DC‑block) and sleeve return to MIC_N. Route L/R to EAR/aux if used (check module’s stereo support).

AT command tips for voice/audio
- Select audio path: AT+QAUDCH=<ch>  (0=handset, 1=headset, 2=loudspeaker; check manual)
- Speaker volume: AT+CLVL=<0..100>
- Ringer volume: AT+CRSL=<0..100>
- Mic mute: AT+CMUT=<0|1>
- Mic gain: AT+QMIC=<chan>,<0..15>
- Sidetone: AT+QSIDET=<0|1>,<0..15>
- Audio interface/routing: AT+QDAI=...
- Loopback test: AT+QAUDLOOP=<0|1>

Safety and layout
- Keep analog audio traces short; avoid crossing digital clocks.
- Use ground guard/shielding around MIC lines if noisy.
- Separate AGND and DGND per the HW guide and join at a single point.
- Add TVS/ESD diodes on external audio connectors.

Disclaimer
- This is a generic guide. Always follow Quectel’s official hardware design manual for your exact module and carrier requirements.
