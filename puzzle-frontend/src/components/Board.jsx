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

        return (
          <div
            key={idx}
            className={`cell ${filled ? 'filled' : ''}`}
            title={`(${x}, ${y}) value=${v}`}
          >
            {filled ? v : ''}
          </div>
        )
      })}
    </div>
  )
}
