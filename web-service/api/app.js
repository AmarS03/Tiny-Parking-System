const express = require('express');
const statusRouter = require('./routes/status');
const entryRouter = require('./routes/entry');
const exitRouter = require('./routes/exit');

const app = express();
const port = process.env.PORT || 5000;
app.use(express.json());
app.get('/', (_, res) => res.sendFile('./index.html', { root: __dirname }));

app.use('/status', statusRouter);
app.use('/entry', entryRouter);
app.use('/exit', exitRouter);

// Global error handler
app.use((err, req, res, next) => {
  console.error(err)
  res.status(err.status || 500).json({ error: err.message || 'Unhandled API error' })
});

app.listen(port, () => {
  console.log(`Tiny Parking System API: listening on port ${port}`)
});
