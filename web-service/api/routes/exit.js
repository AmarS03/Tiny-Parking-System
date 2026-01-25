const express = require("express");
const { addNewLog, removeParkedVehicle, getParkingSpots } = require("../lib/data");

const router = express.Router();

// POST /exit - mark spot as freed by license plate (simple helper endpoint)
router.post("/", (req, res, next) => {
    try {
        const { licensePlate } = req.body;
	
        if (!licensePlate) {
            const err = new Error("Invalid exit payload (no license plate)");
            err.status = 400;
            throw err;
        }
		
        // Finds spot occupied by this licensePlate and free it
		const freed = removeParkedVehicle(licensePlate);

		// LIMITATION: due to the project simplcity, if the license plate 
		// is not found, we simulate the exit by removing a random spot instead
		// (this is because we don't have another camera sensor for exit detection)
        if (!freed && !noOccupiedSpots()) {
			const spot = getParkingSpots.find(spot => spot.isOccupied);
			licensePlate = spot.occupiedBy;
			removeParkedVehicle(spot.occupiedBy);
        }
		
		addNewLog(
			"success", 
			`Vehicle exit detected, with license plate ${licensePlate}`
		);
    } catch (err) {
		addNewLog(
			"error", 
			`API error on POST /exit: ${err.message}`
		);
		
        next(err);
    }
});

module.exports = router;

