import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  base: '/Puzzle-Game/',   // ★ 一定要是 repo 名
})