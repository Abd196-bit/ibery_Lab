import { useState } from 'react'
import { IberyCompiler } from './compiler/compiler'
import './App.css'

const EXAMPLE_CODE = `// Example Ibery code
function main() {
    print("Hello, Ibery!")
    quantum_op()
    run_command("echo 'Quantum operation completed'")
}

main()
`

function App() {
  const [code, setCode] = useState(EXAMPLE_CODE)
  const [output, setOutput] = useState('')
  const compiler = new IberyCompiler()

  const handleRun = () => {
    try {
      // Compile the code
      const binary = compiler.compile(code)
      
      // Disassemble the binary to show the generated code
      const disassembled = compiler.disassemble(binary)
      
      setOutput(`Compilation successful!\n\nGenerated code:\n${disassembled}`)
    } catch (error) {
      setOutput(`Error: ${error instanceof Error ? error.message : String(error)}`)
    }
  }

  return (
    <div className="App">
      <h1>Ibery Language IDE</h1>
      <div className="ide-container">
        <div className="editor-container">
          <textarea
            className="code-editor"
            value={code}
            onChange={(e) => setCode(e.target.value)}
            placeholder="Write your Ibery code here..."
          />
          <button onClick={handleRun} className="run-button">
            Compile & Run
          </button>
        </div>
        <div className="output-container">
          <h3>Compiler Output</h3>
          <pre className="output">{output}</pre>
        </div>
      </div>
    </div>
  )
}

export default App 