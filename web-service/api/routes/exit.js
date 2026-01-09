const express = require('express')
const router = express.Router()
const { store, getSpots } = require('../lib/data')

// POST /exit - mark spot as freed by license plate (simple helper endpoint)
router.post('/', (req, res, next) => {
  try {
    const { licensePlate, timestamp } = req.body
    if (!licensePlate) {
      const err = new Error('Invalid exit payload')
      err.status = 400
      throw err
    }

    // find spot occupied by this licensePlate and free it
    const spots = getSpots()
    const spot = spots.find(s => s.occupiedBy === licensePlate)
    if (!spot) {
      return res.status(404).json({ error: 'License plate not found in any spot' })
    }

    spot.occupiedBy = ''

    res.json({ message: 'Spot freed', spotID: spot.id, timestamp: timestamp || new Date().toISOString() })
  } catch (err) {
    next(err)
  }
})

module.exports = router
