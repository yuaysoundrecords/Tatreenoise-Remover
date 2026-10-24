#!/usr/bin/env python3
"""Development comparison only. Reference library is not bundled with the plugin.
Example (Linux, after building TatreeOffline with the --filter-off option):
python tests/quality_benchmark.py --processor build/TatreeOffline_artefacts/Release/TatreeOffline \
 --speex-library /path/to/libspeexdsp.so.1 --output build/new-quality-results
All processors receive the same 16-bit input, 0.5 s noise-only preroll, and no AGC.
Tatree has filter/ess/breath disabled. Speex uses -12 dB noise suppression with
AGC/VAD/dereverb disabled and its measured bypass impulse latency removed.
Amounts are not equivalent controls. SNR/gain are proxies, not listening scores.
"""
import ctypes as c, numpy as np, wave, pathlib, subprocess, json
import argparse, hashlib, platform
parser=argparse.ArgumentParser(description="Controlled speech/noise comparison; requires NumPy and optional external SpeexDSP library. Does not establish perceptual or commercial parity.")
parser.add_argument('--processor',type=pathlib.Path,required=True)
parser.add_argument('--baseline',type=pathlib.Path)
parser.add_argument('--speex-library',type=pathlib.Path,required=True)
parser.add_argument('--output',type=pathlib.Path,required=True,help="New directory; existing results are never overwritten")
args=parser.parse_args()
root=pathlib.Path(__file__).resolve().parents[1]
out=args.output.resolve();out.mkdir(parents=True,exist_ok=False)
exe=str(args.processor.resolve())
def read(p):
 with wave.open(str(p)) as w:
  raw=w.readframes(w.getnframes());bits=w.getsampwidth();sr=w.getframerate()
  if bits==2:a=np.frombuffer(raw,'<i2').astype(float)/32768
  else:
   b=np.frombuffer(raw,np.uint8).reshape(-1,3).astype(np.int32);a=b[:,0]+(b[:,1]<<8)+(b[:,2]<<16);a=((a+8388608)%16777216-8388608)/8388608
  return a,sr
def write(p,x,sr):
 with wave.open(str(p),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes(np.clip(x*32768,-32768,32767).astype('<i2').tobytes())
lib=c.CDLL(str(args.speex_library.resolve()))
lib.speex_preprocess_state_init.argtypes=[c.c_int,c.c_int];lib.speex_preprocess_state_init.restype=c.c_void_p
lib.speex_preprocess_ctl.argtypes=[c.c_void_p,c.c_int,c.c_void_p];lib.speex_preprocess_run.argtypes=[c.c_void_p,c.POINTER(c.c_short)];lib.speex_preprocess_state_destroy.argtypes=[c.c_void_p]
def speex(x,sr,enabled=True):
 n=sr//100;st=lib.speex_preprocess_state_init(n,sr)
 for key,val in [(0,int(enabled)),(2,0),(4,0),(8,0),(18,-12)]:
  v=c.c_int(val);assert lib.speex_preprocess_ctl(st,key,c.byref(v))==0
 a=np.clip(np.pad(x,(0,n*3))*32768,-32768,32767).astype(np.int16);res=[]
 for i in range(0,len(a)-n+1,n):
  buf=a[i:i+n].copy();lib.speex_preprocess_run(st,buf.ctypes.data_as(c.POINTER(c.c_short)));res.append(buf.astype(float)/32768)
 lib.speex_preprocess_state_destroy(st);return np.concatenate(res)
clean,sr=read(root/'tests/fixtures/jfk.wav');clean*=.5;pre=sr//2;reference=np.pad(clean,(pre,sr));rng=np.random.default_rng(749)
imp=np.zeros(sr);imp[500]=.5;latency=int(np.argmax(np.abs(speex(imp,sr,False))))-500
print('Speex latency',latency,flush=True)
rows=[]
for kind in ['white','pink','hum','modulated']:
 for snr in [0,10]:
  n=rng.standard_normal(len(reference));t=np.arange(len(n))/sr
  if kind=='pink':
   f=np.fft.rfftfreq(len(n));a=np.fft.rfft(n);a[0]=0;a/=np.sqrt(np.maximum(f,1/len(n)));n=np.fft.irfft(a,len(n))
  if kind=='hum':n=np.sin(2*np.pi*60*t)+.4*np.sin(2*np.pi*120*t)+.1*n
  if kind=='modulated':n*=.15+.85*(np.sin(2*np.pi*3.1*t)>0)
  n*=np.sqrt(np.mean(clean**2)/10**(snr/10))/np.sqrt(np.mean(n[pre:pre+len(clean)]**2))
  noisy=reference+n;src=out/f'{kind}-{snr}-input.wav';write(src,noisy,sr)
  assert np.max(np.abs(noisy))<.99, 'Fixture clipping would invalidate the comparison'
  variants={'SpeexDSP 1.2.1 -12dB':speex(noisy,sr)[latency:latency+len(reference)]}
  for label,processor in [('Tatree current',exe)]+([('Tatree 2.0.1',str(args.baseline.resolve()))] if args.baseline else []):
   for amount in [45,65]:
    dst=out/f'{kind}-{snr}-{label.replace(" ","-")}-{amount}.wav'
    subprocess.run([processor,str(src),str(dst),str(amount),'0','0','--filter-off'],check=True,stdout=subprocess.DEVNULL)
    variants[f'{label} {amount}%']=read(dst)[0]
  for name,y in variants.items():
   a=y[pre:pre+len(clean)];gain=float(a@clean/(clean@clean));err=float(np.sum((a-clean)**2));snrout=float(10*np.log10(np.sum(clean**2)/err));sisdr=float(10*np.log10(np.sum((gain*clean)**2)/np.sum((a-gain*clean)**2)))
   row=dict(noise=kind,input_snr=snr,processor=name,output_snr=round(snrout,3),si_sdr=round(sisdr,3),voice_gain=round(gain,3));rows.append(row);print(row,flush=True)
clean_results=[]
clean_src=out/'clean-reference.wav';write(clean_src,clean,sr)
for label,processor in [('Tatree current',exe)]+([('Tatree 2.0.1',str(args.baseline.resolve()))] if args.baseline else []):
 for amount in [0,45,65]:
  dst=out/f'clean-{label.replace(" ","-")}-{amount}.wav'
  subprocess.run([processor,str(clean_src),str(dst),str(amount),'0','0','--filter-off'],check=True,stdout=subprocess.DEVNULL)
  y=read(dst)[0];gain=float(y@clean/(clean@clean));error_snr=float(10*np.log10(np.sum(clean**2)/np.sum((y-clean)**2)))
  clean_results.append(dict(processor=label,amount=amount,voice_gain=gain,error_snr=error_snr))
  print('clean',label,amount,'gain',gain,'errorSNR',error_snr,flush=True)
(out/'comparison.json').write_text(json.dumps(dict(sample_rate=sr,speex_latency=latency,seed=749,preroll_seconds=.5,reference_scale=.5,reference_source_sha256=hashlib.sha256((root/'tests/fixtures/jfk.wav').read_bytes()).hexdigest(),processor_sha256=hashlib.sha256(args.processor.read_bytes()).hexdigest(),baseline_sha256=hashlib.sha256(args.baseline.read_bytes()).hexdigest() if args.baseline else None,speex_library_sha256=hashlib.sha256(args.speex_library.resolve().read_bytes()).hexdigest(),python=platform.python_version(),numpy=np.__version__,rows=rows,clean=clean_results),indent=2))
