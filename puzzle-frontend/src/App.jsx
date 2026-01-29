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
const API_BASE = import.meta.env.PROD ? 'https://puzzle-game-698n.onrender.com' : ''

function apiUrl(path) {
  const p = path.startsWith('/') ? path : `/${path}`
  return import.meta.env.PROD ? `${API_BASE}${p}` : p
}

export default function App() {
  // -----------------------
  // (A) Levels 狀態（其實是從 /groups flatten 出來的 levels）
  // -----------------------
  const [levels, setLevels] = useState([])
  const [levelsMsg, setLevelsMsg] = useState('')
  const [levelId, setLevelId] = useState('') // ★ 這裡存的是 fullId

  // -----------------------
  // (B) Pieces 狀態（全部 pieces）
  // -----------------------
  const [allPieces, setAllPieces] = useState([])
  const [piecesMsg, setPiecesMsg] = useState('')

  // -----------------------
  // (C) 棋盤狀態
  // -----------------------
  const [grid, setGrid] = useState(() => emptyGrid(1, 1))
  const [loading, setLoading] = useState(false)
  const [msg, setMsg] = useState('')

  // -----------------------
  // (D) 取得目前關卡（★ 用 fullId 找）
  // -----------------------
  const level = useMemo(() => {
    return levels.find(l => l.fullId === levelId) || null
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
  // useEffect 1：初次載入 groups -> flatten 成 levels
  // =====================================================
  useEffect(() => {
    async function loadLevelsFromGroups() {
      setLevelsMsg('Loading levels...')
      try {
        const res = await fetch(apiUrl('groups'))

        if (!res.ok) {
          const text = await res.text()
          setLevelsMsg(`Load levels failed (HTTP ${res.status}): ${text.slice(0, 120)}`)
          setLevels([])
          setLevelId('')
          return
        }

        const data = await res.json()
        const groups = data.groups || []

        const flatLevels = groups.flatMap((g) =>
          (g.levels || []).map((lv) => {
            const groupId = g.groupId ?? g.id ?? g.name
            const lvId = lv.id ?? lv.name
            return {
              ...lv,
              groupId,
              groupName: g.name ?? String(groupId),
              fullId: `${groupId}/${lvId}`, // ✅ 全域唯一
            }
          })
        )

        setLevels(flatLevels)
        setLevelId(flatLevels[0]?.fullId ?? '')
        setLevelsMsg('')
      } catch (e) {
        setLevelsMsg('Load levels failed: ' + String(e))
        setLevels([])
        setLevelId('')
      }
    }

    loadLevelsFromGroups()
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
        if (!res.ok) {
          const text = await res.text()
          setPiecesMsg(`Load pieces failed (HTTP ${res.status}): ${text.slice(0, 120)}`)
          return
        }
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
  // 顯示 Loading / No levels（更精準）
  // =====================================================
  if (levelsMsg) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>{levelsMsg}</div>
      </div>
    )
  }

  if (levels.length === 0) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>No levels</div>
      </div>
    )
  }

  if (!level) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>Invalid levelId: {String(levelId)}</div>
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
            {/* 關卡選擇（★ value/key 都用 fullId） */}
            <label>
              Level:&nbsp;
              <select value={levelId} onChange={(e) => setLevelId(e.target.value)}>
                {levels.map(l => (
                  <option key={l.fullId} value={l.fullId}>
                    {l.groupName} / {l.name ?? l.id}
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
