#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Quectel Manager</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-color: #0f172a;
            --card-bg: rgba(30, 41, 59, 0.7);
            --text-color: #e2e8f0;
            --accent-color: #38bdf8;
            --success-color: #4ade80;
            --error-color: #f87171;
            --border-color: rgba(148, 163, 184, 0.1);
            --glass-border: 1px solid rgba(255, 255, 255, 0.1);
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Inter', sans-serif;
        }

        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            min-height: 100vh;
            background-image: 
                radial-gradient(at 0% 0%, rgba(56, 189, 248, 0.15) 0px, transparent 50%),
                radial-gradient(at 100% 100%, rgba(139, 92, 246, 0.15) 0px, transparent 50%);
            padding: 20px;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding-bottom: 80px; /* Space for bottom nav on mobile */
        }

        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 30px;
            padding: 20px;
            background: var(--card-bg);
            backdrop-filter: blur(12px);
            border-radius: 16px;
            border: var(--glass-border);
            box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
        }

        h1 {
            font-size: 1.5rem;
            font-weight: 600;
            background: linear-gradient(to right, #38bdf8, #818cf8);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
        }

        .card {
            background: var(--card-bg);
            backdrop-filter: blur(12px);
            border-radius: 16px;
            padding: 24px;
            border: var(--glass-border);
            transition: transform 0.2s, box-shadow 0.2s;
        }

        .card:hover {
            transform: translateY(-2px);
            box-shadow: 0 10px 15px -3px rgba(0, 0, 0, 0.1);
        }

        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 20px;
        }

        .card-title {
            font-size: 1.1rem;
            font-weight: 500;
            color: var(--accent-color);
        }

        .status-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
        }

        .status-item {
            background: rgba(255, 255, 255, 0.03);
            padding: 15px;
            border-radius: 12px;
            text-align: center;
        }

        .status-label {
            font-size: 0.8rem;
            color: #94a3b8;
            margin-bottom: 5px;
        }

        .status-value {
            font-size: 1.1rem;
            font-weight: 600;
        }

        input, select, textarea {
            width: 100%;
            background: rgba(15, 23, 42, 0.6);
            border: 1px solid rgba(148, 163, 184, 0.2);
            padding: 12px;
            border-radius: 8px;
            color: white;
            margin-bottom: 15px;
            font-size: 0.95rem;
            transition: border-color 0.2s;
        }

        input:focus, select:focus, textarea:focus {
            outline: none;
            border-color: var(--accent-color);
        }

        button {
            width: 100%;
            padding: 12px;
            border-radius: 8px;
            border: none;
            background: linear-gradient(135deg, #38bdf8, #3b82f6);
            color: white;
            font-weight: 600;
            cursor: pointer;
            transition: opacity 0.2s;
        }

        button:hover {
            opacity: 0.9;
        }

        button.secondary {
            background: rgba(255, 255, 255, 0.1);
        }
        
        button.danger {
            background: linear-gradient(135deg, #f87171, #ef4444);
        }

        .terminal-output {
            background: #000;
            color: #4ade80;
            font-family: 'Courier New', monospace;
            padding: 15px;
            border-radius: 8px;
            height: 200px;
            overflow-y: auto;
            margin-bottom: 15px;
            font-size: 0.9rem;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }

        .nav-tabs {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
            overflow-x: auto;
            padding-bottom: 5px;
        }

        .nav-tab {
            padding: 8px 16px;
            background: rgba(255, 255, 255, 0.05);
            border-radius: 20px;
            cursor: pointer;
            white-space: nowrap;
            font-size: 0.9rem;
            transition: background 0.2s;
        }

        .nav-tab.active {
            background: var(--accent-color);
            color: #0f172a;
            font-weight: 600;
        }

        .section {
            display: none;
            animation: fadeIn 0.3s ease;
        }

        .section.active {
            display: block;
        }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        /* Toast Notification */
        #toast {
            visibility: hidden;
            min-width: 250px;
            background-color: #333;
            color: #fff;
            text-align: center;
            border-radius: 8px;
            padding: 16px;
            position: fixed;
            z-index: 1;
            left: 50%;
            bottom: 30px;
            transform: translateX(-50%);
            font-size: 17px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.3);
        }

        #toast.show {
            visibility: visible;
            animation: fadein 0.5s, fadeout 0.5s 2.5s;
        }

        @keyframes fadein {
            from {bottom: 0; opacity: 0;}
            to {bottom: 30px; opacity: 1;}
        }

        @keyframes fadeout {
            from {bottom: 30px; opacity: 1;}
            to {bottom: 0; opacity: 0;}
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Quectel Manager</h1>
            <div id="connection-status" style="color: var(--success-color); font-size: 0.9rem;">● Connected</div>
        </header>

        <div class="nav-tabs">
            <div class="nav-tab active" onclick="showSection('dashboard')">Dashboard</div>
            <div class="nav-tab" onclick="showSection('sms')">SMS</div>
            <div class="nav-tab" onclick="showSection('gps')">GPS</div>
            <div class="nav-tab" onclick="showSection('phone')">Phone</div>
            <div class="nav-tab" onclick="showSection('terminal')">Terminal</div>
            <div class="nav-tab" onclick="showSection('tcp')">TCP Test</div>
        </div>

        <!-- Dashboard Section -->
        <div id="dashboard" class="section active">
            <div class="grid">
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Network Status</div>
                    </div>
                    <div class="status-grid">
                        <div class="status-item">
                            <div class="status-label">Signal Strength</div>
                            <div class="status-value" id="signal-val">--%</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Network Type</div>
                            <div class="status-value" id="net-type">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Operator</div>
                            <div class="status-value" id="operator">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">SIM Status</div>
                            <div class="status-value" id="sim-status">--</div>
                        </div>
                    </div>
                </div>
                
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Device Info</div>
                    </div>
                    <div class="status-grid">
                        <div class="status-item">
                            <div class="status-label">IMEI</div>
                            <div class="status-value" id="imei" style="font-size: 0.9rem;">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Model</div>
                            <div class="status-value" id="model">--</div>
                        </div>
                    </div>
                    <button onclick="refreshStatus()" class="secondary" style="margin-top: 15px;">Refresh Status</button>
                </div>
            </div>
        </div>

        <!-- SMS Section -->
        <div id="sms" class="section">
            <div class="grid">
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Send SMS</div>
                    </div>
                    <input type="text" id="sms-number" placeholder="Phone Number">
                    <textarea id="sms-text" rows="4" placeholder="Message"></textarea>
                    <button onclick="sendSMS()">Send Message</button>
                </div>
                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Read SMS</div>
                    </div>
                    <div style="display: flex; gap: 10px; margin-bottom: 15px;">
                        <input type="number" id="sms-index" placeholder="Index" style="margin-bottom: 0;">
                        <button onclick="readSMS()" class="secondary" style="width: auto;">Read</button>
                    </div>
                    <div id="sms-content" class="terminal-output" style="height: 100px;">No message loaded</div>
                </div>
            </div>
        </div>

        <!-- GPS Section -->
        <div id="gps" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">GPS Location</div>
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
                <button onclick="getGPS()">Get Location</button>
                <div style="margin-top: 15px; font-size: 0.8rem; color: #94a3b8; text-align: center;">
                    Make sure GNSS is enabled and antenna is connected.
                </div>
            </div>
        </div>

        <!-- Phone Section -->
        <div id="phone" class="section">
            <div class="card" style="max-width: 400px; margin: 0 auto;">
                <div class="card-header">
                    <div class="card-title">Phone</div>
                </div>
                <input type="tel" id="phone-number" placeholder="Enter Number" style="font-size: 1.5rem; text-align: center; letter-spacing: 2px;">
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 15px;">
                    <button onclick="dialNumber()" style="background: var(--success-color);">Call</button>
                    <button onclick="hangupCall()" class="danger">Hangup</button>
                </div>
            </div>
        </div>

        <!-- Terminal Section -->
        <div id="terminal" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">AT Command Terminal</div>
                </div>
                <div id="term-output" class="terminal-output"></div>
                <div style="display: flex; gap: 10px;">
                    <input type="text" id="at-cmd" placeholder="Enter AT Command (e.g. ATI)" style="margin-bottom: 0;">
                    <button onclick="sendAT()" style="width: auto;">Send</button>
                </div>
            </div>
        </div>

        <!-- TCP Section -->
        <div id="tcp" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">TCP Test</div>
                </div>
                <div class="grid" style="grid-template-columns: 2fr 1fr; gap: 10px; margin-bottom: 15px;">
                    <input type="text" id="tcp-host" placeholder="Host (e.g. google.com)" style="margin-bottom: 0;">
                    <input type="number" id="tcp-port" placeholder="Port" value="80" style="margin-bottom: 0;">
                </div>
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px;">
                    <button onclick="tcpOpen()">Open Connection</button>
                    <button onclick="tcpClose()" class="danger">Close</button>
                </div>
                <textarea id="tcp-data" rows="3" placeholder="Data to send (e.g. GET / HTTP/1.1\r\nHost: google.com\r\n\r\n)"></textarea>
                <button onclick="tcpSend()" class="secondary">Send Data</button>
                <div style="margin-top: 15px;">
                    <div class="status-label">Response:</div>
                    <div id="tcp-response" class="terminal-output" style="height: 150px;"></div>
                </div>
            </div>
        </div>
    </div>

    <div id="toast">Notification</div>

    <script>
        function showSection(id) {
            document.querySelectorAll('.section').forEach(el => el.classList.remove('active'));
            document.getElementById(id).classList.add('active');
            document.querySelectorAll('.nav-tab').forEach(el => el.classList.remove('active'));
            event.target.classList.add('active');
        }

        function showToast(msg) {
            const x = document.getElementById("toast");
            x.innerText = msg;
            x.className = "show";
            setTimeout(function(){ x.className = x.className.replace("show", ""); }, 3000);
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
                showToast("Error: " + e.message);
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
            
            const res = await apiCall('/api/sms/send', { number: num, text: text });
            if (res && res.success) showToast("SMS Sent!");
            else showToast("Failed to send SMS");
        }

        async function readSMS() {
            const idx = document.getElementById('sms-index').value;
            if (!idx) return showToast("Enter index");
            
            const res = await apiCall('/api/sms/read?index=' + idx);
            if (res && res.success) {
                document.getElementById('sms-content').innerText = res.message;
            } else {
                document.getElementById('sms-content').innerText = "Failed to read or empty";
            }
        }

        async function getGPS() {
            document.getElementById('gps-lat').innerText = "Loading...";
            document.getElementById('gps-lon').innerText = "Loading...";
            const res = await apiCall('/api/gps/location');
            if (res && res.success) {
                document.getElementById('gps-lat').innerText = res.lat;
                document.getElementById('gps-lon').innerText = res.lon;
            } else {
                document.getElementById('gps-lat').innerText = "Error";
                document.getElementById('gps-lon').innerText = "Error";
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
            if (res && res.success) showToast("Hung up");
            else showToast("Hangup failed");
        }

        async function sendAT() {
            const cmd = document.getElementById('at-cmd').value;
            if (!cmd) return;
            
            const term = document.getElementById('term-output');
            term.innerHTML += `> ${cmd}<br>`;
            
            const res = await apiCall('/api/at', { cmd: cmd });
            if (res) {
                term.innerHTML += `${res.response.replace(/\n/g, '<br>')}<br>`;
                term.scrollTop = term.scrollHeight;
            }
        }

        async function tcpOpen() {
            const host = document.getElementById('tcp-host').value;
            const port = document.getElementById('tcp-port').value;
            const res = await apiCall('/api/tcp/open', { host: host, port: parseInt(port) });
            if (res && res.success) showToast("TCP Opened ID: " + res.socketId);
            else showToast("TCP Open Failed");
        }

        async function tcpClose() {
            const res = await apiCall('/api/tcp/close', {});
            if (res && res.success) showToast("TCP Closed");
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

        // Initial load
        setInterval(refreshStatus, 5000);
        refreshStatus();
    </script>
</body>
</html>
)rawliteral";

#endif
