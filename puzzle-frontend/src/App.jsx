import { useEffect, useMemo, useState } from 'react'
import Board from './components/Board.jsx'
import PiecePreview from './components/PiecePreview.jsx'

// 建立空棋盤（-1 代表空）
function emptyGrid(width, height) {
  return Array(width * height).fill(-1)
}

// 將後端的 placements 轉成棋盤 grid
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

// Production( GitHub Pages ) 打 Render；Dev(本機) 走 Vite proxy
const API_BASE = import.meta.env.PROD
  ? 'https://puzzle-game-698n.onrender.com'
  : ''

function apiUrl(path) {
  // path 例如 'solve' / 'pieces' / 'levels'
  return import.meta.env.PROD ? new URL(path, API_BASE).toString() : `/${path}`
}

export default function App() {
  // -----------------------
  // (A) Levels 狀態
  // -----------------------
  const [levels, setLevels] = useState([])
  const [levelsMsg, setLevelsMsg] = useState('')
  const [levelId, setLevelId] = useState('')

  // -----------------------
  // (B) Pieces 狀態（全部 pieces）
  // -----------------------
  const [allPieces, setAllPieces] = useState([])
  const [piecesMsg, setPiecesMsg] = useState('')

  // -----------------------
  // (C) 棋盤狀態
  // -----------------------
  const [grid, setGrid] = useState(() => emptyGrid(1, 1)) // 初始先給 1x1，避免 level 還沒載入就爆
  const [loading, setLoading] = useState(false)
  const [msg, setMsg] = useState('')

  // -----------------------
  // (D) 取得目前關卡（可能為 null）
  // -----------------------
  const level = useMemo(() => {
    return levels.find(l => l.id === levelId) || null
  }, [levels, levelId])

  // 給安全預設，避免 level 為 null 時崩潰
  const width = level ? level.width : 1
  const height = level ? level.height : 1
  const pieceIds = level ? level.pieceIds : []

  // -----------------------
  // (E) 右側顯示：只顯示本關需要的 pieces
  // -----------------------
  const levelPieces = useMemo(() => {
    const setIds = new Set(pieceIds)
    return allPieces.filter(p => setIds.has(p.pieceId))
  }, [allPieces, pieceIds])

  // =====================================================
  // useEffect 1：初次載入 levels
  // =====================================================
  useEffect(() => {
    async function loadLevels() {
      setLevelsMsg('Loading levels...')
      try {
        const res = await fetch(apiUrl('levels'))
        const data = await res.json()
        const list = data.levels || []

        setLevels(list)
        setLevelsMsg('')

        // 預設選第一關
        if (list.length > 0) setLevelId(list[0].id)
      } catch (e) {
        setLevelsMsg('Load levels failed: ' + String(e))
      }
    }
    loadLevels()
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  // =====================================================
  // useEffect 2：初次載入 pieces
  // =====================================================
  useEffect(() => {
    async function loadPieces() {
      setPiecesMsg('Loading pieces...')
      try {
        const res = await fetch(apiUrl('pieces'))
        const data = await res.json()

        setAllPieces(data.pieces || [])
        setPiecesMsg('')
      } catch (e) {
        setPiecesMsg('Load pieces failed: ' + String(e))
      }
    }
    loadPieces()
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  // =====================================================
  // useEffect 3：切換關卡時清空棋盤
  // =====================================================
  useEffect(() => {
    if (!level) return
    setGrid(emptyGrid(level.width, level.height))
    setMsg('')
  }, [levelId, level])

  // =====================================================
  // Solve：呼叫後端 /solve
  // =====================================================
  async function solve() {
    if (!level) return

    setLoading(true)
    setMsg('Solving...')

    try {
      const res = await fetch(apiUrl('solve'), {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ width, height, pieceIds }),
      })

      // 用 text -> JSON.parse，避免後端回非 JSON 時直接炸
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

  // 清空棋盤
  function clearBoard() {
    setGrid(emptyGrid(width, height))
    setMsg('')
  }

  // =====================================================
  // Levels 還沒載入時：顯示 loading
  // =====================================================
  if (!level) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>{levelsMsg || 'No levels'}</div>
      </div>
    )
  }

  // =====================================================
  // UI
  // =====================================================
  return (
    <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
      <h2>Puzzle Game</h2>

      <div style={{ display: 'flex', gap: 16, alignItems: 'flex-start' }}>
        {/* 左側：控制列 + 棋盤 */}
        <div>
          <div style={{ display: 'flex', gap: 12, alignItems: 'center', marginBottom: 12 }}>
            {/* 關卡選擇 */}
            <label>
              Level:&nbsp;
              <select value={levelId} onChange={(e) => setLevelId(e.target.value)}>
                {levels.map(l => (
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

          <Board width={width} height={height} grid={grid} />

          <p style={{ color: '#666', marginTop: 12 }}>
            選擇關卡後按 Solve，後端會回傳解答並顯示在棋盤上
          </p>
        </div>

        {/* 右側：本關需要的 pieces */}
        <div style={{ width: 520 }}>
          <h3 style={{ marginTop: 0 }}>Pieces</h3>
          {piecesMsg && <div style={{ color: '#aaa', marginBottom: 8 }}>{piecesMsg}</div>}

          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(4, 1fr)', gap: 12 }}>
            {levelPieces.map(p => (
              <PiecePreview key={p.pieceId} pieceId={p.pieceId} cells={p.cells} />
            ))}
          </div>
        </div>
      </div>
    </div>
  )
}
