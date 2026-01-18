import { useMemo } from 'react'
import Board from './components/Board'

export default function App() {
  const width = 5
  const height = 5

  // 假資料：先填出幾個格子測試（-1 表示空）
  const grid = useMemo(() => {
    const g = Array(width * height).fill(-1)
    // 隨便填一些 pieceId
    g[0] = 0
    g[1] = 0
    g[5] = 1
    g[6] = 1
    g[12] = 2
    g[18] = 3
    return g
  }, [])

  return (
    <div style={{ padding: 16, fontFamily: 'sans-serif', justifyContent: 'center', width: '100vw' }}>
      <h2 style = {{ textAlign: 'center' }}>Katamino</h2>
      <div style={{ padding: 10, justifyContent: 'center', display: 'flex' }}>
        <Board width={width} height={height} grid={grid} />
      </div>
      <p style={{ color: '#666' }}>
        -1 代表空格
      </p>
      <p style={{ color: '#666' }}>
        數字代表 pieceId
      </p>
    </div>
  )
}
