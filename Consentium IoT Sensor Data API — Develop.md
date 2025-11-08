Consentium IoT Sensor Data API — Developer Documentation v2
The Consentium IoT API enables IoT devices to send telemetry (sensor readings, firmware/device status) to the cloud and allows applications to retrieve historical and real-time data for monitoring, analytics, or automation.

Base URL

Copy

https://api.consentiumiot.com
Authentication & Access Control
Every request requires authentication keys provided at board registration:

sendKey → Used for write operations (sending or updating sensor data).

receiveKey → Used for read operations (retrieving sensor data).

boardKey → Unique identifier for a virtual board.

⚠️ Important:

Keep your keys secret. Treat them like passwords.

Use HTTPS at all times to avoid exposing credentials.

Quickstart Guide
1. Submit Data (POST)
cURL Example


Copy
curl -X POST "https://api.consentiumiot.com/v2/updateData?sendKey=<YOUR-SEND-KEY>&boardKey=<YOUR-BOARD-KEY>" \
  -H "Content-Type: application/json" \
  -d '{
    "sensors": {
      "sensorData": [
        { "info": "Temperature", "data": "40.00" },
        { "info": "Humidity", "data": "90.00" }
      ]
    },
    "boardInfo": {
      "firmwareVersion": "0.0",
      "architecture": "ESP32",
      "statusOTA": false,
      "deviceMAC": "DD:DA:0C:20:E1:41",
      "signalStrength": -30
    }
  }'
Postman Example

Method: POST

URL:


Copy
https://api.consentiumiot.com/v2/updateData?sendKey=<YOUR-SEND-KEY>&boardKey=<YOUR-BOARD-KEY>
Headers: Content-Type: application/json

Body (raw JSON):


Copy
{
  "sensors": {
    "sensorData": [
      { "info": "Temperature", "data": "40.00" },
      { "info": "Humidity", "data": "90.00" }
    ]
  },
  "boardInfo": {
    "firmwareVersion": "0.0",
    "architecture": "ESP32",
    "statusOTA": false,
    "deviceMAC": "DD:DA:0C:20:E1:41",
    "signalStrength": -30
  }
}
2. Get Historical Data (GET)
cURL Example


Copy
curl "https://api.consentiumiot.com/getData?receiveKey=<YOUR-RECEIVE-KEY>&boardKey=<YOUR-BOARD-KEY>"
Postman Example

Method: GET

URL:


Copy
https://api.consentiumiot.com/getData?receiveKey=<YOUR-RECEIVE-KEY>&boardKey=<YOUR-BOARD-KEY>
3. Get Most Recent Data (GET)
cURL Example


Copy
curl "https://api.consentiumiot.com/getData?recents=true&receiveKey=<YOUR-RECEIVE-KEY>&boardKey=<YOUR-BOARD-KEY>"
Postman Example

Method: GET

URL:


Copy
https://api.consentiumiot.com/getData?recents=true&receiveKey=<YOUR-RECEIVE-KEY>&boardKey=<YOUR-BOARD-KEY>
Endpoints Overview
HTTP Method
Endpoint
Purpose
POST

/v2/updateData

Submit or update telemetry data for a board.

GET

/getData

Retrieve all historical data feeds for a board.

GET

/getData?recents=true

Retrieve only the most recent feed(s).

1. Submit / Update Sensor Data
Endpoint

Copy
POST /v2/updateData?sendKey=<YOUR-SEND-KEY>&boardKey=<YOUR-BOARD-KEY>
Description
Used by IoT boards or edge devices to push telemetry data to the Consentium IoT platform.

Each update includes:

Sensor readings → flexible key-value structure.

Board metadata (boardInfo) → mandatory information about the device’s state and identity.

Request Body

Copy
{
  "sensors": {
    "sensorData": [
      {
        "info": "Temperature",
        "data": "40.00"
      },
      {
        "info": "Humidity",
        "data": "90.00"
      }
    ]
  },
  "boardInfo": {
    "firmwareVersion": "0.0",
    "architecture": "ESP32",
    "statusOTA": false,
    "deviceMAC": "DD:DA:0C:20:E1:41",
    "signalStrength": -30
  }
}
Response
200 OK


Copy
{
  "status": "success",
  "message": "Sensor data updated successfully"
}
Error Codes
422 Unprocessable Entity → Missing required boardInfo fields.

429 Too Many Requests → MAC address mismatch (board vs. physical device).

400 Bad Request → Malformed JSON or missing parameters.

401 Unauthorized → Invalid sendKey.

2. Get Historical Data
Endpoint

Copy
GET /getData?receiveKey=<YOUR-RECEIVE-KEY>&boardKey=<YOUR-BOARD-KEY>
Description
Fetch all stored feeds for a board. Each feed entry maps infoN (sensor name) to valueN (sensor reading).

Response

Copy
{
  "board": {
    "info1": "Temperature",
    "info2": "Humidity",
    "name": "Test board2"
  },
  "feeds": [
    {
      "updated_at": "2025-09-05T14:20:56.914801Z",
      "value1": 40.0,
      "value2": 90.0
    },
    {
      "updated_at": "2025-09-05T13:59:22.101432Z",
      "value1": 39.5,
      "value2": 88.0
    }
  ]
}
3. Get Most Recent Data
Endpoint

Copy
GET /getData?recents=true&receiveKey=<YOUR-RECEIVE-KEY>&boardKey=<YOUR-BOARD-KEY>
Description
Returns only the most recent feed entry for a board.

Response

Copy
{
  "board": {
    "info1": "Temperature",
    "info2": "Humidity",
    "name": "Test board2"
  },
  "feeds": [
    {
      "updated_at": "2025-09-05T14:20:56.914801Z",
      "value1": 40.0,
      "value2": 90.0
    }
  ]
}
Error Handling
The API uses standard HTTP status codes:

Status Code
Meaning
Example Scenario
200 OK

Success

Data retrieved or updated successfully.

400 Bad Request

Invalid request

Malformed JSON, missing params.

401 Unauthorized

Invalid/missing key

Wrong sendKey or receiveKey.

404 Not Found

Board not found

Invalid boardKey.

422 Unprocessable Entity

Validation error

Missing required boardInfo field.

429 Too Many Requests

MAC mismatch

Board’s virtual key doesn’t match reported MAC.

Best Practices
Rate limiting: Avoid excessive calls. Implement exponential backoff for retries.

Secure your keys: Never expose sendKey or receiveKey in client-side code.

Validate data locally: Ensure correct sensor units, numeric ranges, and field consistency.

Use device MAC correctly: Each virtual board is tagged with its physical MAC. Mismatches will be rejected.

Timestamp awareness: All timestamps are returned in UTC (ISO 8601 format).