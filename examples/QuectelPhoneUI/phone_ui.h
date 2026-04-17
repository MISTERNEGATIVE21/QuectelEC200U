#ifndef PHONE_UI_H
#define PHONE_UI_H

const char phone_ui_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>Android</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent;user-select:none}
:root{
--pri:#c2e7ff;--priD:#004a77;--priC:rgba(194,231,255,.12);
--sec:#c3e8d0;--secD:#005234;
--ter:#efb8c8;--terD:#7d2040;
--bg:#111318;--s1:#1d2024;--s2:#282a2e;--s3:#353840;
--cv:#1b1f24;--onS:#c6c6cd;
--t1:#e2e2e9;--t2:#8e929a;--t3:#5d616a;
--err:#f2b8b5;--errD:#601410;
--gn:#a8dab5;--gnD:#00391c;--yl:#e3cc6e;
--ff:system-ui,-apple-system,Roboto,'Segoe UI',sans-serif;
--r:28px;--rs:16px;--rx:14px;
}
html,body{height:100%;overflow:hidden;font-family:var(--ff);background:var(--bg);color:var(--t1)}
body{display:flex;flex-direction:column;position:relative}

/* ===== ANIMATED WALLPAPER ===== */
.wall{position:fixed;inset:0;z-index:0;overflow:hidden;
background:linear-gradient(165deg,#0d1117 0%,#111827 35%,#1e1b4b 70%,#0f172a 100%)}
.wall .orb{position:absolute;border-radius:50%;filter:blur(80px);opacity:.35;animation:orb 20s ease-in-out infinite alternate}
.wall .o1{width:300px;height:300px;top:-60px;left:-40px;background:#3b82f6}
.wall .o2{width:250px;height:250px;bottom:-40px;right:-30px;background:#8b5cf6}
.wall .o3{width:200px;height:200px;top:40%;left:60%;background:#06b6d4;animation-delay:-7s}
@keyframes orb{0%{transform:translate(0,0) scale(1)}50%{transform:translate(30px,-20px) scale(1.1)}100%{transform:translate(-20px,30px) scale(.95)}}

/* ===== STATUS BAR ===== */
.sb{height:28px;display:flex;align-items:center;justify-content:space-between;padding:0 20px;
font-size:11px;font-weight:500;color:rgba(255,255,255,.9);z-index:50;position:relative}
.sb-l{display:flex;align-items:center;gap:5px}
.sb-r{display:flex;align-items:center;gap:8px}
.sb-sig{display:flex;align-items:flex-end;gap:1.5px;height:12px}
.sb-sig .b{width:3px;border-radius:1px;background:rgba(255,255,255,.25);transition:.4s}
.sb-sig .b.on{background:rgba(255,255,255,.92)}
.sb-bt-i{width:18px;height:9px;border:1.5px solid rgba(255,255,255,.6);border-radius:2px;position:relative}
.sb-bt-i::after{content:'';position:absolute;right:-3px;top:2px;width:1.5px;height:4px;background:rgba(255,255,255,.4);border-radius:1px}
.sb-bt-f{height:100%;border-radius:.5px;background:var(--gn);transition:.6s}

/* ===== HOME ===== */
.home{position:relative;flex:1;display:flex;flex-direction:column;z-index:10;overflow:hidden}
/* At a Glance */
.aag{padding:28px 28px 0;color:rgba(255,255,255,.88)}
.aag-d{font-size:15px;font-weight:400;letter-spacing:.2px}
.aag-op{font-size:12px;opacity:.55;margin-top:2px}
/* Clock */
.clk{text-align:center;padding:24px 0 14px}
.clk-t{font-size:72px;font-weight:100;color:#fff;letter-spacing:-4px;line-height:1}
/* App Grid */
.apps{flex:1;overflow-y:auto;padding:0 16px;scrollbar-width:none}
.apps::-webkit-scrollbar{display:none}
.agrid{display:grid;grid-template-columns:repeat(4,1fr);gap:22px 8px;padding:6px 0 20px}
.aico{display:flex;flex-direction:column;align-items:center;gap:7px;cursor:pointer}
.aico:active .aicn{transform:scale(.85)}
/* Material You squircle icon */
.aicn{width:56px;height:56px;border-radius:16px;display:flex;align-items:center;justify-content:center;
font-size:26px;transition:transform .15s cubic-bezier(.2,0,.38,.9);
box-shadow:0 4px 12px rgba(0,0,0,.35),inset 0 1px 1px rgba(255,255,255,.08)}
.aicl{font-size:12px;color:rgba(255,255,255,.78);font-weight:400;text-align:center;max-width:72px;
overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
/* Search bar — Google Pixel style */
.srch{margin:0 20px 10px;height:52px;background:rgba(255,255,255,.1);backdrop-filter:blur(24px);
border-radius:26px;display:flex;align-items:center;padding:0 20px;gap:12px;
color:rgba(255,255,255,.45);font-size:15px;cursor:pointer;
border:1px solid rgba(255,255,255,.06);transition:.2s}
.srch:active{background:rgba(255,255,255,.14);transform:scale(.98)}
.srch svg{width:22px;height:22px;flex-shrink:0}
/* Dock */
.dock{display:flex;justify-content:space-evenly;padding:8px 20px 4px;
background:rgba(0,0,0,.18);backdrop-filter:blur(32px);border-radius:32px 32px 0 0;margin:0 6px;
border-top:1px solid rgba(255,255,255,.03)}
.dock .aicn{width:50px;height:50px;border-radius:14px;font-size:23px}
/* Gesture pill */
.gpill{width:128px;height:5px;background:rgba(255,255,255,.22);border-radius:3px;
margin:6px auto 8px;flex-shrink:0}

/* ===== APP SCREENS ===== */
.asc{position:fixed;inset:0;z-index:100;background:var(--bg);display:none;flex-direction:column}
.asc.on{display:flex;animation:ain .25s cubic-bezier(.2,0,0,1)}
@keyframes ain{0%{opacity:0;transform:scale(.94) translateY(16px)}100%{opacity:1;transform:none}}
/* App bar */
.ahd{height:64px;display:flex;align-items:center;padding:0 4px;gap:4px;flex-shrink:0}
.abk{width:48px;height:48px;display:flex;align-items:center;justify-content:center;cursor:pointer;
border-radius:50%;border:none;background:none;color:var(--t1);transition:.15s}
.abk:active{background:var(--priC)}
.abk svg{width:24px;height:24px;fill:currentColor}
.atl{font-size:22px;font-weight:400;flex:1;letter-spacing:-.3px}
.abd{flex:1;overflow-y:auto;padding:0 16px 24px}
.abd::-webkit-scrollbar{width:3px}.abd::-webkit-scrollbar-thumb{background:rgba(255,255,255,.06);border-radius:2px}

/* ===== MATERIAL YOU CARDS ===== */
.mc{background:var(--s1);border-radius:var(--r);padding:18px;margin-bottom:14px;
border:1px solid rgba(255,255,255,.04);transition:transform .1s}
.mc:active{transform:scale(.995)}
.mct{font-size:11px;font-weight:600;color:var(--t2);text-transform:uppercase;letter-spacing:1px;margin-bottom:12px}
/* Metric grid */
.mg{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.mi{background:var(--cv);border-radius:var(--rs);padding:12px;text-align:center}
.mil{font-size:10px;color:var(--t3);text-transform:uppercase;letter-spacing:.6px;margin-bottom:3px}
.miv{font-size:20px;font-weight:700}.miv.s{font-size:14px;font-weight:600}

/* ===== INPUTS — Material 3 style ===== */
input,textarea,select{width:100%;background:var(--cv);border:2px solid rgba(255,255,255,.06);
padding:14px 16px;border-radius:var(--rx);color:var(--t1);font-family:var(--ff);font-size:15px;
transition:border-color .2s,box-shadow .2s;outline:none}
input:focus,textarea:focus{border-color:var(--priD);box-shadow:0 0 0 3px rgba(194,231,255,.08)}
input::placeholder,textarea::placeholder{color:var(--t3)}
textarea{resize:vertical}

/* ===== BUTTONS — Material 3 ===== */
.bt{display:inline-flex;align-items:center;justify-content:center;gap:8px;padding:14px 24px;
border-radius:100px;border:none;font-family:var(--ff);font-size:14px;font-weight:500;
cursor:pointer;transition:.15s;width:100%;letter-spacing:.3px}
.bt:active{transform:scale(.96);filter:brightness(1.1)}
.bp{background:var(--priD);color:var(--pri)}.bs{background:var(--s2);color:var(--t1);border:1px solid rgba(255,255,255,.08)}
.bd{background:var(--errD);color:var(--err)}.bg{background:var(--gnD);color:var(--gn)}
.bsm{padding:10px 18px;font-size:13px;width:auto}
.br{display:flex;gap:8px}.g2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.fb{display:flex;justify-content:space-between;align-items:center}
.mt{margin-top:12px}.mb{margin-bottom:12px}
.tc{text-align:center}.tm{color:var(--t2);font-size:14px}

/* ===== DIALER ===== */
.dd{text-align:center;padding:16px 0;font-size:32px;font-weight:200;letter-spacing:3px;min-height:60px;color:var(--pri)}
.kpd{display:grid;grid-template-columns:repeat(3,1fr);gap:12px;max-width:280px;margin:0 auto}
.ky{aspect-ratio:1;display:flex;flex-direction:column;align-items:center;justify-content:center;
background:var(--s2);border-radius:50%;border:none;color:var(--t1);font-size:24px;font-weight:300;
cursor:pointer;transition:.12s;font-family:var(--ff)}
.ky:active{background:var(--priC);transform:scale(.9)}
.ks{font-size:9px;color:var(--t3);letter-spacing:2px;font-weight:500;margin-top:1px}
.cab{display:flex;justify-content:center;gap:18px;margin-top:16px}
.cbt{width:56px;height:56px;border-radius:50%;border:none;display:flex;align-items:center;
justify-content:center;cursor:pointer;transition:.15s}
.cbt:active{transform:scale(.88)}
.cbt svg{width:24px;height:24px;fill:#fff}

/* SMS */
.smi{background:var(--cv);border-radius:var(--rs);padding:14px;margin-bottom:10px;position:relative;
border:1px solid rgba(255,255,255,.03)}
.sms{font-weight:600;color:var(--pri);font-size:15px}.smd{font-size:11px;color:var(--t3);position:absolute;top:14px;right:14px}
.smb{font-size:14px;line-height:1.5;margin-top:5px;white-space:pre-wrap;word-break:break-word;color:var(--onS)}
/* Compose sheet */
.csh{position:fixed;bottom:0;left:0;right:0;background:var(--s1);backdrop-filter:blur(30px);
border-radius:var(--r) var(--r) 0 0;padding:20px 16px;transform:translateY(100%);
transition:transform .3s cubic-bezier(.2,0,0,1);z-index:200;border-top:1px solid rgba(255,255,255,.06)}
.csh.op{transform:translateY(0)}
.csh-h{width:36px;height:4px;background:var(--t3);border-radius:2px;margin:0 auto 16px}

/* Browser */
.bvw{background:#fafafa;color:#1a1a1a;border-radius:var(--rs);padding:16px;min-height:180px;
max-height:50vh;overflow:auto;font-size:14px;line-height:1.6}
/* Terminal */
.tmo{background:#0d1117;color:var(--gn);font-family:'Courier New',monospace;padding:14px;
border-radius:var(--rx);height:240px;overflow-y:auto;font-size:13px;line-height:1.6;
white-space:pre-wrap;word-break:break-all;margin-bottom:10px;border:1px solid rgba(255,255,255,.04)}
/* Game */
#snk{border-radius:var(--rs);border:1px solid rgba(255,255,255,.06);background:#0d1117;touch-action:none}
.gp{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;width:156px}
.gb{padding:14px;border-radius:14px;border:none;background:var(--s2);color:var(--t1);font-size:18px;
cursor:pointer;font-family:var(--ff);transition:.12s}
.gb:active{background:var(--priC);transform:scale(.9)}
/* WiFi */
.wfi{background:var(--cv);border-radius:var(--rx);padding:12px 16px;margin-bottom:8px;
display:flex;justify-content:space-between;align-items:center;border:1px solid rgba(255,255,255,.03)}
/* Setting row */
.srow{display:flex;align-items:center;justify-content:space-between;padding:14px 0;
border-bottom:1px solid rgba(255,255,255,.04)}
.srow:last-child{border-bottom:none}
.srow-t{font-size:15px;font-weight:400}.srow-s{font-size:12px;color:var(--t3);margin-top:2px}

/* Toast */
#tst{visibility:hidden;position:fixed;bottom:70px;left:50%;transform:translateX(-50%);
background:var(--s2);backdrop-filter:blur(16px);color:var(--t1);padding:12px 28px;border-radius:14px;
font-size:14px;z-index:300;box-shadow:0 8px 32px rgba(0,0,0,.5);border:1px solid rgba(255,255,255,.04)}
#tst.sh{visibility:visible;animation:ti .3s,tout .3s 2.5s}
@keyframes ti{from{opacity:0;transform:translateX(-50%) translateY(14px)}to{opacity:1;transform:translateX(-50%)}}
@keyframes tout{from{opacity:1}to{opacity:0}}
</style></head><body>

<!-- WALLPAPER -->
<div class="wall"><div class="orb o1"></div><div class="orb o2"></div><div class="orb o3"></div></div>

<!-- STATUS BAR -->
<div class="sb">
<div class="sb-l"><span style="font-weight:600;font-size:12px" id="sck">--:--</span></div>
<div class="sb-r">
<span style="font-size:10px;font-weight:600;color:var(--pri)" id="snt">--</span>
<div class="sb-sig" id="sig"><div class="b" style="height:3px"></div><div class="b" style="height:5px"></div><div class="b" style="height:8px"></div><div class="b" style="height:11px"></div></div>
<svg width="14" height="14" viewBox="0 0 24 24" fill="rgba(255,255,255,.7)"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3c-1.65-1.66-4.34-1.66-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg>
<div class="sb-bt-i"><div class="sb-bt-f" id="sbf" style="width:70%"></div></div>
</div>
</div>

<!-- HOME SCREEN -->
<div class="home" id="homeScr">
<div class="aag"><div class="aag-d" id="aagD">--</div><div class="aag-op" id="sop">--</div></div>
<div class="clk"><div class="clk-t" id="htim">--:--</div></div>

<div class="apps"><div class="agrid">
<div class="aico" onclick="openA('phone')"><div class="aicn" style="background:linear-gradient(135deg,#34a853,#1e8e3e)">📞</div><div class="aicl">Phone</div></div>
<div class="aico" onclick="openA('msg')"><div class="aicn" style="background:linear-gradient(135deg,#4285f4,#1967d2)">💬</div><div class="aicl">Messages</div></div>
<div class="aico" onclick="openA('browser')"><div class="aicn" style="background:linear-gradient(135deg,#ea4335,#c5221f)">🌐</div><div class="aicl">Chrome</div></div>
<div class="aico" onclick="openA('game')"><div class="aicn" style="background:linear-gradient(135deg,#fbbc04,#e37400)">🎮</div><div class="aicl">Games</div></div>
<div class="aico" onclick="openA('status')"><div class="aicn" style="background:linear-gradient(135deg,#5bb974,#0d652d)">📊</div><div class="aicl">Network</div></div>
<div class="aico" onclick="openA('wifi')"><div class="aicn" style="background:linear-gradient(135deg,#4fc3f7,#0277bd)">📶</div><div class="aicl">WiFi</div></div>
<div class="aico" onclick="openA('gps')"><div class="aicn" style="background:linear-gradient(135deg,#ff7043,#d84315)">📍</div><div class="aicl">Maps</div></div>
<div class="aico" onclick="openA('calc')"><div class="aicn" style="background:linear-gradient(135deg,#78909c,#37474f)">🧮</div><div class="aicl">Calculator</div></div>
<div class="aico" onclick="openA('terminal')"><div class="aicn" style="background:linear-gradient(135deg,#37474f,#263238)">⌨️</div><div class="aicl">Terminal</div></div>
<div class="aico" onclick="openA('settings')"><div class="aicn" style="background:linear-gradient(135deg,#78909c,#455a64)">⚙️</div><div class="aicl">Settings</div></div>
<div class="aico" onclick="openA('device')"><div class="aicn" style="background:linear-gradient(135deg,#ab47bc,#7b1fa2)">📱</div><div class="aicl">About</div></div>
<div class="aico" onclick="openA('modem')"><div class="aicn" style="background:linear-gradient(135deg,#26a69a,#00796b)">📡</div><div class="aicl">Modem</div></div>
</div></div>

<div class="srch" onclick="openA('browser')">
<svg viewBox="0 0 24 24" fill="none" stroke="rgba(255,255,255,.4)" stroke-width="2.5"><circle cx="10.5" cy="10.5" r="7"/><line x1="15.5" y1="15.5" x2="21" y2="21" stroke-linecap="round"/></svg>
<span>Search or type URL</span></div>

<div class="dock">
<div class="aico" onclick="openA('phone')"><div class="aicn" style="background:linear-gradient(135deg,#34a853,#1e8e3e)">📞</div></div>
<div class="aico" onclick="openA('msg')"><div class="aicn" style="background:linear-gradient(135deg,#4285f4,#1967d2)">💬</div></div>
<div class="aico" onclick="openA('browser')"><div class="aicn" style="background:linear-gradient(135deg,#ea4335,#c5221f)">🌐</div></div>
<div class="aico" onclick="openA('game')"><div class="aicn" style="background:linear-gradient(135deg,#fbbc04,#e37400)">🎮</div></div>
</div>
<div class="gpill"></div>
</div>

<!-- PHONE -->
<div class="asc" id="a-phone"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Phone</div></div>
<div class="abd">
<div class="mc"><div class="dd" id="dsp"></div>
<div class="kpd">
<button class="ky" onclick="kp('1')">1</button><button class="ky" onclick="kp('2')">2<span class="ks">ABC</span></button><button class="ky" onclick="kp('3')">3<span class="ks">DEF</span></button>
<button class="ky" onclick="kp('4')">4<span class="ks">GHI</span></button><button class="ky" onclick="kp('5')">5<span class="ks">JKL</span></button><button class="ky" onclick="kp('6')">6<span class="ks">MNO</span></button>
<button class="ky" onclick="kp('7')">7<span class="ks">PQRS</span></button><button class="ky" onclick="kp('8')">8<span class="ks">TUV</span></button><button class="ky" onclick="kp('9')">9<span class="ks">WXYZ</span></button>
<button class="ky" onclick="kp('*')">✱</button><button class="ky" onclick="kp('0')">0<span class="ks">+</span></button><button class="ky" onclick="kp('#')">#</button>
</div>
<div class="cab">
<button class="cbt" style="background:var(--errD)" onclick="dnum=dnum.slice(0,-1);tx('dsp',dnum)"><svg viewBox="0 0 24 24" style="width:20px;height:20px"><path d="M22 3H7c-.69 0-1.23.35-1.59.88L0 12l5.41 8.11c.36.53.9.89 1.59.89h15c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm-3 12.59L17.59 17 14 13.41 10.41 17 9 15.59 12.59 12 9 8.41 10.41 7 14 10.59 17.59 7 19 8.41 15.41 12 19 15.59z"/></svg></button>
<button class="cbt" style="background:#1e8e3e;box-shadow:0 4px 20px rgba(30,142,62,.35)" onclick="doCall()"><svg viewBox="0 0 24 24"><path d="M20.01 15.38c-1.23 0-2.42-.2-3.53-.56-.35-.12-.74-.03-1.01.24l-1.57 1.97c-2.83-1.35-5.48-3.9-6.89-6.83l1.95-1.66c.27-.28.35-.67.24-1.02-.37-1.11-.56-2.3-.56-3.53 0-.54-.45-.99-.99-.99H4.19C3.65 3 3 3.24 3 3.99 3 13.28 10.73 21 20.01 21c.71 0 .99-.63.99-1.18v-3.45c0-.54-.45-.99-.99-.99z"/></svg></button>
<button class="cbt" style="background:var(--errD)" onclick="doHang()"><svg viewBox="0 0 24 24"><path d="M12 9c-1.6 0-3.15.25-4.6.72v3.1c0 .39-.23.74-.56.9-.98.49-1.87 1.12-2.66 1.85-.18.18-.43.28-.7.28-.28 0-.53-.11-.71-.29L.29 13.08a1 1 0 01-.29-.7c0-.28.11-.53.29-.71C3.34 8.78 7.46 7 12 7s8.66 1.78 11.71 4.67c.18.18.29.43.29.71 0 .28-.11.53-.29.71l-2.48 2.48c-.18.18-.43.29-.71.29-.27 0-.52-.11-.7-.28-.79-.74-1.69-1.36-2.67-1.85-.33-.16-.56-.5-.56-.9v-3.1C15.15 9.25 13.6 9 12 9z"/></svg></button>
</div></div>
<div class="mc"><div class="fb mb"><span class="mct" style="margin:0">Active Call</span><button class="bt bs bsm" onclick="syncC()">Refresh</button></div>
<div class="g2"><div class="mi"><div class="mil">State</div><div class="miv s" id="cs">Idle</div></div><div class="mi"><div class="mil">Number</div><div class="miv s" id="cn">--</div></div></div>
<div class="fb mt" style="gap:8px"><button class="bt bs bsm" onclick="doVol(-10)">Vol −</button><span id="vd" style="font-weight:600;min-width:38px;text-align:center">--</span><button class="bt bs bsm" onclick="doVol(10)">Vol +</button><button class="bt bg bsm" onclick="doAns()">Answer</button></div>
</div></div></div>

<!-- MESSAGES -->
<div class="asc" id="a-msg"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Messages</div><button class="bt bp bsm" style="margin-right:14px" onclick="togC()">+ New</button></div>
<div class="abd"><div class="fb mb"><span></span><button class="bt bs bsm" onclick="ldSms()">↻ Refresh</button></div><div id="sml"><div class="tc tm" style="padding:40px 0">Tap ↻ to load</div></div></div></div>

<!-- BROWSER -->
<div class="asc" id="a-browser"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Browser</div></div>
<div class="abd">
<div style="display:flex;gap:8px;margin-bottom:14px"><input type="text" id="burl" placeholder="google.com" style="border-radius:28px;padding-left:20px;flex:1" onkeydown="if(event.key==='Enter')doGo()"><button class="bt bp bsm" style="border-radius:24px" onclick="doGo()">Go</button></div>
<div id="bst" style="font-size:13px;color:var(--t3);margin-bottom:10px">Type a website — APN auto-detected</div>
<div class="bvw" id="bvw" style="display:none"></div>
<div class="mc mt"><div class="mct">Manual APN (if auto fails)</div>
<div class="g2 mb"><input type="text" id="papn" placeholder="APN"><input type="text" id="pusr" placeholder="User"></div>
<div class="g2 mb"><input type="password" id="ppas" placeholder="Password"><select id="paut"><option value="0">None</option><option value="1">PAP</option><option value="2">CHAP</option></select></div>
<button class="bt bp" onclick="doPdp()">Activate Data</button></div></div></div>

<!-- GAMES -->
<div class="asc" id="a-game"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Games</div></div>
<div class="abd">
<div class="mc"><div class="fb mb"><span class="mct" style="margin:0">🐍 Snake</span><div><span style="font-size:28px;font-weight:700;color:var(--pri)" id="gsc">0</span> <span style="font-size:12px;color:var(--t3)" id="ghi">Best: 0</span></div></div>
<div style="display:flex;flex-direction:column;align-items:center;gap:14px">
<canvas id="snk" width="280" height="280"></canvas>
<div class="gp"><div></div><button class="gb" onclick="sd(0,-1)">▲</button><div></div><button class="gb" onclick="sd(-1,0)">◀</button><button class="gb" onclick="snkGo()" id="gpl" style="background:var(--priD);color:var(--pri)">▶</button><button class="gb" onclick="sd(1,0)">▶</button><div></div><button class="gb" onclick="sd(0,1)">▼</button><div></div></div></div></div></div></div>

<!-- CALC -->
<div class="asc" id="a-calc"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Calculator</div></div>
<div class="abd"><div class="mc">
<input type="text" id="cdi" readonly value="0" style="font-size:36px;text-align:right;font-family:monospace;margin-bottom:14px;background:none;border:none;color:var(--t1)">
<div style="display:grid;grid-template-columns:repeat(4,1fr);gap:8px">
<button class="gb" onclick="cc('C')" style="color:var(--err)">C</button><button class="gb" onclick="cc('(')">( )</button><button class="gb" onclick="cc('%')">%</button><button class="gb" onclick="cc('/')" style="color:var(--pri)">÷</button>
<button class="gb" onclick="cc('7')">7</button><button class="gb" onclick="cc('8')">8</button><button class="gb" onclick="cc('9')">9</button><button class="gb" onclick="cc('*')" style="color:var(--pri)">×</button>
<button class="gb" onclick="cc('4')">4</button><button class="gb" onclick="cc('5')">5</button><button class="gb" onclick="cc('6')">6</button><button class="gb" onclick="cc('-')" style="color:var(--pri)">−</button>
<button class="gb" onclick="cc('1')">1</button><button class="gb" onclick="cc('2')">2</button><button class="gb" onclick="cc('3')">3</button><button class="gb" onclick="cc('+')" style="color:var(--pri)">+</button>
<button class="gb" onclick="cc('0')" style="grid-column:span 2">0</button><button class="gb" onclick="cc('.')">.</button><button class="gb" onclick="cc('=')" style="background:var(--priD);color:var(--pri)">=</button>
</div></div></div></div>

<!-- NETWORK STATUS -->
<div class="asc" id="a-status"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Network</div></div>
<div class="abd">
<div class="mc"><div class="mct">Cellular · EC200U</div><div class="mg"><div class="mi"><div class="mil">Signal</div><div class="miv" id="ws">--</div></div><div class="mi"><div class="mil">Network</div><div class="miv s" id="wn">--</div></div><div class="mi"><div class="mil">Operator</div><div class="miv s" id="wo">--</div></div><div class="mi"><div class="mil">Status</div><div class="miv s" id="wr">--</div></div></div></div>
<div class="mc"><div class="mct">Battery</div><div class="mg"><div class="mi"><div class="mil">Level</div><div class="miv" id="wb">--</div></div><div class="mi"><div class="mil">Voltage</div><div class="miv s" id="wvt">--</div></div></div></div>
<div class="mc"><div class="mct">Identity</div><div class="mg"><div class="mi" style="grid-column:span 2"><div class="mil">IMEI</div><div class="miv s" id="wim" style="font-size:13px;letter-spacing:1px">--</div></div><div class="mi" style="grid-column:span 2"><div class="mil">Model</div><div class="miv s" id="wmo">--</div></div><div class="mi" style="grid-column:span 2"><div class="mil">SIM</div><div class="miv s" id="wsi">--</div></div></div></div>
</div></div>

<!-- WIFI -->
<div class="asc" id="a-wifi"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">WiFi</div><button class="bt bp bsm" style="margin-right:14px" onclick="wScan()">Scan</button></div>
<div class="abd"><div id="wfl"><div class="tc tm" style="padding:40px 0">Tap Scan to find networks</div></div></div></div>

<!-- GPS -->
<div class="asc" id="a-gps"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Location</div></div>
<div class="abd"><div class="mc"><div class="fb mb"><span class="mct" style="margin:0">GPS</span><button class="bt bp bsm" onclick="doGPS()">Get Location</button></div>
<div class="g2"><div class="mi"><div class="mil">Lat</div><div class="miv s" id="wla">--</div></div><div class="mi"><div class="mil">Lon</div><div class="miv s" id="wlo">--</div></div></div></div></div></div>

<!-- TERMINAL -->
<div class="asc" id="a-terminal"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Terminal</div></div>
<div class="abd"><div class="mc"><div class="tmo" id="tmo"></div>
<div style="display:flex;gap:8px"><input type="text" id="atc" placeholder="AT command" style="font-family:monospace" onkeydown="if(event.key==='Enter')doAT()"><button class="bt bp bsm" onclick="doAT()">Send</button><button class="bt bs bsm" onclick="E('tmo').innerHTML=''">Clear</button></div></div></div></div>

<!-- SETTINGS -->
<div class="asc" id="a-settings"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Settings</div></div>
<div class="abd">
<div class="mc"><div class="mct">Connectivity</div>
<div class="srow"><div><div class="srow-t">Mobile Data (SIM)</div><div class="srow-s" id="apnI">APN: detecting...</div></div><button class="bt bp bsm" onclick="actD()" id="dBtn">Activate</button></div>
<div class="srow"><div><div class="srow-t">WiFi Hotspot</div><div class="srow-s">ESP32 captive portal</div></div><span style="color:var(--gn);font-size:13px;font-weight:600">Active</span></div></div>
<div class="mc"><div class="mct">WiFi Client</div><input type="text" id="wss" placeholder="SSID" class="mb"><input type="password" id="wps" placeholder="Password" class="mb">
<div class="g2"><button class="bt bp" onclick="saveW()">Save & Connect</button><button class="bt bd" onclick="fgW()">Forget</button></div></div>
<div class="mc"><div class="mct">Modem · EC200U</div><div class="g2"><button class="bt bg" onclick="mPow(1)">⚡ Power On</button><button class="bt bd" onclick="mPow(0)">Power Off</button></div>
<button class="bt bs mt" onclick="doRbt()">🔄 Reboot ESP32</button></div></div></div>

<!-- ABOUT -->
<div class="asc" id="a-device"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">About</div></div>
<div class="abd"><div class="mc"><div class="mct">ESP32</div><div id="einf" class="tm">Loading...</div></div>
<div class="mc"><div class="mct">EC200U</div><div id="minf" class="tm">Loading...</div></div></div></div>

<!-- MODEM -->
<div class="asc" id="a-modem"><div class="ahd"><button class="abk" onclick="closeA()"><svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg></button><div class="atl">Modem</div></div>
<div class="abd"><div class="mc"><div class="mct">Power</div><div class="g2"><button class="bt bg" onclick="mPow(1)">⚡ Power On</button><button class="bt bd" onclick="mPow(0)">Power Off</button></div></div>
<div class="mc"><div class="mct">Info</div><div id="minf2" class="tm">Loading...</div></div></div></div>

<!-- COMPOSE -->
<div class="csh" id="csh"><div class="csh-h" onclick="togC()"></div>
<div style="font-size:18px;font-weight:400;margin-bottom:14px">New Message</div>
<input type="tel" id="sto" placeholder="Recipient" class="mb"><textarea id="stx" rows="3" placeholder="Message" class="mb"></textarea>
<div class="g2"><button class="bt bp" onclick="doSnd()">Send</button><button class="bt bs" onclick="togC()">Cancel</button></div></div>

<div id="tst"></div>

<script>
const E=id=>document.getElementById(id),tx=(id,v)=>{const e=E(id);if(e)e.innerText=v},
eh=s=>(s||'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]||c));
function toast(m){const t=E('tst');t.innerText=m;t.className='sh';setTimeout(()=>t.className='',3000)}
async function api(u,d=null){try{const o={method:d?'POST':'GET',headers:{'Content-Type':'application/json'}};
if(d)o.body=JSON.stringify(d);return await(await fetch(u,o)).json()}catch(e){toast('Connection error');return null}}
function uc(){const n=new Date(),t=n.toLocaleTimeString([],{hour:'2-digit',minute:'2-digit'}),
d=n.toLocaleDateString([],{weekday:'long',month:'long',day:'numeric'});tx('sck',t);tx('htim',t);tx('aagD',d)}
setInterval(uc,1000);uc();
let curA=null;function openA(id){const el=E('a-'+id);if(el){el.classList.add('on');curA=id;
if(id==='status')rSt();if(id==='device'){ldE();ldM()}if(id==='modem')ldM2()}}
function closeA(){if(curA){E('a-'+curA).classList.remove('on');curA=null}}
function uSig(p){E('sig').querySelectorAll('.b').forEach((b,i)=>b.classList.toggle('on',i<(p>75?4:p>50?3:p>25?2:p>0?1:0)))}
function uBat(p){const f=E('sbf');f.style.width=p+'%';f.style.background=p<20?'var(--err)':p<50?'var(--yl)':'var(--gn)'}
async function rSt(){const d=await api('/api/status');if(!d)return;tx('sop',d.operator?(d.operator+' · '+(d.net_type||'')):'--');
tx('snt',d.net_type||'');uSig(d.signal||0);tx('ws',(d.signal||0)+'%');tx('wn',d.net_type||'--');tx('wo',d.operator||'--');
tx('wr',d.registration||'--');tx('wim',d.imei||'--');tx('wmo',d.model||'--');tx('wsi',d.sim_status||'--');
if(d.battery&&d.battery.valid){uBat(d.battery.percent);tx('wb',d.battery.percent+'%');tx('wvt',(d.battery.voltage||0).toFixed(2)+' V')}
const ai=E('apnI');if(ai&&d.operator)ai.innerText='APN: auto ('+d.operator+')'}
let dnum='';function kp(k){dnum+=k;tx('dsp',dnum)}
async function doCall(){if(!dnum)return toast('Enter number');toast('Calling...');const r=await api('/api/call/dial',{number:dnum});toast(r&&r.success?'Calling':'Failed');syncC()}
async function doHang(){await api('/api/call/hangup',{});toast('Ended');syncC()}
async function doAns(){await api('/api/call/answer',{});toast('Answered');syncC()}
async function doVol(d){const r=await api('/api/call/volume',{delta:d});if(r&&r.success)tx('vd',r.level+'%')}
async function syncC(){const r=await api('/api/call/status');if(!r)return;const e=r.entries&&r.entries.length?r.entries[0]:null;
tx('cs',e?e.state:'Idle');tx('cn',e?e.number:'--');if(typeof r.speaker_volume==='number')tx('vd',r.speaker_volume+'%')}
let compO=false;
async function ldSms(){const l=E('sml');l.innerHTML='<div class="tc tm" style="padding:40px 0">Loading...</div>';
const r=await api('/api/sms/list');if(r&&r.success&&r.messages){l.innerHTML=r.messages.length?r.messages.map(m=>
`<div class="smi"><div class="sms">${eh(m.sender||'Unknown')}</div><div class="smd">${eh(m.date||'')}</div>
<div class="smb">${eh(m.text||'')}</div><div style="text-align:right;margin-top:8px">
<button class="bt bd bsm" onclick="delS(${m.index})">Delete</button></div></div>`).join('')
:'<div class="tc tm" style="padding:40px 0">No messages</div>'}else l.innerHTML='<div class="tc" style="padding:40px 0;color:var(--err)">Failed</div>'}
async function delS(i){if(!confirm('Delete?'))return;await api('/api/sms/delete',{index:i});toast('Deleted');ldSms()}
function togC(){compO=!compO;E('csh').classList.toggle('op',compO)}
async function doSnd(){const n=E('sto').value,t=E('stx').value;if(!n||!t)return toast('Fill in fields');toast('Sending...');
const r=await api('/api/sms/send',{number:n,text:t});if(r&&r.success){toast('Sent!');E('stx').value='';togC();ldSms()}else toast('Failed')}
async function doGo(){let u=E('burl').value.trim();if(!u)return toast('Type a website');
tx('bst','⏳ Activating data & connecting...');E('bvw').style.display='none';
const r=await api('/api/browse',{url:u});
if(r&&r.success){tx('bst','✅ '+(r.url||u)+(r.apn_used?' · APN: '+r.apn_used:''));E('bvw').style.display='block';E('bvw').innerText=r.content||'(empty)';toast('Page loaded')}
else{tx('bst','❌ '+(r&&r.error?r.error:'Connection failed'));toast(r&&r.error||'Browse failed')}}
async function actD(){E('dBtn').innerText='Connecting...';tx('apnI','Activating...');
const r=await api('/api/data/status');
if(r){tx('apnI','APN: '+r.apn+' ('+r.apn_source+') · '+r.operator)}
const r2=await api('/api/pdp/activate',{ctxId:1,persist:true});
if(r2&&r2.success){toast('Mobile data active!');E('dBtn').innerText='✅ Active';E('dBtn').style.background='var(--gnD)'}
else{toast(r2&&r2.error||'Failed');E('dBtn').innerText='Retry'}}
async function doPdp(){toast('Activating...');const pl={ctxId:1,persist:true};const a=E('papn').value.trim();if(a)pl.apn=a;
const r=await api('/api/pdp/activate',pl);toast(r&&r.success?'Active!':(r&&r.error||'Failed'))}
async function doGPS(){tx('wla','...');tx('wlo','...');const r=await api('/api/gps/location');
if(r&&r.success){tx('wla',r.lat);tx('wlo',r.lon)}else{tx('wla','N/A');tx('wlo','N/A');toast('GPS failed')}}
function tLog(t,ty='rx'){const o=E('tmo');o.innerHTML+=`<div><span style="color:var(--t3);font-size:10px">[${new Date().toLocaleTimeString()}]</span> <span style="color:${ty==='tx'?'var(--pri)':'var(--gn)'}">${ty==='tx'?'> ':''}${eh(t)}</span></div>`;o.scrollTop=o.scrollHeight}
async function doAT(){const c=E('atc').value;if(!c)return;tLog(c,'tx');E('atc').value='';const r=await api('/api/at',{cmd:c});if(r&&r.response)tLog(r.response.replace(/\r/g,''))}
async function wScan(){const l=E('wfl');l.innerHTML='<div class="tc tm" style="padding:20px 0">Scanning...</div>';
const r=await api('/api/wifi/scan');if(r&&r.success&&r.networks){l.innerHTML=r.networks.length?r.networks.map(n=>
`<div class="wfi"><div><div style="font-weight:500;font-size:15px">${eh(n.ssid||'(hidden)')}</div>
<div style="font-size:12px;color:var(--t3)">Ch ${n.ch} · ${n.enc?'🔒':'🔓'}</div></div>
<div style="font-weight:600;color:${n.rssi>-50?'var(--gn)':n.rssi>-70?'var(--yl)':'var(--err)'}">${n.rssi}dBm</div></div>`).join('')
:'<div class="tc tm" style="padding:20px 0">No networks</div>'}else l.innerHTML='<div class="tc" style="color:var(--err)">Scan failed</div>'}
async function saveW(){const s=E('wss').value;if(!s)return toast('Enter SSID');toast('Saving...');const r=await api('/api/wifi/save',{ssid:s,pass:E('wps').value});toast(r&&r.success?r.message||'Saved':'Failed')}
async function fgW(){if(!confirm('Forget WiFi?'))return;await api('/api/wifi/forget',{});toast('Forgotten')}
async function mPow(on){toast(on?'Powering on...':'Powering off...');const r=await api(on?'/api/modem/poweron':'/api/modem/poweroff',{});toast(r&&r.success?(r.message||'Done'):'Failed');if(on)setTimeout(rSt,3000)}
async function doRbt(){if(!confirm('Reboot ESP32?'))return;await api('/api/esp/reboot',{});toast('Rebooting...')}
async function ldE(){const r=await api('/api/esp/info');if(r&&r.success)E('einf').innerHTML=
`<div><b>Chip:</b> ${eh(r.chip||'--')}</div><div><b>Cores:</b> ${r.cores||'--'} @ ${r.freq||'--'}MHz</div>
<div><b>Heap:</b> ${r.heap_free?Math.round(r.heap_free/1024)+'KB':'--'}</div>
<div><b>Flash:</b> ${r.flash_size?Math.round(r.flash_size/1048576)+'MB':'--'}</div>
<div><b>WiFi:</b> ${eh(r.wifi_mode||'--')} · ${r.ap_clients||0} clients</div>`}
async function ldM(){const r=await api('/api/modem/info');if(r)E('minf').innerHTML=
`<div><b>Model:</b> ${eh(r.model||'--')}</div><div><b>Mfr:</b> ${eh(r.manufacturer||'--')}</div>
<div><b>IMEI:</b> ${eh(r.imei||'--')}</div><div><b>FW:</b> ${eh(r.firmware||'--')}</div>`}
async function ldM2(){const r=await api('/api/modem/info');if(r)E('minf2').innerHTML=
`<div><b>Model:</b> ${eh(r.model||'--')}</div><div><b>IMEI:</b> ${eh(r.imei||'--')}</div><div><b>FW:</b> ${eh(r.firmware||'--')}</div>`}
let sg=null,shi=0,sdr={x:1,y:0};
function snkGo(){const c=E('snk'),x=c.getContext('2d'),G=14,W=20,H=20;let s=[{x:10,y:10}],f,sc=0,run=true;sdr={x:1,y:0};
function pf(){f={x:Math.floor(Math.random()*W),y:Math.floor(Math.random()*H)}}pf();if(sg)clearInterval(sg);
sg=setInterval(()=>{if(!run)return;let h={x:s[0].x+sdr.x,y:s[0].y+sdr.y};
if(h.x<0||h.x>=W||h.y<0||h.y>=H||s.some(p=>p.x===h.x&&p.y===h.y)){run=false;clearInterval(sg);sg=null;
if(sc>shi){shi=sc;tx('ghi','Best: '+shi)}E('gpl').innerText='▶';return}
s.unshift(h);if(h.x===f.x&&h.y===f.y){sc++;tx('gsc',sc);pf()}else s.pop();
x.fillStyle='#0d1117';x.fillRect(0,0,c.width,c.height);x.fillStyle='var(--gn)';
s.forEach((p,i)=>{x.globalAlpha=Math.max(.3,1-i*.02);x.beginPath();x.roundRect(p.x*G+1,p.y*G+1,G-2,G-2,[3]);x.fill()});
x.globalAlpha=1;x.fillStyle='var(--err)';x.beginPath();x.arc(f.x*G+G/2,f.y*G+G/2,G/2-2,0,Math.PI*2);x.fill()},110);
E('gpl').innerText='⏸';tx('gsc','0')}
function sd(dx,dy){if((dx&&sdr.x!==-dx)||(dy&&sdr.y!==-dy))sdr={x:dx,y:dy}}
document.addEventListener('keydown',e=>{if(e.key==='ArrowUp')sd(0,-1);else if(e.key==='ArrowDown')sd(0,1);else if(e.key==='ArrowLeft')sd(-1,0);else if(e.key==='ArrowRight')sd(1,0)});
let cx='';function cc(v){const d=E('cdi');if(v==='C'){cx='';d.value='0'}else if(v==='='){try{d.value=Function('return '+cx)()}catch(e){d.value='Error'}cx=''}else{cx+=v;d.value=cx}}
rSt();syncC();setInterval(rSt,6000);setInterval(syncC,5000);
(()=>{const c=E('snk');if(c){const x=c.getContext('2d');x.fillStyle='#0d1117';x.fillRect(0,0,280,280);x.fillStyle='#5f6368';x.font='500 15px system-ui';x.textAlign='center';x.fillText('Tap \u25b6 to play',140,140)}})();
</script></body></html>
)rawliteral";

#endif
