import { useEffect, useMemo, useState } from 'react'
import Board from './components/Board.jsx'
import PiecePreview from './components/PiecePreview.jsx'
import { LEVELS } from './levels'

// 建立空棋盤
function emptyGrid(width, height) {
  return Array(width * height).fill(-1)
}

// 將後端的 placements 轉變成 棋盤
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

// Render base URL（production） / dev 用 proxy
const API_BASE = import.meta.env.PROD
  ? 'https://puzzle-game-698n.onrender.com'
  : ''

function apiUrl(path) {
  return import.meta.env.PROD
    ? new URL(path, API_BASE).toString()
    : `/${path}`
}


// App 主元件
export default function App() {
  // 關卡選擇狀態
  const [levelId, setLevelId] = useState(LEVELS[0].id)

  const level = useMemo(
    () =>LEVELS.find(l => l.id === levelId) ?? LEVELS[0],
    [levelId]
  )

  const width = level.width
  const height = level.height
  const pieceIds = level.pieceIds
  

  const [grid, setGrid] = useState(() => emptyGrid(width, height))
  const [loading, setLoading] = useState(false)
  const [msg, setMsg] = useState('')

  const [allPieces, setAllPieces] = useState([]) 
  // allPieces: [{ pieceId, cells:[{x,y}]}]
  const [piecesMsg, setPiecesMsg] = useState('')
  const levelPieces = useMemo(() => {
    const setIds = new Set(pieceIds)
    return allPieces.filter(p => setIds.has(p.pieceId))
  }, [allPieces, pieceIds])

  // 換關卡的時候 從所有的 pieces 中篩選出關卡需要的
  useEffect(() => {
    setGrid(emptyGrid(width, height))
    setMsg('')
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

        setAllPieces(data.pieces || [])
        setPiecesMsg('')
      } catch (e) {
        setPiecesMsg('Load pieces failed: ' + String(e))
      }
    }
    loadPieces()
  }, [levelId, width, height])

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

  // clear Board
  function clearBoard() {
    setGrid(emptyGrid(width, height))
    setMsg('')
  }

  // UI
  return (
    <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
      <h2>Puzzle Game</h2>
      
      <div>
        {/*----- 控制列 -----*/}
        <div style={{ display: 'flex', gap: 12, alignItems: 'center', marginBottom: 12  }}>
          
          {/*----- 關卡選擇 -----*/}
          <label>
            Level:&nbsp;
            <select value={levelId} onChange={(e) => setLevelId(e.target.value)}>
              {LEVELS.map(l =>(
                <option key={l.id} value={l.id}>
                  {l.name}
                </option>
              ))}
            </select>
          </label>

          {/* Solve */}
          <button onClick={solve} disabled={loading}>
            {loading ? 'Solving...' : 'Solve'}
          </button>

          {/* Clear */}
          <button onClick={clearBoard} disabled={loading}>
            Clear
          </button>

          {/* 訊息 */}
          <span style={{ marginLeft: 8 }}>{msg}</span>
        </div>
        {/* ===== 棋盤 ===== */}
        <Board width={width} height={height} grid={grid} />

        <p style={{ color: '#666', marginTop: 12 }}>
          選擇關卡後按 Solve 後端會回傳解答並顯示在棋盤上
        </p>
      </div>
      {/* 右：這關的 Pieces */}
      <div style={{ width: 320}}>
        <h3 style={{ marginTop: 0 }}>Pieces</h3>
        {piecesMsg && <div style={{ color: '#aaa', marginBottom: 8 }}>{piecesMsg}</div>}
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(4, 1fr)', gap: 12 }}>
          {levelPieces.map(p => (
            <PiecePreview key={p.pieceId} pieceId={p.pieceId} cells={p.cells} />
          ))}
        </div>
      </div>
 
    </div>
  )
}
