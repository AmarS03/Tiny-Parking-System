const express = require('express')
const router = express.Router()
const { store, updateStore, getBoardStatus, getSpots, getOpeningHours, getLogs } = require('../lib/data')

// GET /status - returns current system status
router.get('/', (req, res, next) => {
  try {
    res.json({
      boardStatus: getBoardStatus(),
      parkingSpots: getSpots().length,
      openingTime: getOpeningHours().openingTime,
      closingTime: getOpeningHours().closingTime,
    })
  } catch (err) {
    next(err)
  }
})

// PUT /status - initialize or update system status
router.put('/', (req, res, next) => {
  try {
    const status = req.body
    if (!status || typeof status.parkingSpots !== 'number') {
      const err = new Error('Invalid systemStatus payload')
      err.status = 400
      throw err
    }

    updateStore(status)
    res.json({ message: 'System status updated/initialized successfully' })
  } catch (err) {
    next(err)
  }
})

module.exports = router
