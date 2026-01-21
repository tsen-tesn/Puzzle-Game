import { useEffect, useMemo, useState } from 'react'
import Board from './components/Board.jsx'
import PiecePreview from './components/PiecePreview.jsx'

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

// ✅ Render base URL（production） / dev 用 proxy
const API_BASE = import.meta.env.PROD
  ? 'https://puzzle-game-698n.onrender.com'
  : ''

function apiUrl(path) {
  // production: 絕對網址；dev: 走 vite proxy
  return import.meta.env.PROD
    ? new URL(path, API_BASE).toString()
    : `/${path}`
}

export default function App() {
  const [width, setWidth] = useState(10)
  const [height, setHeight] = useState(5)

  const [grid, setGrid] = useState(() => emptyGrid(10, 5))
  const [loading, setLoading] = useState(false)
  const [msg, setMsg] = useState('')

  // ✅ 新增：pieces 清單（來自 GET /pieces）
  const [pieces, setPieces] = useState([]) // [{pieceId, cells:[{x,y}]}]
  const [piecesMsg, setPiecesMsg] = useState('')

  const pieceIds = useMemo(() => [0, 1, 2, 3, 5, 7, 8, 9, 10, 11], [])

  // ✅ 載入 pieces（右側預覽）
  useEffect(() => {
    async function loadPieces() {
      setPiecesMsg('Loading pieces...')
      try {
        const res = await fetch(apiUrl('pieces'))
        const text = await res.text()

        let data
        try {
          data = JSON.parse(text)
        } catch {
          setPiecesMsg(`Pieces not JSON (status ${res.status}): ${text.slice(0, 120)}`)
          return
        }

        setPieces(data.pieces || [])
        setPiecesMsg('')
      } catch (e) {
        setPiecesMsg('Load pieces failed: ' + String(e))
      }
    }
    loadPieces()
  }, [])

  async function solve() {
    setLoading(true)
    setMsg('Solving...')
    try {
      const res = await fetch(apiUrl('solve'), {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ width, height, pieceIds }),
      })

      const text = await res.text()
      let data
      try {
        data = JSON.parse(text)
      } catch {
        setMsg(`Backend not JSON (status ${res.status}): ${text.slice(0, 120)}`)
        return
      }

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

      <div style={{ display: 'flex', gap: 16, alignItems: 'flex-start' }}>
        {/* 左：控制 + 棋盤 */}
        <div>
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

        {/* 右：Pieces Palette */}
        <div style={{ width: 320 }}>
          <h3 style={{ marginTop: 0 }}>Pieces</h3>
          {piecesMsg && <div style={{ color: '#aaa', marginBottom: 8 }}>{piecesMsg}</div>}

          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(2, 1fr)', gap: 12 }}>
            {pieces.map(p => (
              <PiecePreview key={p.pieceId} pieceId={p.pieceId} cells={p.cells} />
            ))}
          </div>
        </div>
      </div>
    </div>
  )
}
