import { useMemo, useState } from 'react'
import Board from './components/Board.jsx'

function emptyGrid(width, height) {
  return Array(width * height).fill(-1)
}

function applyPlacementsToGrid(width, height, placements) {
  const g = emptyGrid(width, height)

  for (const p of placements) {
    const id = p.pieceId
    for (const c of p.cells) {
      const x = c.x
      const y = c.y
      if (x < 0 || x >= width || y < 0 || y >= height) continue
      g[y * width + x] = id
    }
  }

  return g
}

const API_BASE = import.meta.env.PROD
  ? 'https://puzzle-game-698n.onrender.com/'   // production server URL
  : '';

export default function App() {
  const [width, setWidth] = useState(10)
  const [height, setHeight] = useState(5)

  const [grid, setGrid] = useState(() => emptyGrid(10, 5))
  const [loading, setLoading] = useState(false)
  const [msg, setMsg] = useState('')

  const pieceIds = useMemo(() => [0, 1, 2, 3, 5, 7, 8, 9, 10, 11], [])

  async function solve() {
    setLoading(true)
    setMsg('Solving...')
    try {
      const url = `${API_BASE}/solve`;
      const res = await fetch(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          width,
          height,
          pieceIds,
        }),
      })

      const data = await res.json()

      if (!data.solved) {
        setMsg(data.error || 'No solution')
        setGrid(emptyGrid(width, height))
        return
      }

      setMsg('Solved!')
      const newGrid = applyPlacementsToGrid(width, height, data.placements)
      setGrid(newGrid)
    } catch (e) {
      setMsg('Request failed: ' + String(e))
    } finally {
      setLoading(false)
    }
  }

  function clearBoard() {
    setGrid(emptyGrid(width, height))
    setMsg('')
  }

  return (
    <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
      <h2>Puzzle Game</h2>

      <div style={{ display: 'flex', gap: 12, alignItems: 'center', marginBottom: 12 }}>
        <label>
          W:&nbsp;
          <input
            type="number"
            value={width}
            min={1}
            onChange={(e) => setWidth(Number(e.target.value))}
            style={{ width: 60 }}
          />
        </label>

        <label>
          H:&nbsp;
          <input
            type="number"
            value={height}
            min={1}
            onChange={(e) => setHeight(Number(e.target.value))}
            style={{ width: 60 }}
          />
        </label>

        <button onClick={solve} disabled={loading}>
          {loading ? 'Solving...' : 'Solve'}
        </button>

        <button onClick={clearBoard} disabled={loading}>
          Clear
        </button>

        <span style={{ marginLeft: 8 }}>{msg}</span>
      </div>

      <Board width={width} height={height} grid={grid} />

      <p style={{ color: '#666', marginTop: 12 }}>
        按下 Solve 後端會送出答案 顯示在棋盤上
      </p>
    </div>
  )
}
