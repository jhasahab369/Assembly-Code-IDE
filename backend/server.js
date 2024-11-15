const express = require('express');
const fs = require('fs');
const { exec } = require('child_process');
const path = require('path');
const cors = require('cors');

const app = express();
const port = 3000;

// Middleware to parse JSON requests
app.use(express.json());
app.use(cors({
    origin: 'http://localhost:5173',
}));

// Temporary directory for assembly files
const tempDir = path.join(__dirname, 'temp');
if (!fs.existsSync(tempDir)) {
  fs.mkdirSync(tempDir);
}

// Save assembly code to a file
app.post('/assemble', (req, res) => {
    console.log("got the request to assemble \n");
  const { assemblyCode } = req.body;
  const tempFile = path.join(tempDir, 'temp.asm');

  fs.writeFileSync(tempFile, assemblyCode);

  exec(`./ass ${tempFile}`, (error, stdout, stderr) => {
    if (error) {
      res.status(500).json({ error: stderr });
    } else {
      res.json({
        logOutput: stdout,
        logOutput: fs.readFileSync(path.join(tempDir, 'temp.log'), 'utf-8'),
        lstOutput: fs.readFileSync(path.join(tempDir, 'temp.lst'), 'utf-8'),
        objectFile: 'temp.o',
      });
    }
  });
});

// Emulate the object file
app.post('/emulate', (req, res) => {
  const { objectFile } = req.body;
  console.log("emulate ho rah \n ",objectFile);
  const tempFile = path.join(tempDir, objectFile);
  console.log("got the request to emulate \n");
  exec(`./emo -trace ${tempFile}`, (error, stdout, stderr) => {
    if (error) {
      res.status(500).json({ error: stderr });
    } else {
      res.json({ emulatorOutput: stdout });
    }
  });
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});