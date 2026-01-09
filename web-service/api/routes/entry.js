const express = require('express')
const router = express.Router()
const { store, initSpots, getSpots } = require('../lib/data')

let nextEntryId = 1

// POST /entry - records a vehicle entering and returns entryID
router.post('/', (req, res, next) => {
  try {
    const { licensePlate, recordedWeight, timestamp } = req.body
    if (!licensePlate || typeof recordedWeight !== 'number') {
      const err = new Error('Invalid entryRequest payload')
      err.status = 400
      throw err
    }

    // create an entry record and push to logs
    const entry = {
      entryID: String(nextEntryId++),
      licensePlate,
      recordedWeight,
      timestamp: timestamp || new Date().toISOString(),
      allowed: null,
      message: 'pending',
    }

    store.logs.push(entry)

    res.json(Number(entry.entryID))
  } catch (err) {
    next(err)
  }
})

// GET /entry/:entryID - returns entry result if recorded
router.get('/:entryID', (req, res, next) => {
  try {
    const { entryID } = req.params
    const found = store.logs.find(e => e.entryID === entryID)
    if (!found) {
      return res.status(404).json({ error: 'Not found: invalid entryID' })
    }

    const response = {
      entryID: found.entryID,
      allowed: !!found.allowed,
      message: found.message,
      timestamp: found.timestamp,
    }

    res.json(response)
  } catch (err) {
    next(err)
  }
})

module.exports = router
