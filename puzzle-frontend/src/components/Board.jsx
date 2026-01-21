import './Board.css'

export default function Board({ width, height, grid }) {
  return (
    <div
      className="board"
      style={{
        gridTemplateColumns: `repeat(${width}, 36px)`,
        gridTemplateRows: `repeat(${height}, 36px)`,
      }}
    >
      {grid.map((v, idx) => {
        const x = idx % width
        const y = Math.floor(idx / width)
        const filled = v !== -1

        // pieceId 對應 class：piece-0, piece-1, piece-2...
        const pieceClass = filled ? `piece-${v}` : ''

        return (
          <div
            key={idx}
            className={`cell ${filled ? 'filled' : ''} ${pieceClass}`}
            title={`(${x}, ${y}) value=${v}`}
          >
            {''}
          </div>
        )
      })}
    </div>
  )
}
