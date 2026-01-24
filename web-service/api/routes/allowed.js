const express = require("express");
const { addNewLog, setAllowedLicensePlates } = require("../lib/data");
const { isLicensePlateValid } = require("../lib/utils");

const router = express.Router();

// PUT /status/allowed - updates allowed license plates
router.put("/", (req, res, next) => {
    try {
        const { allowedPlates } = req.body;

        if (Array.isArray(allowedPlates) && allowedPlates.every(plate => isLicensePlateValid(plate))) {
            setAllowedLicensePlates(allowedPlates);
            addNewLog("info", "Allowed license plates list updated");
            res.json({ message: "Allowed license plates list updated" });
        } else {
            const err = new Error("Invalid allowed license plates payload");
            err.status = 400;
            throw err;
        }
    } catch (err) {
        addNewLog(
            "error", 
            `API error on PUT /status/allowed: ${err.message}`
        );

        next(err);
    }
});

module.exports = router;