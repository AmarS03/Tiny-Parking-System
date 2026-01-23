const express = require("express");
const { getStore, setBoardStatus, addLog, addNewLog, addLogSubscriber } = require("../lib/data");

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

// GET /status/logs/stream - SSE endpoint for real-time log updates
router.get("/logs/stream", (req, res, next) => {
    try {
        // Set SSE headers
        res.setHeader('Content-Type', 'text/event-stream');
        res.setHeader('Cache-Control', 'no-cache');
        res.setHeader('Connection', 'keep-alive');
        res.setHeader('Access-Control-Allow-Origin', '*');
        
        // Add this client as a subscriber
        const cleanup = addLogSubscriber(res);
        
        // Send initial logs
        const store = getStore();
        store.logs.forEach(log => {
            res.write(`data: ${JSON.stringify(log)}\n\n`);
        });
        
        // Handle client disconnect
        req.on('close', () => {
            cleanup();
            res.end();
        });
    } catch (err) {
        addNewLog("error", `SSE connection error: ${err.message}`);
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
            addNewLog("info", "System started");
            res.json({ message: "System started" });
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

