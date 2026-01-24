const express = require('express');
const cors = require('cors');
const path = require('path');
const statusRouter = require('./routes/status');
const entryRouter = require('./routes/entry');
const exitRouter = require('./routes/exit');

const port = process.env.PORT || 5000;

const app = express();

// CORS configuration
const corsOptions = {
	origin: '*',
	methods: ['GET', 'POST', 'PUT', 'OPTIONS'],
	allowedHeaders: ['Content-Type'],
	credentials: false
};

app.use(cors(corsOptions));
app.use(express.json());
app.get('/', (_, res) => res.sendFile(path.join(__dirname, 'index.html')));

app.use('/status', statusRouter);
app.use('/entry', entryRouter);
app.use('/exit', exitRouter);

// Global error handler
app.use((err, req, res, next) => {
	console.error(err)
	res.status(err.status || 500).json({ error: err.message || 'Unhandled API error' })
});

module.exports = app;

// If deployed locally, directly start the server
if (require.main === module) {
	app.listen(port, () => {
		console.log(`Tiny Parking System API: listening on port ${port}`)
		console.log(`http://localhost:${port}`)
	});
}
