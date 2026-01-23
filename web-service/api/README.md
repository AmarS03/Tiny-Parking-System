# Tiny Parking System - API

This folder contains a minimal Express.js implementation of the Tiny Parking System API. The API manages parking spot availability, vehicle entry/exit, and system status monitoring for an automated parking facility.

## Quick Start

1. Navigate to the `web-service/api` directory
2. Run `npm install` to install dependencies
3. Run `node app.js` to start the server (default port: 5000)

## API Endpoints

### System Status
- **GET /status** → Returns current system status including board status, logs, allowed plates, and parking spots
- **PUT /status** → Updates or initializes the system status with new board state

### Vehicle Entry
- **POST /entry** → Records a vehicle entry attempt
  - Request: `{ licensePlate, recordedWeight, imageUrl? }`
  - Response: `{ allowed: boolean, message: string }`
  - Validates license plate format (alphanumeric, max 7 characters)
  - Checks if license plate is in the allowed list
  - Parks vehicle if spot is available

### Vehicle Exit
- **POST /exit** → Records a vehicle exiting the parking system
  - Request: `{ licensePlate }`
  - Response: `{ message: string }`
  - Removes vehicle from parking spot

## Data Structure

### System Status Object
```json
{
  "boardStatus": [
    {
      "name": "string",
      "status": "No connection|Active|Failed",
      "espStatus": "string"
    }
  ],
  "logs": [
    {
      "type": "info|success|warning|error",
      "message": "string",
      "imageUrl": "string (optional)",
      "timestamp": "ISO 8601"
    }
  ],
  "allowedPlates": ["string"],
  "spots": [
    {
      "id": "integer",
      "isOccupied": "boolean",
      "occupiedBy": "string",
      "occupiedAt": "ISO 8601 or null"
    }
  ],
  "openingTime": "HH:mm",
  "closingTime": "HH:mm"
}
```

## Features

- **In-memory store** - All data resets on restart (no persistence)
- **License plate validation** - Format check: alphanumeric, 1-7 characters
- **Parking spot management** - Tracks 10 parking spots by default
- **Event logging** - Detailed logs of all system events with timestamps
- **Module status tracking** - Monitors ESP, sensors, and WiFi module status
- **Image support** - Optional image URLs can be attached to entry logs

## Configuration

Environment variables:
- `PORT` - Server port (default: 5000)

## Implementation Notes

- The API uses ES modules (import/export syntax)
- Express.json middleware is configured for JSON request bodies
- All errors are logged to the system logs
- The system tracks 10 parking spots by default
- Operating hours can be configured via system status updates
- See `openapi.yaml` for complete API specification

## Project Structure

```
.
├── app.js              # Main Express application
├── routes/
│   ├── status.js       # Status endpoints
│   ├── entry.js        # Vehicle entry endpoint
│   └── exit.js         # Vehicle exit endpoint
├── lib/
│   ├── data.js         # Core data store and operations
│   └── utils.js        # Utility functions (license plate validation)
├── openapi.yaml        # OpenAPI 3.0 specification
└── package.json        # Dependencies and scripts
```
