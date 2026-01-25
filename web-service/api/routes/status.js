const express = require("express");
const { addNewLog, getStore, setBoardStatus } = require("../lib/data");

const router = express.Router();

// GET /status - returns current board status
router.get("/", (req, res, next) => {
    try {
        res.json(getStore());
    } catch (err) {
		addNewLog(
			"error", 
			`API error on GET /status: ${err.message}`
		);
		
        next(err);
    }
});

// PUT /status - initialize or update board status
router.put("/", (req, res, next) => {
    try {
        const updatedStatus = req.body;

        if (!updatedStatus) {
            const err = new Error("Invalid system status payload");
            err.status = 400;
            throw err;
        } else {
            setBoardStatus(updatedStatus);
            addNewLog("info", "ESP system started");
            res.json({ message: "ESP system started" });
        }
    } catch (err) {
		addNewLog(
			"error", 
			`API error on PUT /status: ${err.message}`
		);
		
        next(err);
    }
});

module.exports = router;

