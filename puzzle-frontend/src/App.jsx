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
  // (A) Groups 狀態（大關卡）
  // -----------------------
  const [groups, setGroups] = useState([])

  const GROUP_ORDER = [
    'The small slam',
    'The slam1',
    'The slam2',
    'The slam3',
    'The slam4',
    'The grand slam',
    'The super slam1',
    'The super slam2',
    'The super slam3',
    'The super slam4',
  ]

  // 排序後的 groups
  const orderedGroups = useMemo(() => {
    const rank = new Map(GROUP_ORDER.map((id, i) => [String(id), i]))
    return [...groups].sort((a, b) => {
      const ra = rank.has(String(a.groupId)) ? rank.get(String(a.groupId)) : 9999
      const rb = rank.has(String(b.groupId)) ? rank.get(String(b.groupId)) : 9999
      if (ra !== rb) return ra - rb
      // 沒在清單裡的：用 name 當穩定排序（可自行改）
      return String(a.name ?? '').localeCompare(String(b.name ?? ''))
    })
  }, [groups])

  const [levelsMsg, setLevelsMsg] = useState('')
  const [groupId, setGroupId] = useState('') // ★ 大關卡
  const [levelId, setLevelId] = useState('') // ★ 小關卡（在 group 內）

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
  // (D) 目前 group / level（由 groupId + levelId 決定）
  // -----------------------
  const currentGroup = useMemo(() => {
    return groups.find(g => String(g.groupId) === String(groupId)) || null
  }, [groups, groupId])

  const level = useMemo(() => {
    if (!currentGroup) return null
    return (currentGroup.levels || []).find(lv => String(lv.id) === String(levelId)) || null
  }, [currentGroup, levelId])

  const orderedLevels = useMemo(() => {
    if (!currentGroup) return []
    return [...(currentGroup.levels || [])]
      .map(lv => {
        const n = parseInt(String(lv.id).match(/\d+/)?.[0] ?? '0', 10)
        return { ...lv, _num: n }
      })
      .sort((a, b) => (a._num - b._num) || String(a.id).localeCompare(String(b.id)))
  }, [currentGroup])  
  // 安全預設
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
  // useEffect 1：初次載入 groups
  // =====================================================
  useEffect(() => {
    async function loadGroups() {
      setLevelsMsg('Loading groups...')
      try {
        const res = await fetch(apiUrl('groups'))

        if (!res.ok) {
          const text = await res.text()
          setLevelsMsg(`Load groups failed (HTTP ${res.status}): ${text.slice(0, 120)}`)
          setGroups([])
          setGroupId('')
          setLevelId('')
          return
        }

        const data = await res.json()
        const gs = data.groups || []
        setGroups(gs)

        // ✅ 用同一套排序規則挑預設 group
        const rank = new Map(GROUP_ORDER.map((id, i) => [String(id), i]))
        const sorted = [...gs].sort((a, b) => {
          const ra = rank.has(String(a.groupId)) ? rank.get(String(a.groupId)) : 9999
          const rb = rank.has(String(b.groupId)) ? rank.get(String(b.groupId)) : 9999
          if (ra !== rb) return ra - rb
          return String(a.name ?? '').localeCompare(String(b.name ?? ''))
        })

        const g0 = sorted[0]
        const lv0 = g0?.levels?.[0]
        setGroupId(g0 ? String(g0.groupId) : '')
        setLevelId(lv0 ? String(lv0.id) : '')

        setLevelsMsg('')
      } catch (e) {
        setLevelsMsg('Load groups failed: ' + String(e))
        setGroups([])
        setGroupId('')
        setLevelId('')
      }
    }

    loadGroups()
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
  // useEffect 3：切換關卡（level）時清空棋盤
  // =====================================================
  useEffect(() => {
    if (!level) return
    setGrid(emptyGrid(level.width, level.height))
    setMsg('')
  }, [level, levelId])

  // =====================================================
  // 切換大關卡：同步切到該 group 的第一個 level
  // =====================================================
  function switchGroup(newGroupId) {
    const g = groups.find(x => String(x.groupId) === String(newGroupId))

    const sortedLevels = [...(g?.levels || [])].sort((a, b) => {
      const na = parseInt(String(a.id).match(/\d+/)?.[0] ?? '0', 10)
      const nb = parseInt(String(b.id).match(/\d+/)?.[0] ?? '0', 10)
      return (na - nb) || String(a.id).localeCompare(String(b.id))
    })

    const lv0 = sortedLevels[0]

    setGroupId(String(newGroupId))
    setLevelId(lv0 ? String(lv0.id) : '')
    setMsg('')
  }

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

  function clearBoard() {
    setGrid(emptyGrid(width, height))
    setMsg('')
  }

  // =====================================================
  // 顯示 Loading / No groups / Invalid
  // =====================================================
  if (levelsMsg) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>{levelsMsg}</div>
      </div>
    )
  }

  if (groups.length === 0) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>No groups</div>
      </div>
    )
  }

  if (!currentGroup) {
    return (
      <div style={{ padding: 16, fontFamily: 'sans-serif' }}>
        <h2>Puzzle Game</h2>
        <div>Invalid groupId: {String(groupId)}</div>
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

      <div style={{ display: 'flex', gap: 30, alignItems: 'flex-start' }}>
        {/* 最左側：大關卡按鈕 */}
        <div style={{ width: 180 }}>
          <h3 style={{ marginTop: 0 }}>Groups</h3>

          <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
            {orderedGroups.map(g => {
              const active = String(g.groupId) === String(groupId)
              return (
                <button
                  key={String(g.groupId)}
                  onClick={() => switchGroup(g.groupId)}
                  disabled={loading}
                  style={{
                    textAlign: 'left',
                    padding: '10px 12px',
                    borderRadius: 8,
                    border: '1px solid #ddd',
                    cursor: 'pointer',
                    color: '#111',
                    fontWeight: active ? 700 : 400,
                    background: active ? '#f5f5f5' : 'white',
                  }}
                >
                  {g.name ?? String(g.groupId)}
                </button>
              )
            })}
          </div>
        </div>

        {/* 中間：控制列 + 棋盤 */}
        <div>
          <div style={{ display: 'flex', gap: 12, alignItems: 'center', marginBottom: 12 }}>
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
            選擇大關卡/小關卡後按 Solve 後端會回傳解答並顯示在棋盤上
          </p>
        </div>

        {/* 右側：本關需要的 pieces */}
        <div style={{ width: 520 }}>
          <h3 style={{ marginTop: 0 }}>Pieces</h3>
          {piecesMsg && <div style={{ color: '#aaa', marginBottom: 8 }}>{piecesMsg}</div>}

          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(5, 1fr)', gap: 12 }}>
            {levelPieces.map(p => (
              <PiecePreview key={p.pieceId} pieceId={p.pieceId} cells={p.cells} />
            ))}
          </div>

          {/* ✅ Level 面板（現在會跟 Pieces 左對齊） */}
          <div
            style={{
              
              width: 355,
              background: 'rgba(20,20,20,0.9)',
              border: '1px solid rgba(255,255,255,0.12)',
              borderRadius: 12,
              padding: 12,
              position: 'absolute', top: '500px'
            }}
          >
            <div style={{ color: '#ddd', fontSize: 12, marginBottom: 8 }}>
              Level
            </div>

            <div
              style={{
                display: 'grid',
                gridTemplateColumns: 'repeat(10, 30px)',
                gap: 6,
              }}
            >
              {orderedLevels.map(lv => {
                const active = String(lv.id) === String(levelId)
                return (
                  <button
                    key={lv.id}
                    onClick={() => setLevelId(String(lv.id))}
                    disabled={loading}
                    style={{
                      width: 30,
                      height: 30,
                      borderRadius: 8,
                      border: active
                        ? '1px solid rgba(255,255,255,0.55)'
                        : '1px solid rgba(255,255,255,0.18)',
                      background: active
                        ? 'rgba(255,255,255,0.20)'
                        : 'rgba(255,255,255,0.08)',
                      color: '#fff',
                      fontSize: 12,
                      fontWeight: active ? 700 : 500,
                      cursor: 'pointer',
                      padding: 0,
                    }}
                  >
                    {lv._num}
                  </button>
                )
              })}
            </div>
          </div>
          
        </div>
      </div>
    </div>
  )
}