#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Quectel Manager</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-color: #0f172a;
            --text-color: #e2e8f0;
            --accent-color: #38bdf8;
            --success-color: #4ade80;
            --error-color: #f87171;
            --font-main: 'Inter', sans-serif;
            --font-mono: 'JetBrains Mono', monospace;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            -webkit-tap-highlight-color: transparent;
        }

        body {
            background-color: var(--bg-color);
            color: var(--text-primary);
            font-family: var(--font-main);
            min-height: 100vh;
            background-image: 
                radial-gradient(at 0% 0%, rgba(56, 189, 248, 0.1) 0px, transparent 50%),
                radial-gradient(at 100% 100%, rgba(139, 92, 246, 0.1) 0px, transparent 50%);
            background-attachment: fixed;
            padding: 20px;
            padding-bottom: 80px;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
        }

        /* Header */
        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
            padding: 15px 20px;
            background: var(--card-bg);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border-radius: 16px;
            border: 1px solid var(--card-border);
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
        }

        h1 {
            font-size: 1.25rem;
            font-weight: 600;
            background: linear-gradient(135deg, #38bdf8, #818cf8);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            letter-spacing: -0.02em;
        }

        /* Navigation */
        .nav-scroll {
            overflow-x: auto;
            margin: 0 -20px 20px -20px;
            padding: 0 20px;
            -ms-overflow-style: none;
            scrollbar-width: none;
        }
        .nav-scroll::-webkit-scrollbar { display: none; }

        .nav-tabs {
            display: flex;
            gap: 8px;
            padding-bottom: 5px;
        }

        .nav-tab {
            padding: 10px 18px;
            background: rgba(255, 255, 255, 0.03);
            border: 1px solid var(--card-border);
            border-radius: 12px;
            cursor: pointer;
            white-space: nowrap;
            font-size: 0.9rem;
            font-weight: 500;
            color: var(--text-secondary);
            transition: all 0.2s ease;
        }

        .nav-tab.active {
            background: rgba(56, 189, 248, 0.15);
            border-color: rgba(56, 189, 248, 0.3);
            color: var(--accent-color);
            box-shadow: 0 0 15px var(--accent-glow);
        }

        /* Grid Layout */
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
        }

        /* Cards */
        .card {
            background: var(--card-bg);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border-radius: 20px;
            padding: 24px;
            border: 1px solid var(--card-border);
            box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
            transition: transform 0.2s ease;
        }

        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 20px;
        }

        .card-title {
            font-size: 1.1rem;
            font-weight: 600;
            color: var(--text-primary);
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .card-title::before {
            content: '';
            display: block;
            width: 4px;
            height: 16px;
            background: var(--accent-color);
            border-radius: 2px;
            box-shadow: 0 0 8px var(--accent-color);
        }

        /* Status Grid */
        .status-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 12px;
        }

        .status-item {
            background: rgba(0, 0, 0, 0.2);
            padding: 16px;
            border-radius: 14px;
            text-align: center;
            border: 1px solid rgba(255, 255, 255, 0.03);
        }

        .status-label {
            font-size: 0.75rem;
            color: var(--text-secondary);
            margin-bottom: 6px;
            text-transform: uppercase;
            letter-spacing: 0.05em;
        }

        .status-value {
            font-size: 1.1rem;
            font-weight: 600;
            color: var(--text-primary);
        }

        /* Forms */
        input, select, textarea {
            width: 100%;
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid var(--card-border);
            padding: 14px;
            border-radius: 12px;
            color: var(--text-primary);
            margin-bottom: 12px;
            font-family: var(--font-main);
            font-size: 0.95rem;
            transition: all 0.2s;
        }

        input:focus, select:focus, textarea:focus {
            outline: none;
            border-color: var(--accent-color);
            box-shadow: 0 0 0 2px rgba(56, 189, 248, 0.1);
        }

        button {
            width: 100%;
            padding: 14px;
            border-radius: 12px;
            border: none;
            background: linear-gradient(135deg, #38bdf8, #3b82f6);
            color: white;
            font-weight: 600;
            font-size: 0.95rem;
            cursor: pointer;
            transition: all 0.2s;
            box-shadow: 0 4px 12px rgba(56, 189, 248, 0.2);
        }

        button:active {
            transform: scale(0.98);
        }

        button.secondary {
            background: rgba(255, 255, 255, 0.08);
            color: var(--text-primary);
            box-shadow: none;
        }

        button.danger {
            background: linear-gradient(135deg, #f87171, #ef4444);
            box-shadow: 0 4px 12px rgba(239, 68, 68, 0.2);
        }

        /* Terminal */
        .terminal-container {
            position: relative;
        }

        .terminal-output {
            background: #0d1117;
            color: #4ade80;
            font-family: var(--font-mono);
            padding: 16px;
            border-radius: 12px;
            height: 300px;
            overflow-y: auto;
            margin-bottom: 15px;
            font-size: 0.85rem;
            line-height: 1.5;
            border: 1px solid var(--card-border);
            white-space: pre-wrap;
            word-break: break-all;
        }

        .terminal-output::-webkit-scrollbar {
            width: 8px;
        }
        .terminal-output::-webkit-scrollbar-track {
            background: #0d1117;
        }
        .terminal-output::-webkit-scrollbar-thumb {
            background: #333;
            border-radius: 4px;
        }

        .terminal-controls {
            display: flex;
            gap: 10px;
        }

        /* Sections */
        .section {
            display: none;
            animation: fadeIn 0.3s ease;
        }
        .section.active { display: block; }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        /* Toast */
        #toast {
            visibility: hidden;
            min-width: 280px;
            background: rgba(15, 23, 42, 0.95);
            backdrop-filter: blur(10px);
            color: #fff;
            text-align: center;
            border-radius: 50px;
            padding: 12px 24px;
            position: fixed;
            z-index: 100;
            left: 50%;
            bottom: 30px;
            transform: translateX(-50%);
            font-size: 0.95rem;
            box-shadow: 0 10px 25px rgba(0,0,0,0.3);
            border: 1px solid var(--card-border);
        }
        #toast.show { visibility: visible; animation: slideUp 0.3s, fadeOut 0.3s 2.7s; }

        @keyframes slideUp {
            from { bottom: 0; opacity: 0; }
            to { bottom: 30px; opacity: 1; }
        }
        @keyframes fadeOut {
            from { bottom: 30px; opacity: 1; }
            to { bottom: 0; opacity: 0; }
        }

        /* Responsive Adjustments */
        @media (max-width: 600px) {
            body { padding: 15px; }
            .grid { grid-template-columns: 1fr; }
            .status-grid { grid-template-columns: 1fr 1fr; }
            h1 { font-size: 1.1rem; }
            .card { padding: 20px; }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Quectel Manager</h1>
            <div id="connection-status" style="display: flex; align-items: center; gap: 6px; font-size: 0.85rem; color: var(--success-color);">
                <span style="width: 8px; height: 8px; background: var(--success-color); border-radius: 50%; box-shadow: 0 0 8px var(--success-color);"></span>
                Online
            </div>
        </header>

        <div class="nav-scroll">
            <div class="nav-tabs">
                <div class="nav-tab active" onclick="showSection('dashboard', this)">Dashboard</div>
                <div class="nav-tab" onclick="showSection('sms', this)">SMS</div>
                <div class="nav-tab" onclick="showSection('gps', this)">GPS</div>
                <div class="nav-tab" onclick="showSection('phone', this)">Phone</div>
                <div class="nav-tab" onclick="showSection('terminal', this)">Terminal</div>
                <div class="nav-tab" onclick="showSection('tcp', this)">TCP</div>
                <div class="nav-tab" onclick="showSection('settings', this)">Settings</div>
            </div>
        </div>

        <!-- Dashboard -->
        <div id="dashboard" class="section active">
            <div class="grid">
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Network Status</div>
                    </div>
                    <div class="status-grid">
                        <div class="status-item">
                            <div class="status-label">Signal</div>
                            <div class="status-value" id="signal-val">--%</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Type</div>
                            <div class="status-value" id="net-type">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Operator</div>
                            <div class="status-value" id="operator">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">SIM</div>
                            <div class="status-value" id="sim-status">--</div>
                        </div>
                    </div>
                </div>
                
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Device Info</div>
                    </div>
                    <div class="status-grid">
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">IMEI</div>
                            <div class="status-value" id="imei" style="font-family: var(--font-mono); font-size: 0.9rem;">--</div>
                        </div>
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Model</div>
                            <div class="status-value" id="model">--</div>
                        </div>
                    </div>
                    <button onclick="refreshStatus()" class="secondary" style="margin-top: 15px;">
                        <span style="margin-right: 5px;">↻</span> Refresh Status
                    </button>
                </div>
            </div>
        </div>

        <!-- SMS -->
        <div id="sms" class="section">
            <div class="grid">
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Compose SMS</div>
                    </div>
                    <input type="tel" id="sms-number" placeholder="Recipient Number">
                    <textarea id="sms-text" rows="4" placeholder="Type your message here..."></textarea>
                    <button onclick="sendSMS()">Send Message</button>
                </div>
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Inbox Reader</div>
                    </div>
                    <div style="display: flex; gap: 10px; margin-bottom: 15px;">
                        <input type="number" id="sms-index" placeholder="Index ID" style="margin-bottom: 0;">
                        <button onclick="readSMS()" class="secondary" style="width: auto;">Read</button>
                    </div>
                    <div id="sms-content" class="terminal-output" style="height: 120px; color: var(--text-secondary);">
                        No message loaded
                    </div>
                </div>
            </div>
        </div>

        <!-- GPS -->
        <div id="gps" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">GNSS Location</div>
                </div>
                <div class="status-grid" style="margin-bottom: 20px;">
                    <div class="status-item">
                        <div class="status-label">Latitude</div>
                        <div class="status-value" id="gps-lat">--</div>
                    </div>
                    <div class="status-item">
                        <div class="status-label">Longitude</div>
                        <div class="status-value" id="gps-lon">--</div>
                    </div>
                </div>
                <button onclick="getGPS()">Get Current Location</button>
                <div style="margin-top: 15px; font-size: 0.8rem; color: var(--text-secondary); text-align: center; background: rgba(0,0,0,0.2); padding: 10px; border-radius: 8px;">
                    Ensure GNSS antenna is connected with clear sky view.
                </div>
            </div>
        </div>

        <!-- Phone -->
        <div id="phone" class="section">
            <div class="card" style="max-width: 400px; margin: 0 auto;">
                <div class="card-header">
                    <div class="card-title">Phone Dialer</div>
                </div>
                <input type="tel" id="phone-number" placeholder="+1234567890" style="font-size: 1.5rem; text-align: center; letter-spacing: 1px; font-family: var(--font-mono);">
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 15px;">
                    <button onclick="dialNumber()" style="background: var(--success-color); box-shadow: 0 4px 12px rgba(74, 222, 128, 0.2);">Call</button>
                    <button onclick="hangupCall()" class="danger">End Call</button>
                </div>
            </div>
        </div>

        <!-- Terminal -->
        <div id="terminal" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">AT Command Terminal</div>
                    <button onclick="clearTerminal()" class="secondary" style="width: auto; padding: 8px 16px; font-size: 0.8rem;">Clear</button>
                </div>
                <div id="term-output" class="terminal-output"></div>
                <div class="terminal-controls">
                    <input type="text" id="at-cmd" placeholder="Enter AT Command (e.g. ATI)" style="margin-bottom: 0; font-family: var(--font-mono);" onkeypress="handleEnter(event)">
                    <button onclick="sendAT()" style="width: auto;">Send</button>
                </div>
            </div>
        </div>

        <!-- TCP -->
        <div id="tcp" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">TCP Client</div>
                </div>
                <div class="grid" style="grid-template-columns: 2fr 1fr; gap: 10px; margin-bottom: 15px;">
                    <input type="text" id="tcp-host" placeholder="Host (e.g. google.com)" style="margin-bottom: 0;">
                    <input type="number" id="tcp-port" placeholder="Port" value="80" style="margin-bottom: 0;">
                </div>
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px;">
                    <button onclick="tcpOpen()">Connect</button>
                    <button onclick="tcpClose()" class="danger">Disconnect</button>
                </div>
                <textarea id="tcp-data" rows="3" placeholder="Data payload (e.g. GET / HTTP/1.1\r\nHost: google.com\r\n\r\n)" style="font-family: var(--font-mono); font-size: 0.85rem;"></textarea>
                <button onclick="tcpSend()" class="secondary">Send Data</button>
                <div style="margin-top: 20px;">
                    <div class="status-label">Server Response</div>
                    <div id="tcp-response" class="terminal-output" style="height: 150px;"></div>
                </div>
            </div>
        </div>
        <!-- Settings -->
        <div id="settings" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">WiFi Configuration</div>
                </div>
                <div style="margin-bottom: 15px; color: var(--text-secondary); font-size: 0.9rem;">
                    Configure a WiFi network for the device to connect to. If connection fails, it will revert to Hotspot mode.
                </div>
                <input type="text" id="wifi-ssid" placeholder="WiFi SSID">
                <input type="password" id="wifi-pass" placeholder="WiFi Password">
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-top: 10px;">
                    <button onclick="saveWifi()">Save & Connect</button>
                    <button onclick="forgetWifi()" class="danger">Forget WiFi</button>
                </div>
            </div>
        </div>
    </div>

    <div id="toast">Notification</div>

    <script>
        function showSection(id, tab) {
            document.querySelectorAll('.section').forEach(el => el.classList.remove('active'));
            document.getElementById(id).classList.add('active');
            document.querySelectorAll('.nav-tab').forEach(el => el.classList.remove('active'));
            tab.classList.add('active');
        }

        function showToast(msg) {
            const x = document.getElementById("toast");
            x.innerText = msg;
            x.className = "show";
            setTimeout(() => { x.className = x.className.replace("show", ""); }, 3000);
        }

        function handleEnter(e) {
            if (e.key === 'Enter') sendAT();
        }

        function clearTerminal() {
            document.getElementById('term-output').innerHTML = '';
        }

        function logToTerminal(text, type = 'rx') {
            const term = document.getElementById('term-output');
            const timestamp = new Date().toLocaleTimeString();
            const prefix = type === 'tx' ? '> ' : '';
            const color = type === 'tx' ? '#38bdf8' : '#4ade80';
            
            const line = `<div style="margin-bottom: 4px;"><span style="color: #666; font-size: 0.75rem;">[${timestamp}]</span> <span style="color: ${color}">${prefix}${text}</span></div>`;
            
            term.innerHTML += line;
            term.scrollTop = term.scrollHeight;
        }

        async function apiCall(endpoint, data = null) {
            try {
                const options = {
                    method: data ? 'POST' : 'GET',
                    headers: { 'Content-Type': 'application/json' },
                };
                if (data) options.body = JSON.stringify(data);
                
                const res = await fetch(endpoint, options);
                const json = await res.json();
                return json;
            } catch (e) {
                console.error(e);
                showToast("Connection Error");
                return null;
            }
        }

        async function refreshStatus() {
            const data = await apiCall('/api/status');
            if (data) {
                document.getElementById('signal-val').innerText = data.signal + '%';
                document.getElementById('operator').innerText = data.operator;
                document.getElementById('net-type').innerText = data.net_type;
                document.getElementById('sim-status').innerText = data.sim_status;
                document.getElementById('imei').innerText = data.imei;
                document.getElementById('model').innerText = data.model;
            }
        }

        async function sendSMS() {
            const num = document.getElementById('sms-number').value;
            const text = document.getElementById('sms-text').value;
            if (!num || !text) return showToast("Enter number and text");
            
            showToast("Sending SMS...");
            const res = await apiCall('/api/sms/send', { number: num, text: text });
            if (res && res.success) {
                showToast("SMS Sent Successfully");
                document.getElementById('sms-text').value = '';
            } else showToast("Failed to send SMS");
        }

        async function readSMS() {
            const idx = document.getElementById('sms-index').value;
            if (!idx) return showToast("Enter index");
            
            const res = await apiCall('/api/sms/read?index=' + idx);
            const content = document.getElementById('sms-content');
            if (res && res.success) {
                content.innerText = res.message;
                content.style.color = '#f1f5f9';
            } else {
                content.innerText = "Failed to read or empty slot";
                content.style.color = '#f87171';
            }
        }

        async function getGPS() {
            const latEl = document.getElementById('gps-lat');
            const lonEl = document.getElementById('gps-lon');
            latEl.innerText = "Locating...";
            lonEl.innerText = "Locating...";
            
            const res = await apiCall('/api/gps/location');
            if (res && res.success) {
                latEl.innerText = res.lat;
                lonEl.innerText = res.lon;
            } else {
                latEl.innerText = "Unavailable";
                lonEl.innerText = "Unavailable";
                showToast("GPS Fix Failed");
            }
        }

        async function dialNumber() {
            const num = document.getElementById('phone-number').value;
            if (!num) return showToast("Enter number");
            const res = await apiCall('/api/call/dial', { number: num });
            if (res && res.success) showToast("Calling...");
            else showToast("Call failed");
        }

        async function hangupCall() {
            const res = await apiCall('/api/call/hangup', {});
            if (res && res.success) showToast("Call Ended");
            else showToast("Hangup failed");
        }

        async function sendAT() {
            const cmdInput = document.getElementById('at-cmd');
            const cmd = cmdInput.value;
            if (!cmd) return;
            
            logToTerminal(cmd, 'tx');
            cmdInput.value = '';
            
            const res = await apiCall('/api/at', { cmd: cmd });
            if (res && res.response) {
                // Format response to preserve newlines
                const formatted = res.response.replace(/\n/g, '<br>');
                logToTerminal(formatted, 'rx');
            }
        }

        async function tcpOpen() {
            const host = document.getElementById('tcp-host').value;
            const port = document.getElementById('tcp-port').value;
            showToast("Connecting...");
            const res = await apiCall('/api/tcp/open', { host: host, port: parseInt(port) });
            if (res && res.success) showToast("Connected (ID: " + res.socketId + ")");
            else showToast("Connection Failed");
        }

        async function tcpClose() {
            const res = await apiCall('/api/tcp/close', {});
            if (res && res.success) showToast("Disconnected");
        }

        async function tcpSend() {
            const data = document.getElementById('tcp-data').value;
            const res = await apiCall('/api/tcp/send', { data: data });
            if (res && res.success) {
                showToast("Data Sent");
                if (res.response) {
                    document.getElementById('tcp-response').innerText = res.response;
                }
            } else {
                showToast("Send Failed");
            }
        }

        async function saveWifi() {
            const ssid = document.getElementById('wifi-ssid').value;
            const pass = document.getElementById('wifi-pass').value;
            if (!ssid) return showToast("Enter SSID");
            
            showToast("Saving...");
            const res = await apiCall('/api/wifi/save', { ssid: ssid, pass: pass });
            if (res && res.success) {
                showToast(res.message);
            } else {
                showToast("Save Failed");
            }
        }

        async function forgetWifi() {
            if (!confirm("Forget WiFi credentials?")) return;
            showToast("Forgetting...");
            const res = await apiCall('/api/wifi/forget', {});
            if (res && res.success) {
                showToast(res.message);
            }
        }

        // Initial load
        setInterval(refreshStatus, 5000);
        refreshStatus();
    </script>
</body>
</html>
)rawliteral";

#endif
