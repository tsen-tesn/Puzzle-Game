import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig(({ mode }) => ({
  plugins: [react()],
  base: mode === 'production' ? '/Puzzle-Game/' : '/',
  server: {
    proxy: {
      '/solve': 'http://127.0.0.1:8080',
      '/health': 'http://127.0.0.1:8080',
      '/pieces': 'http://127.0.0.1:8080',
    },
  },
}))
