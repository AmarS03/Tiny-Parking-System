# MongoDB Setup Guide for Tiny Parking System

## Overview
Your Tiny Parking System API is now fully integrated with MongoDB for persistent data storage. All vehicle data, logs, and system status are now saved in MongoDB instead of in-memory storage.

---

## ✅ What Was Configured

### 1. **Environment Variables** (.env.local)
- Created `.env.local` in `web-service/api/` directory
- Contains `MONGODB_URI` connection string from Vercel Storage
- Contains `DB_NAME` for database selection

### 2. **Database Connection Module** (lib/db.js)
- Manages MongoDB connection with connection pooling
- Caches connection for serverless environments (Vercel)
- Handles disconnection gracefully

### 3. **Data Access Layer** (lib/data.js)
All functions now support MongoDB operations:
- `getStore()` - Retrieve all parking system data
- `setStore(newStore)` - Update entire system data
- `addNewLog(type, message, imageUrl)` - Add log entries
- `setBoardStatus(updatedStatus)` - Update board status
- `parkVehicle(licensePlate)` - Register vehicle entry
- `removeParkedVehicle(licensePlate)` - Register vehicle exit
- `isLicensePlateAllowed(licensePlate)` - Check if plate is allowed
- `addAllowedPlate(plate)` - Add allowed license plate
- `removeAllowedPlate(plate)` - Remove allowed license plate
- `getAllLogs(limit)` - Retrieve logs from MongoDB
- `noOccupiedSpots()` - Check if parking is empty

### 4. **API Routes** (routes/)
All routes updated to handle async MongoDB operations:
- **entry.js** - POST /entry for vehicle entry with validation
- **exit.js** - POST /exit for vehicle departure
- **status.js** - GET/PUT /status for system status management

### 5. **Dependencies Installed**
```json
"mongodb": "^6.0.0",  // MongoDB driver
"dotenv": "^16.0.0"   // Environment variables
```

---

## 🚀 How to Deploy

### Step 1: Add Environment Variable to Vercel
1. Go to your Vercel project dashboard
2. Navigate to **Settings → Environment Variables**
3. Add the following variables:
   ```
   MONGODB_URI = mongodb+srv://username:password@cluster.mongodb.net/parking_db?retryWrites=true&w=majority
   DB_NAME = parking_db
   NODE_ENV = production
   ```

### Step 2: Install Dependencies
```bash
cd web-service/api
npm install mongodb dotenv
```

### Step 3: Test Locally
```bash
cd web-service/api
npm run dev
```

Then test the endpoints:
```bash
# Get current status
curl http://localhost:5000/status

# Test vehicle entry
curl -X POST http://localhost:5000/entry \
  -H "Content-Type: application/json" \
  -d '{
    "licensePlate": "ABC123",
    "imageUrl": "https://example.com/image.jpg",
    "recordedWeight": 1200
  }'

# Test vehicle exit
curl -X POST http://localhost:5000/exit \
  -H "Content-Type: application/json" \
  -d '{"licensePlate": "ABC123"}'
```

### Step 4: Deploy to Vercel
```bash
git add .
git commit -m "Add MongoDB integration for persistent storage"
git push
```

---

## 📊 Database Structure

### Collection: `store`
```javascript
{
  _id: "main",
  boardStatus: [
    { name: "ESP main module", status: "...", espStatus: "..." },
    // ... other sensors
  ],
  logs: [
    { type: "info", message: "...", imageUrl: "...", timestamp: "..." },
    // ... recent logs (max 100)
  ],
  allowedPlates: ["ABC123", "XYZ789"],
  spots: [
    { id: 1, isOccupied: false, occupiedBy: "", occupiedAt: null },
    // ... more spots
  ],
  openingTime: "08:00",
  closingTime: "22:00",
  createdAt: "2026-01-23T...",
  updatedAt: "2026-01-23T..."
}
```

### Collection: `logs`
Stores all system logs separately for better performance:
```javascript
{
  _id: ObjectId(...),
  type: "info|warning|error|success",
  message: "...",
  imageUrl: "...",
  timestamp: "2026-01-23T..."
}
```

---

## 🔄 Data Flow

```
ESP32 Device
    ↓
API Routes (entry/exit/status)
    ↓
lib/data.js (Business Logic)
    ↓
lib/db.js (MongoDB Connection)
    ↓
MongoDB (Vercel Storage)
```

---

## ⚠️ Important Notes

1. **Connection String Security**: Never commit `.env.local` to git. It's already in `.gitignore` by default.

2. **First Run**: On first deployment, MongoDB collections are created automatically on demand.

3. **Connection Pooling**: The connection is cached after first use for optimal performance in serverless environments.

4. **Async Operations**: All database functions are async. Always use `await` when calling them.

5. **Error Handling**: All functions include error handling and will log to MongoDB.

6. **Log Retention**: Logs are kept both in the `store` collection (recent 100) and in a separate `logs` collection (all logs).

---

## 🧪 Testing Checklist

- [ ] Environment variables set in `.env.local`
- [ ] `npm install` completed in web-service/api/
- [ ] `npm run dev` starts successfully
- [ ] GET /status returns data
- [ ] POST /entry accepts vehicle entry
- [ ] POST /exit accepts vehicle exit
- [ ] Logs appear in MongoDB
- [ ] Data persists after server restart

---

## 📝 Example: Adding a New Feature

If you want to add a new data property (e.g., `maxOccupancyLimit`):

1. Update the `defaultStore` in `lib/data.js`:
```javascript
const defaultStore = {
  // ... existing properties
  maxOccupancyLimit: 10,  // Add here
};
```

2. Create getter/setter functions in `lib/data.js`:
```javascript
export async function getMaxOccupancyLimit() {
  const store = await getStore();
  return store.maxOccupancyLimit;
}

export async function setMaxOccupancyLimit(limit) {
  const store = await getStore();
  store.maxOccupancyLimit = limit;
  await setStore(store);
}
```

3. Use in your routes with `await`.

---

## 🆘 Troubleshooting

**Issue**: "Cannot find module 'mongodb'"
- Solution: Run `npm install mongodb` in web-service/api/

**Issue**: "MONGODB_URI is not defined"
- Solution: Ensure `.env.local` exists and contains the connection string

**Issue**: "MongoServerError: connection timeout"
- Solution: Check your MongoDB connection string is correct and network access is allowed

**Issue**: "Cannot use `await` outside async function"
- Solution: Ensure your route handler is declared as `async`

---

## 📞 Support

If you encounter any issues:
1. Check the browser console for error messages
2. Check the server logs in terminal
3. Verify MongoDB connection string is correct
4. Check that all `await` keywords are in place
