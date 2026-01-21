import './PiecePreview.css'

export default function PiecePreview({ pieceId, cells, cellSize = 16 }) {
  let maxX = 0, maxY = 0
  for (const c of cells) {
    if (c.x > maxX) maxX = c.x
    if (c.y > maxY) maxY = c.y
  }
  const w = maxX + 1
  const h = maxY + 1

  const filled = new Set(cells.map(c => `${c.x},${c.y}`))

  return (
    <div className="pieceCard">
      <div className="pieceTitle">Piece {pieceId + 1}</div>
      <div
        className="pieceGrid"
        style={{
          gridTemplateColumns: `repeat(${w}, ${cellSize}px)`,
          gridTemplateRows: `repeat(${h}, ${cellSize}px)`,
        }}
      >
        {Array.from({ length: w * h }, (_, i) => {
          const x = i % w
          const y = Math.floor(i / w)
          const on = filled.has(`${x},${y}`)
          return (
            <div
              key={i}
              className={`pCell ${on ? `piece-${pieceId}` : 'empty'}`}
              style={{ width: cellSize, height: cellSize }}
            />
          )
        })}
      </div>
    </div>
  )
}
