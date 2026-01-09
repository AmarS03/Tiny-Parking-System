# Tiny Parking System - API

This folder contains a minimal Express.js implementation of the API described in the project's README (`openapi.yaml`).

Quick start

1. cd into `web-service/api`
2. npm install
3. npm run dev (or npm start)

Endpoints

- `POST /api/auth/login` → { username, password } returns `{ token }` (use `admin`/`admin` for admin role in this demo)
- `GET /api/status` → system status
- `GET /api/spots`, `POST /api/spots` (admin), `GET /api/spots/:id`, `PATCH /api/spots/:id` (admin)
- `GET /api/vehicles`, `POST /api/vehicles`, `GET /api/vehicles/:plate`, `DELETE /api/vehicles/:plate`
- `POST /api/events/entry`, `POST /api/events/exit`
- `GET /api/sensors`, `POST /api/sensors`
- `GET /api/config/opening-hours`, `PATCH /api/config/opening-hours` (admin)
- `GET /api/dashboard/stream` → Server Sent Events stream for real-time updates

Notes

- The implementation uses an in-memory store for simplicity. No persistence is included.
- JWT secret is `change_me` unless `JWT_SECRET` is set.
- See `openapi.yaml` for the API model.
