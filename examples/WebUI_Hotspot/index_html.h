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
            --card-bg: rgba(30, 41, 59, 0.6);
            --card-border: rgba(255, 255, 255, 0.08);
            --text-primary: #f1f5f9;
            --text-secondary: #94a3b8;
            --accent-color: #38bdf8;
            --accent-glow: rgba(56, 189, 248, 0.3);
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
                <div class="nav-tab" onclick="showSection('tcp', this)">Internet</div>
                <div class="nav-tab" onclick="showSection('mqtt', this)">MQTT</div>
                <div class="nav-tab" onclick="showSection('settings', this)">Settings</div>
                <div class="nav-tab" onclick="showSection('advanced', this)">Advanced</div>
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
                            <div class="status-value" id="signal-val">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Radio</div>
                            <div class="status-value" id="net-type">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Registration</div>
                            <div class="status-value" id="registration">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Operator</div>
                            <div class="status-value" id="operator">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">SIM</div>
                            <div class="status-value" id="sim-status">--</div>
                        </div>
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Network Info</div>
                            <div class="status-value" id="network-info" style="font-size: 0.85rem;">--</div>
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
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Firmware</div>
                            <div class="status-value" id="firmware" style="font-family: var(--font-mono); font-size: 0.85rem;">--</div>
                        </div>
                    </div>
                    <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 15px;">
                        <button onclick="refreshStatus()" class="secondary">
                            <span style="margin-right: 5px;">↻</span> Refresh
                        </button>
                        <div style="display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 5px;">
                            <button onclick="powerOnModem()" style="background: linear-gradient(135deg, #f59e0b, #d97706); padding: 8px; font-size: 0.85rem;">
                                <span style="margin-right: 2px;">⚡</span> On
                            </button>
                            <button onclick="powerOffModem()" class="danger" style="padding: 8px; font-size: 0.85rem;">
                                <span style="margin-right: 2px;">⭕</span> Off
                            </button>
                            <button onclick="rebootEsp()" class="secondary" style="padding: 8px; font-size: 0.85rem;">
                                <span style="margin-right: 2px;">🔄</span> ESP
                            </button>
                        </div>
                    </div>
                </div>

                <div class="card">
                    <div class="card-header">
                        <div class="card-title">Power & Sensors</div>
                        <button onclick="refreshDeviceSensors(true)" class="secondary" style="width: auto; padding: 8px 16px; font-size: 0.85rem;">Refresh</button>
                    </div>
                    <div class="status-grid">
                        <div class="status-item">
                            <div class="status-label">Battery</div>
                            <div class="status-value" id="battery-percent">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Voltage</div>
                            <div class="status-value" id="battery-voltage">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">ADC Value</div>
                            <div class="status-value" id="battery-adc">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">State</div>
                            <div class="status-value" id="battery-state">--</div>
                        </div>
                    </div>
                    <div style="margin-top: 12px; font-size: 0.8rem; color: var(--text-secondary);">
                        <div>Updated: <span id="battery-updated">--</span></div>
                        <div>Raw: <span id="battery-raw" style="font-family: var(--font-mono); font-size: 0.75rem;">--</span></div>
                    </div>
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
            <div class="card" style="max-width: 420px; margin: 0 auto;">
                <div class="card-header" style="justify-content: space-between; gap: 10px;">
                    <div class="card-title">Phone Dialer</div>
                    <button onclick="refreshCallStatus(true)" class="secondary" style="width: auto; padding: 6px 12px; font-size: 0.8rem;">Refresh</button>
                </div>
                <input type="tel" id="phone-number" placeholder="+1234567890" style="font-size: 1.4rem; text-align: center; letter-spacing: 1px; font-family: var(--font-mono);">
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 15px;">
                    <button onclick="dialNumber()" style="background: var(--success-color); box-shadow: 0 4px 12px rgba(74, 222, 128, 0.2);">Call</button>
                    <button onclick="hangupCall()" class="danger">End Call</button>
                </div>
                <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 10px; margin-bottom: 15px;">
                    <button onclick="answerCall()" class="secondary" style="background: rgba(34,197,94,0.15); border: 1px solid rgba(34,197,94,0.4); color: #4ade80;">Answer</button>
                    <button onclick="refreshCallStatus()" class="secondary">Sync Status</button>
                </div>
                <div class="status-grid" style="margin-bottom: 15px;">
                    <div class="status-item">
                        <div class="status-label">State</div>
                        <div class="status-value" id="call-state">Idle</div>
                    </div>
                    <div class="status-item">
                        <div class="status-label">Direction</div>
                        <div class="status-value" id="call-direction">--</div>
                    </div>
                    <div class="status-item" style="grid-column: span 2;">
                        <div class="status-label">Number</div>
                        <div class="status-value" id="call-number" style="font-family: var(--font-mono); font-size: 0.95rem;">--</div>
                    </div>
                </div>
                <div>
                    <div class="status-label">Active Calls</div>
                    <div id="call-log" class="terminal-output" style="height: 140px; color: var(--text-secondary);">No active calls</div>
                </div>
                <div class="status-label" style="margin-top: 15px;">Speaker Volume</div>
                <div style="display: flex; align-items: center; justify-content: center; gap: 12px; margin-bottom: 8px;">
                    <button onclick="adjustCallVolume(-5)" class="secondary" style="width: auto; padding: 8px 14px;">Vol -</button>
                    <div id="call-volume-display" style="min-width: 60px; text-align: center; font-weight: 600;">--</div>
                    <button onclick="adjustCallVolume(5)" class="secondary" style="width: auto; padding: 8px 14px;">Vol +</button>
                </div>
                <div id="call-message" style="text-align: center; font-size: 0.85rem; color: var(--text-secondary); min-height: 20px;"></div>
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

        <!-- Internet -->
        <div id="tcp" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">Internet Client</div>
                </div>
                <div class="grid" style="grid-template-columns: 2fr 1fr; gap: 10px; margin-bottom: 15px;">
                    <input type="text" id="tcp-host" placeholder="Host (e.g. google.com)" style="margin-bottom: 0;">
                    <input type="number" id="tcp-port" placeholder="Port" value="80" style="margin-bottom: 0;">
                </div>
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px;">
                    <button onclick="tcpOpen()">Connect</button>
                    <button onclick="tcpClose()" class="danger">Disconnect</button>
                </div>
                <button onclick="loadTcpDemo()" class="secondary" style="margin-bottom: 15px;">Load HTTP POST Demo</button>
                <textarea id="tcp-data" rows="3" placeholder="Data payload (e.g. GET / HTTP/1.1\r\nHost: google.com\r\n\r\n)" style="font-family: var(--font-mono); font-size: 0.85rem;"></textarea>
                <button onclick="tcpSend()" class="secondary">Send Data</button>
                <div style="margin-top: 20px;">
                    <div class="status-label">Server Response</div>
                    <div id="tcp-response" class="terminal-output" style="height: 150px;"></div>
                </div>

                <div style="margin-top: 30px; border-top: 1px solid rgba(255,255,255,0.08); padding-top: 20px;">
                    <h3 style="margin-bottom: 12px;">PPP / PDP Control</h3>
                    <div class="status-grid" style="grid-template-columns: repeat(2, minmax(140px, 1fr)); margin-bottom: 15px;">
                        <div class="status-item">
                            <div class="status-label">Context</div>
                            <div class="status-value" id="pdp-ctx">1</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">State</div>
                            <div class="status-value" id="pdp-state">Unknown</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">IP Address</div>
                            <div class="status-value" id="pdp-ip" style="font-size: 0.85rem;">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">APN Source</div>
                            <div class="status-value" id="pdp-source">--</div>
                        </div>
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Detected Operator</div>
                            <div class="status-value" id="pdp-operator">--</div>
                        </div>
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Selected APN</div>
                            <div class="status-value" id="pdp-detected-apn" style="font-size: 0.85rem;">--</div>
                        </div>
                    </div>
                    <div class="grid" style="grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 10px;">
                        <input type="text" id="pdp-context" placeholder="Context ID (default 1)" value="1">
                        <input type="text" id="pdp-apn" placeholder="Custom APN (optional)">
                        <input type="text" id="pdp-user" placeholder="Username (optional)">
                        <input type="password" id="pdp-pass" placeholder="Password (optional)">
                        <select id="pdp-auth">
                            <option value="0">Auth: None</option>
                            <option value="1">Auth: PAP</option>
                            <option value="2">Auth: CHAP</option>
                        </select>
                    </div>
                    <label style="display: flex; align-items: center; gap: 8px; margin: 10px 0; font-size: 0.85rem; color: var(--text-secondary);">
                        <input type="checkbox" id="pdp-save" style="width: auto;"> Save as custom APN
                    </label>
                    <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr)); gap: 10px;">
                        <button onclick="activatePdp()">Activate PDP / PPP</button>
                        <button onclick="deactivatePdp()" class="danger">Deactivate PDP</button>
                        <button onclick="refreshPdpStatus()" class="secondary">Refresh Status</button>
                        <button onclick="clearCustomApn()" class="secondary">Clear Custom APN</button>
                    </div>
                    <div id="pdp-message" style="margin-top: 10px; font-size: 0.85rem; color: var(--text-secondary);"></div>
                </div>
                </div>
            </div>

            <!-- MQTT -->
            <div id="mqtt" class="section">
                <div class="card">
                    <div class="card-header" style="gap: 10px;">
                        <div class="card-title">MQTT Client</div>
                        <button onclick="refreshMqttStatus()" class="secondary" style="width: auto; padding: 8px 16px; font-size: 0.85rem;">Refresh</button>
                    </div>
                    <div class="status-grid" style="margin-bottom: 20px; grid-template-columns: repeat(2, minmax(140px, 1fr));">
                        <div class="status-item">
                            <div class="status-label">Connection</div>
                            <div class="status-value" id="mqtt-connection">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Server</div>
                            <div class="status-value" id="mqtt-server">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Port</div>
                            <div class="status-value" id="mqtt-port">--</div>
                        </div>
                        <div class="status-item">
                            <div class="status-label">Context</div>
                            <div class="status-value" id="mqtt-ctx">--</div>
                        </div>
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Last Topic</div>
                            <div class="status-value" id="mqtt-last-topic" style="font-size: 0.85rem;">--</div>
                        </div>
                        <div class="status-item" style="grid-column: span 2;">
                            <div class="status-label">Last Error</div>
                            <div class="status-value" id="mqtt-last-error" style="font-size: 0.85rem; color: #f87171;">--</div>
                        </div>
                    </div>

                    <div class="grid" style="grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 12px; margin-bottom: 15px;">
                        <input type="text" id="mqtt-host" placeholder="Broker Host (e.g. test.mosquitto.org)">
                        <input type="number" id="mqtt-port-input" placeholder="Port" value="1883">
                        <input type="number" id="mqtt-context" placeholder="Context ID" value="1">
                        <input type="text" id="mqtt-apn" placeholder="Custom APN (optional)">
                        <input type="text" id="mqtt-user" placeholder="APN Username (optional)">
                        <input type="password" id="mqtt-pass" placeholder="APN Password (optional)">
                        <select id="mqtt-auth">
                            <option value="0">Auth: None</option>
                            <option value="1">Auth: PAP</option>
                            <option value="2">Auth: CHAP</option>
                        </select>
                    </div>

                    <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr)); gap: 10px; margin-bottom: 15px;">
                        <button onclick="connectMqtt()">Connect</button>
                        <button onclick="disconnectMqtt()" class="danger">Disconnect</button>
                        <button onclick="refreshMqttStatus()" class="secondary">Refresh Status</button>
                    </div>
                    <div id="mqtt-message" style="margin-bottom: 20px; font-size: 0.85rem; color: var(--text-secondary); min-height: 20px;"></div>

                    <div style="margin-bottom: 15px;">
                        <h3 style="margin-bottom: 10px;">Publish Message</h3>
                        <input type="text" id="mqtt-topic" placeholder="Topic (e.g. demo/topic)">
                        <textarea id="mqtt-payload" rows="3" placeholder='Payload JSON or text (e.g. {"hello":"world"})' style="font-family: var(--font-mono);"></textarea>
                        <button onclick="publishMqtt()" class="secondary" style="margin-top: 10px;">Publish</button>
                    </div>

                    <div style="margin-bottom: 15px;">
                        <h3 style="margin-bottom: 10px;">Subscribe</h3>
                        <div style="display: flex; gap: 10px;">
                            <input type="text" id="mqtt-sub-topic" placeholder="Topic filter" style="margin-bottom: 0;">
                            <button onclick="subscribeMqtt()" class="secondary" style="width: auto;">Subscribe</button>
                        </div>
                    </div>

                    <div>
                        <div class="status-label">Activity Log</div>
                        <div id="mqtt-log" class="terminal-output" style="height: 180px;"></div>
                    </div>
                </div>
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
                <div style="margin-top: 20px; border-top: 1px solid rgba(255,255,255,0.1); padding-top: 15px;">
                    <button onclick="rebootEsp()" class="secondary" style="width: 100%;">Reboot ESP32</button>
                </div>
            </div>
        </div>

        <!-- Advanced Features -->
        <div id="advanced" class="section">
            <div class="card">
                <div class="card-header">
                    <div class="card-title">Advanced Controls</div>
                </div>

                <div style="margin-bottom: 20px;">
                    <h3>SIM & Network</h3>
                    <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
                        <button onclick="apiCall('/api/sim/switch', {}, 'Switching SIM...')">Switch SIM</button>
                        <button onclick="apiCall('/api/sim/isim', {enable: '1'}, 'Enabling ISIM...')">Enable ISIM</button>
                        <button onclick="apiCall('/api/sim/dsds', {dsds: '1'}, 'Setting DSDS...')">Set DSDS</button>
                        <button onclick="apiCall('/api/call/block', {enable: '1'}, 'Blocking Calls...')">Block Calls</button>
                    </div>
                </div>

                <div style="margin-bottom: 20px;">
                    <h3>System</h3>
                    <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
                        <button onclick="apiCall('/api/system/usb', {}, 'Setting USB CDC...')">Set USB CDC</button>
                    </div>
                </div>

                <div style="margin-bottom: 20px;">
                    <h3>Network Ping (AT+QPING)</h3>
                    <input type="text" id="ping-host" placeholder="Host or IP (e.g. 8.8.8.8)">
                    <div style="display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px;">
                        <input type="number" id="ping-context" placeholder="Context ID" value="1">
                        <input type="number" id="ping-timeout" placeholder="Timeout (s)" value="4">
                        <input type="number" id="ping-count" placeholder="# Pings" value="4">
                    </div>
                    <button onclick="runPing()" class="secondary">Run Ping</button>
                    <div id="ping-output" style="margin-top: 10px; font-family: var(--font-mono); white-space: pre-wrap; background: rgba(0,0,0,0.2); padding: 10px; border-radius: 8px; min-height: 60px;">--</div>
                </div>

                <div style="margin-bottom: 20px;">
                    <h3>Quectel WiFi Scan</h3>
                    <button onclick="scanQuectelWifi()" class="secondary" style="width: 100%;">Scan Networks</button>
                    <div id="quectel-wifi-results" style="margin-top: 10px; font-family: var(--font-mono); white-space: pre-wrap; background: rgba(0,0,0,0.2); padding: 10px; border-radius: 8px; display: none;"></div>
                </div>
                <div style="margin-bottom: 20px;">
                    <h3>Bluetooth Scan</h3>
                    <button onclick="scanBluetooth()" class="secondary" style="width: 100%;">Scan Devices</button>
                    <div id="bt-results" style="margin-top: 10px; font-family: var(--font-mono); white-space: pre-wrap; background: rgba(0,0,0,0.2); padding: 10px; border-radius: 8px; display: none;"></div>
                </div>
            </div>
        </div>
    </div>

    <div id="toast">Notification</div>

    <script>
        let modemInfoCache = null;
        let lastStatusSnapshot = null;

        function setText(id, value) {
            const el = document.getElementById(id);
            if (el) {
                el.innerText = value;
            }
        }

        function escapeHtml(str = '') {
            return str.replace(/[&<>\"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;','\'':'&#39;'}[c] || c));
        }

        function batteryStatusText(code) {
            const map = {
                0: 'Unknown',
                1: 'Not Charging',
                2: 'Charging',
                3: 'Charged',
                4: 'Fault',
                5: 'Over Temp'
            };
            return map.hasOwnProperty(code) ? map[code] : 'Unknown';
        }

        function showSection(id, tab) {
            document.querySelectorAll('.section').forEach(el => el.classList.remove('active'));
            document.getElementById(id).classList.add('active');
            document.querySelectorAll('.nav-tab').forEach(el => el.classList.remove('active'));
            tab.classList.add('active');
            if (id === 'advanced') {
                refreshModemInfo(true);
            }
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

        function applyModemInfo(info) {
            if (!info) return;
            modemInfoCache = info;
            const firmware = info.firmware || '--';
            const model = info.model || '--';
            const manufacturer = info.manufacturer || '--';
            const imei = info.imei || '--';
            const banner = (info.version || '').replace(/\r/g, '\n');

            document.getElementById('firmware').innerText = firmware;
            document.getElementById('model').innerText = model;
            document.getElementById('imei').innerText = imei;

        }

        async function refreshModemInfo(force = false) {
            if (!force && modemInfoCache) {
                applyModemInfo(modemInfoCache);
                return;
            }
            const info = await apiCall('/api/modem/info');
            if (info) {
                applyModemInfo(info);
            }
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
                lastStatusSnapshot = data;
                document.getElementById('signal-val').innerText = `${data.signal}% (${data.signal_text})`;
                document.getElementById('operator').innerText = data.operator;
                document.getElementById('net-type').innerText = data.net_type || '--';
                document.getElementById('registration').innerText = data.registration || '--';
                document.getElementById('sim-status').innerText = data.sim_status;
                document.getElementById('network-info').innerText = data.network_info || '--';
                if (data.imei) document.getElementById('imei').innerText = data.imei;
                if (data.model) document.getElementById('model').innerText = data.model;
                if (data.mqtt) applyMqttStatus({
                    connected: data.mqtt.connected,
                    server: data.mqtt.server,
                    port: data.mqtt.port,
                    last_topic: data.mqtt.last_topic
                });
            }
        }

        function applySensorData(data) {
            const battery = data && data.battery ? data.battery : {};
            const sensors = data && data.sensors ? data.sensors : {};
            if (battery.valid && typeof battery.percent === 'number') {
                setText('battery-percent', `${battery.percent}%`);
            } else {
                setText('battery-percent', '--');
            }
            if (typeof battery.voltage === 'number' && !isNaN(battery.voltage)) {
                setText('battery-voltage', `${battery.voltage.toFixed(2)} V`);
            } else {
                setText('battery-voltage', '--');
            }
            setText('battery-state', batteryStatusText(battery.status));
            setText('battery-raw', battery.raw || '--');
            setText('battery-adc', sensors.adc_value != null ? sensors.adc_value : '--');
            setText('battery-updated', new Date().toLocaleTimeString());
        }

        async function refreshDeviceSensors(showToastMsg = false) {
            const res = await apiCall('/api/device/sensors');
            if (res) {
                applySensorData(res);
                if (showToastMsg) showToast('Battery updated');
            } else if (showToastMsg) {
                showToast('Unable to read sensors');
            }
        }

        function applyCallStatus(data) {
            const entries = (data && Array.isArray(data.entries)) ? data.entries : [];
            const activeEntry = entries.length ? entries[0] : null;
            setText('call-state', activeEntry ? (activeEntry.state || 'Active') : 'Idle');
            setText('call-direction', activeEntry ? (activeEntry.direction || '--') : '--');
            setText('call-number', activeEntry ? (activeEntry.number || '--') : '--');
            if (data && typeof data.speaker_volume === 'number') {
                setText('call-volume-display', `${data.speaker_volume}%`);
            }

            const logEl = document.getElementById('call-log');
            if (logEl) {
                if (entries.length === 0) {
                    logEl.innerHTML = 'No active calls';
                } else {
                    logEl.innerHTML = entries.map((entry, idx) => {
                        const number = entry.number && entry.number.length ? entry.number : 'Unknown';
                        return `<div><strong>${idx + 1}. ${entry.state || 'Active'}</strong> · ${entry.direction || '--'}<br><span style="color:#94a3b8;">${escapeHtml(number)}</span></div>`;
                    }).join('<hr style="border:none;border-top:1px solid rgba(255,255,255,0.08); margin:8px 0;">');
                }
            }
        }

        async function refreshCallStatus(showToastMsg = false) {
            const res = await apiCall('/api/call/status');
            if (res && res.success) {
                applyCallStatus(res);
                if (showToastMsg) showToast('Call status synced');
            } else if (showToastMsg) {
                showToast('Unable to sync call status');
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
            const callMsg = document.getElementById('call-message');
            if (callMsg) callMsg.innerText = 'Dialing...';
            const res = await apiCall('/api/call/dial', { number: num });
            if (res && res.success) {
                showToast("Calling...");
                if (callMsg) callMsg.innerText = 'Calling in progress';
            } else {
                showToast("Call failed");
                if (callMsg) callMsg.innerText = 'Call attempt failed';
            }
            refreshCallStatus();
        }

        async function hangupCall() {
            const res = await apiCall('/api/call/hangup', {});
            const callMsg = document.getElementById('call-message');
            if (res && res.success) {
                showToast("Call Ended");
                if (callMsg) callMsg.innerText = 'Call ended';
            } else {
                showToast("Hangup failed");
                if (callMsg) callMsg.innerText = 'Unable to end call';
            }
            refreshCallStatus();
        }

        async function answerCall() {
            const callMsg = document.getElementById('call-message');
            if (callMsg) callMsg.innerText = 'Answering incoming call...';
            const res = await apiCall('/api/call/answer', {});
            if (res && res.success) {
                showToast('Call answered');
                if (callMsg) callMsg.innerText = 'Call answered';
            } else {
                showToast('Answer failed');
                if (callMsg) callMsg.innerText = 'Unable to answer';
            }
            refreshCallStatus();
        }

        async function adjustCallVolume(delta) {
            const callMsg = document.getElementById('call-message');
            if (callMsg) callMsg.innerText = 'Adjusting volume...';
            const res = await apiCall('/api/call/volume', { delta });
            if (res && res.success) {
                setText('call-volume-display', `${res.level}%`);
                showToast(`Volume ${res.level}%`);
                if (callMsg) callMsg.innerText = `Speaker volume ${res.level}%`;
            } else {
                const err = (res && res.error) ? res.error : 'Volume change failed';
                showToast(err);
                if (callMsg) callMsg.innerText = err;
            }
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
            const payload = { host: host, port: parseInt(port) };
            const res = await apiCall('/api/tcp/open', payload);
            if (res && res.success) {
                showToast("Connected (ID: " + res.socketId + ")");
                document.getElementById('tcp-response').innerText = '';
            } else {
                showToast(res && res.error ? res.error : "Connection Failed");
            }
        }

        async function tcpClose() {
            const res = await apiCall('/api/tcp/close', {});
            if (res && res.success) {
                showToast("Disconnected");
            } else {
                showToast(res && res.error ? res.error : "No open socket");
            }
        }

        async function tcpSend() {
            const data = document.getElementById('tcp-data').value;
            const res = await apiCall('/api/tcp/send', { data: data });
            if (res && res.success) {
                showToast("Data Sent");
                const responseBox = document.getElementById('tcp-response');
                responseBox.innerText = res.response ? res.response : (res.raw || 'No response.');
            } else {
                showToast(res && res.error ? res.error : "Send Failed");
            }
        }

        function loadTcpDemo() {
            document.getElementById('tcp-host').value = 'postman-echo.com';
            document.getElementById('tcp-port').value = 80;
            document.getElementById('tcp-data').value = 'POST /post HTTP/1.1\r\nHost: postman-echo.com\r\nContent-Type: application/json\r\nContent-Length: 27\r\n\r\n{"hello":"ec200u webui"}';
            showToast('Demo request loaded');
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

        async function powerOnModem() {
            if (!confirm("Turn on modem? This takes about 3 seconds.")) return;
            showToast("Powering On...");
            const res = await apiCall('/api/modem/poweron', {});
            if (res && res.success) {
                showToast(res.message);
                setTimeout(refreshStatus, 3000);
            } else {
                showToast("Failed to Power On");
            }
        }

        async function powerOffModem() {
            if (!confirm("Turn off modem?")) return;
            showToast("Powering Off...");
            const res = await apiCall('/api/modem/poweroff', {});
            if (res && res.success) showToast(res.message);
        }

        async function rebootEsp() {
            if (!confirm("Reboot ESP32?")) return;
            showToast("Rebooting...");
            await apiCall('/api/esp/reboot', {});
        }

        async function scanQuectelWifi() {
            const panel = document.getElementById('quectel-wifi-results');
            panel.style.display = 'block';
            panel.innerText = "Scanning...";
            const res = await apiCall('/api/quectel/wifi/scan', {});
            if (res && res.success) {
                if (Array.isArray(res.entries) && res.entries.length) {
                    panel.innerHTML = res.entries.map((net, idx) => {
                        const ssid = net.ssid || '(Hidden SSID)';
                        const auth = net.auth ? ` (${net.auth})` : '';
                        const rssi = net.rssi !== undefined ? `${net.rssi} dBm` : '--';
                        const channel = net.channel !== undefined ? net.channel : '--';
                        const bssid = net.bssid || '';
                        return `<div><strong>[${idx + 1}] ${ssid}</strong>${auth}<br>RSSI: ${rssi} | CH: ${channel}<br><span style="color:#94a3b8;">${bssid}</span></div>`;
                    }).join('<hr style="border:none;border-top:1px solid rgba(255,255,255,0.08); margin:8px 0;">');
                } else {
                    panel.innerText = res.raw || "No networks found.";
                }
            } else {
                panel.innerText = (res && res.raw) ? res.raw : "Scan failed.";
            }
        }

        async function scanBluetooth() {
            const panel = document.getElementById('bt-results');
            panel.style.display = 'block';
            panel.innerText = "Scanning...";
            const res = await apiCall('/api/quectel/bt/scan', {});
            if (res && res.success) {
                if (Array.isArray(res.entries) && res.entries.length) {
                    panel.innerHTML = res.entries.map((dev, idx) => {
                        const name = dev.name || 'Unknown Device';
                        const mac = dev.mac || '??';
                        return `<div><strong>[${idx + 1}] ${name}</strong><br><span style="color:#94a3b8;">${mac}</span></div>`;
                    }).join('<hr style="border:none;border-top:1px solid rgba(255,255,255,0.08); margin:8px 0;">');
                } else {
                    panel.innerText = res.raw || "No devices found.";
                }
            } else {
                panel.innerText = (res && res.raw) ? res.raw : "Scan failed.";
            }
        }

        async function runPing() {
            const host = document.getElementById('ping-host').value.trim();
            const ctx = parseInt(document.getElementById('ping-context').value, 10) || 1;
            const timeout = parseInt(document.getElementById('ping-timeout').value, 10) || 4;
            const count = parseInt(document.getElementById('ping-count').value, 10) || 4;
            const panel = document.getElementById('ping-output');

            if (!host) {
                return showToast('Enter a host to ping');
            }

            panel.innerText = 'Running ping...';
            const res = await apiCall('/api/ping', { host, context: ctx, timeout, count });
            if (res) {
                const text = (res.report || '').replace(/\r/g, '\n');
                panel.innerText = text.length ? text : 'No response data received.';
                showToast(res.success ? 'Ping complete' : (res.error || 'Ping failed'));
            } else {
                panel.innerText = 'Ping request failed.';
            }
        }

        function applyPdpStatus(data) {
            if (!data) return;
            setText('pdp-ctx', data.ctxId ?? '--');
            const stateEl = document.getElementById('pdp-state');
            if (stateEl) {
                const active = !!data.active;
                stateEl.innerText = active ? 'Active' : 'Inactive';
                stateEl.style.color = active ? 'var(--success-color)' : 'var(--text-secondary)';
            }
            setText('pdp-ip', (data.ip && data.ip !== '0.0.0.0') ? data.ip : '--');
            const selection = data.selection || {};
            const stored = data.stored || {};
            const sourceLabel = selection.source ? selection.source.toUpperCase() : '--';
            setText('pdp-source', sourceLabel);
            setText('pdp-operator', selection.operator || '--');
            const detectedApn = (selection.source === 'auto' && selection.apn) ? selection.apn : (selection.detected ? (selection.keyword || selection.apn || '--') : '--');
            setText('pdp-detected-apn', detectedApn || '--');

            const ctxInput = document.getElementById('pdp-context');
            if (ctxInput) ctxInput.value = data.ctxId || 1;
            const apnInput = document.getElementById('pdp-apn');
            if (apnInput) apnInput.value = selection.apn || stored.apn || '';
            const userInput = document.getElementById('pdp-user');
            if (userInput) userInput.value = selection.user || stored.user || '';
            const passInput = document.getElementById('pdp-pass');
            if (passInput) passInput.value = selection.pass || stored.pass || '';
            const authInput = document.getElementById('pdp-auth');
            if (authInput) authInput.value = selection.auth != null ? selection.auth : (stored.auth || 0);
            const saveCheckbox = document.getElementById('pdp-save');
            if (saveCheckbox) saveCheckbox.checked = !!stored.hasCustom;
        }

        async function refreshPdpStatus(showToastMsg = false) {
            const ctxVal = parseInt(document.getElementById('pdp-context').value, 10) || 1;
            const res = await apiCall(`/api/pdp/status?ctx=${ctxVal}`);
            if (res) {
                applyPdpStatus(res);
                if (showToastMsg) showToast('PDP status updated');
            } else if (showToastMsg) {
                showToast('Unable to refresh PDP status');
            }
        }

        async function activatePdp() {
            const ctxId = parseInt(document.getElementById('pdp-context').value, 10) || 1;
            const apn = document.getElementById('pdp-apn').value.trim();
            const user = document.getElementById('pdp-user').value.trim();
            const pass = document.getElementById('pdp-pass').value;
            const auth = parseInt(document.getElementById('pdp-auth').value, 10) || 0;
            const persist = document.getElementById('pdp-save').checked;
            const messageEl = document.getElementById('pdp-message');
            messageEl.innerText = 'Configuring context...';

            const payload = { ctxId, persist };
            if (apn) payload.apn = apn;
            if (user) payload.user = user;
            if (pass) payload.pass = pass;
            if (apn || user || pass || auth !== 0) payload.auth = auth;
            if (lastStatusSnapshot && lastStatusSnapshot.operator) {
                payload.operator = lastStatusSnapshot.operator;
            }

            const res = await apiCall('/api/pdp/activate', payload);
            if (res) {
                if (res.success) {
                    showToast('PDP activated');
                    messageEl.innerText = `Context ${ctxId} active.`;
                } else {
                    const err = res.error || 'Activation failed';
                    messageEl.innerText = err;
                    showToast(err);
                }
            } else {
                messageEl.innerText = 'Activation request failed';
                showToast('PDP activation failed');
            }
            refreshPdpStatus();
        }

        async function deactivatePdp() {
            const ctxId = parseInt(document.getElementById('pdp-context').value, 10) || 1;
            const messageEl = document.getElementById('pdp-message');
            messageEl.innerText = 'Deactivating context...';
            const res = await apiCall('/api/pdp/deactivate', { ctxId });
            if (res && res.success) {
                showToast('PDP deactivated');
                messageEl.innerText = `Context ${ctxId} deactivated.`;
            } else {
                const err = (res && res.error) ? res.error : 'Deactivate failed';
                messageEl.innerText = err;
                showToast(err);
            }
            refreshPdpStatus();
        }

        async function clearCustomApn() {
            if (!confirm('Clear saved APN credentials?')) return;
            const res = await apiCall('/api/pdp/clear', {});
            if (res && res.success) {
                showToast('Custom APN cleared');
                document.getElementById('pdp-save').checked = false;
                refreshPdpStatus();
            } else {
                showToast('Failed to clear APN');
            }
        }

        function appendMqttLog(message, level = 'info') {
            const logEl = document.getElementById('mqtt-log');
            if (!logEl) return;
            const ts = new Date().toLocaleTimeString();
            const color = level === 'error' ? '#f87171' : '#38bdf8';
            logEl.innerHTML += `<div><span style="color:#64748b;">[${ts}]</span> <span style="color:${color}">${escapeHtml(message)}</span></div>`;
            logEl.scrollTop = logEl.scrollHeight;
        }

        function applyMqttStatus(status = {}) {
            const connected = !!status.connected;
            const connectionEl = document.getElementById('mqtt-connection');
            if (connectionEl) {
                connectionEl.innerText = connected ? 'Connected' : 'Disconnected';
                connectionEl.style.color = connected ? 'var(--success-color)' : 'var(--text-secondary)';
            }
            setText('mqtt-server', status.server || '--');
            setText('mqtt-port', status.port ? String(status.port) : '--');
            setText('mqtt-ctx', status.ctxId != null ? String(status.ctxId) : '--');
            setText('mqtt-last-topic', status.last_topic || '--');
            setText('mqtt-last-error', status.last_error || '--');

            if (status.server) document.getElementById('mqtt-host').value = status.server;
            if (status.port) document.getElementById('mqtt-port-input').value = status.port;
            if (status.ctxId) document.getElementById('mqtt-context').value = status.ctxId;
        }

        async function refreshMqttStatus(showToastMsg = false) {
            const res = await apiCall('/api/mqtt/status');
            if (res) {
                applyMqttStatus(res);
                if (showToastMsg) showToast('MQTT status updated');
            } else if (showToastMsg) {
                showToast('Unable to refresh MQTT status');
            }
        }

        async function connectMqtt() {
            const host = document.getElementById('mqtt-host').value.trim();
            if (!host) return showToast('Enter broker host');
            const port = parseInt(document.getElementById('mqtt-port-input').value, 10) || 1883;
            const ctxId = parseInt(document.getElementById('mqtt-context').value, 10) || 1;
            const apn = document.getElementById('mqtt-apn').value.trim();
            const user = document.getElementById('mqtt-user').value.trim();
            const pass = document.getElementById('mqtt-pass').value;
            const authVal = parseInt(document.getElementById('mqtt-auth').value, 10) || 0;
            const msgEl = document.getElementById('mqtt-message');
            msgEl.innerText = 'Connecting to broker...';

            const payload = { host, port, ctxId };
            if (lastStatusSnapshot && lastStatusSnapshot.operator) {
                payload.operator = lastStatusSnapshot.operator;
            }
            if (apn) payload.apn = apn;
            if (user) payload.user = user;
            if (pass) payload.pass = pass;
            if (apn || user || pass || authVal !== 0) payload.auth = authVal;

            const res = await apiCall('/api/mqtt/connect', payload);
            if (res && res.success) {
                msgEl.innerText = `Connected to ${host}:${port}`;
                showToast('MQTT connected');
                appendMqttLog(`Connected to ${host}:${port}`);
            } else {
                const err = (res && res.error) ? res.error : 'MQTT connect failed';
                msgEl.innerText = err;
                showToast(err);
                appendMqttLog(`Connect failed: ${err}`, 'error');
            }
            refreshMqttStatus();
        }

        async function disconnectMqtt() {
            const msgEl = document.getElementById('mqtt-message');
            msgEl.innerText = 'Disconnecting...';
            const res = await apiCall('/api/mqtt/disconnect', {});
            if (res && res.success) {
                showToast('MQTT disconnected');
                appendMqttLog('Disconnected');
                msgEl.innerText = 'Disconnected from broker.';
            } else {
                const err = (res && res.error) ? res.error : 'Disconnect failed';
                msgEl.innerText = err;
                showToast(err);
                appendMqttLog(`Disconnect failed: ${err}`, 'error');
            }
            refreshMqttStatus();
        }

        async function publishMqtt() {
            const topic = document.getElementById('mqtt-topic').value.trim();
            const payload = document.getElementById('mqtt-payload').value;
            if (!topic) return showToast('Enter publish topic');
            const res = await apiCall('/api/mqtt/publish', { topic, payload });
            if (res && res.success) {
                showToast('Message published');
                appendMqttLog(`Published ${topic}`);
            } else {
                const err = (res && res.error) ? res.error : 'Publish failed';
                showToast(err);
                appendMqttLog(`Publish failed: ${err}`, 'error');
            }
            refreshMqttStatus();
        }

        async function subscribeMqtt() {
            const topic = document.getElementById('mqtt-sub-topic').value.trim();
            if (!topic) return showToast('Enter subscription topic');
            const res = await apiCall('/api/mqtt/subscribe', { topic });
            if (res && res.success) {
                showToast('Subscription sent');
                appendMqttLog(`Subscribed ${topic}`);
            } else {
                const err = (res && res.error) ? res.error : 'Subscribe failed';
                showToast(err);
                appendMqttLog(`Subscribe failed: ${err}`, 'error');
            }
            refreshMqttStatus();
        }

        // Initial load
        setInterval(refreshStatus, 5000);
        setInterval(refreshCallStatus, 4000);
        setInterval(() => refreshDeviceSensors(), 15000);
        refreshStatus();
        refreshCallStatus();
        refreshDeviceSensors();
        refreshModemInfo();
        refreshPdpStatus();
        refreshMqttStatus();
    </script>
</body>
</html>
)rawliteral";

#endif
