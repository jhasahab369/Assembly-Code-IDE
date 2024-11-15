import React, { useState } from 'react';
import axios from 'axios';
import './AssemblyIDE.css';  // We'll create this CSS file next

const AssemblyIDE = () => {
  const [assemblyCode, setAssemblyCode] = useState('');
  const [logOutput, setLogOutput] = useState('');
  const [lstOutput, setLstOutput] = useState('');
  const [emulatorOutput, setEmulatorOutput] = useState('');
  const [activeTab, setActiveTab] = useState('logs');

  const handleSave = async () => {
    try {
      const response = await axios.post('http://localhost:3000/assemble', { assemblyCode });
      setLogOutput(response.data.logOutput);
      setLstOutput(response.data.lstOutput);
    } catch (error) {
      console.error('Error saving assembly code:', error);
    }
  };

  const handleAssemble = async () => {
    try {
      const response = await axios.post('http://localhost:3000/assemble', { assemblyCode });
      setLogOutput(response.data.logOutput);
      setLstOutput(response.data.lstOutput);
      setEmulatorOutput(response.data.emulatorOutput);
    } catch (error) {
      console.error('Error assembling code:', error);
    }
  };

  const handleEmulate = async () => {
    try {
      const response = await axios.post('http://localhost:3000/emulate', { objectFile: 'temp.o' });
      setEmulatorOutput(response.data.emulatorOutput);
      setActiveTab('emulator');
    } catch (error) {
      console.error('Error emulating object file:', error);
    }
  };

  return (
    <div className="ide-container">
      {/* Header */}
      <header className="header">
        <h1>Assembly IDE</h1>
        <div className="button-group">
          <button className="btn btn-save" onClick={handleSave}>Save</button>
          <button className="btn btn-assemble" onClick={handleAssemble}>Assemble</button>
          <button className="btn btn-emulate" onClick={handleEmulate}>Emulate</button>
        </div>
      </header>

      {/* Main Content */}
      <div className="main-content">
        {/* Code Editor */}
        <div className="editor-panel">
          <div className="editor-container">
            <div className="panel-header">
              <h2>Assembly Code</h2>
            </div>
            <textarea
              className="code-editor"
              value={assemblyCode}
              onChange={(e) => setAssemblyCode(e.target.value)}
              placeholder="Enter your assembly code here..."
            />
          </div>
        </div>

        {/* Output Panels */}
        <div className="output-panel">
          <div className="tab-header">
            <button
              className={`tab-button ${activeTab === 'logs' ? 'active' : ''}`}
              onClick={() => setActiveTab('logs')}
            >
              Logs & Listing
            </button>
            <button
              className={`tab-button ${activeTab === 'emulator' ? 'active' : ''}`}
              onClick={() => setActiveTab('emulator')}
            >
              Emulator Trace
            </button>
          </div>

          <div className="tab-content">
            {activeTab === 'logs' && (
              <div className="logs-content">
                <div className="output-section">
                  <h3>Log Output</h3>
                  <pre className="output-text">
                    {logOutput || 'No log output available'}
                  </pre>
                </div>
                
                <div className="output-section">
                  <h3>Listing Output</h3>
                  <pre className="output-text">
                    {lstOutput || 'No listing output available'}
                  </pre>
                </div>
              </div>
            )}

            {activeTab === 'emulator' && (
              <div className="emulator-content">
                <div className="output-section">
                  <h3>Emulator Output</h3>
                  <pre className="output-text">
                    {emulatorOutput || 'No emulator output available'}
                  </pre>
                </div>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};

export default AssemblyIDE;