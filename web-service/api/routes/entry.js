const express = require("express");
const { addNewLog, isLicensePlateAllowed, parkVehicle } = require("../lib/data");
const { isLicensePlateValid } = require("../lib/utils");

const router = express.Router();

// POST /entry - records a vehicle entering and returns the entry result
router.post("/", (req, res, next) => {
    try {
        const { licensePlate, imageUrl, recordedWeight } = req.body;
	
		addNewLog(
			"info", 
			`Vehicle entry detected, with license plate ${licensePlate} and weight ${recordedWeight}`,
			imageUrl
		);

        if (!isLicensePlateValid(licensePlate)) {
            addNewLog(
				"warning", 
				`Vehicle entry with plate ${licensePlate} denied (invalid format)`
			);
			
        	return res.json(
				{
					allowed: false,
					message: `Invalid license plate`
				}
			);
        } else if (!isLicensePlateAllowed(licensePlate)) {
            addNewLog(
				"warning", 
				`Vehicle entry with plate ${licensePlate} denied (not in allowed list)`
			);
			
        	return res.json(
				{
					allowed: false,
					message: `License plate denied`
				}
			);
        } else {
			const availableSpot = parkVehicle(licensePlate);

			if (!availableSpot) {
				addNewLog(
					"warning", 
					`Vehicle entry with plate ${licensePlate} denied (no available parking spots)`
				);
				
				return res.json(
					{
						allowed: false,
						message: `No parking spots available`
					}
				);
			} else {
				addNewLog(
					"success", 
					`Vehicle entry with plate ${licensePlate} allowed`
				);
				
				return res.json(
					{
						allowed: true,
						message: `License plate allowed`
					}
				);
			}
		}
    } catch (err) {
		addNewLog(
			"error", 
			`API error on POST /entry: ${err.message}`
		);
		
        next(err);
    }
});

module.exports = router;

