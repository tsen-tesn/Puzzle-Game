import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig(({ command }) => ({
  plugins: [react()],
  base: command === 'build' ? '/Puzzle-Game/' : '/',

  server: {
    host: true,          // 等同 0.0.0.0 讓 Windows 也能連到 WSL
    port: 5173,
    strictPort: true,
    proxy: {
      '/solve':   { target: 'http://127.0.0.1:8080', changeOrigin: true },
      '/pieces':  { target: 'http://127.0.0.1:8080', changeOrigin: true },
      '/health':  { target: 'http://127.0.0.1:8080', changeOrigin: true },
      '/levels':  { target: 'http://127.0.0.1:8080', changeOrigin: true },
      '/groups':  { target: 'http://127.0.0.1:8080', changeOrigin: true },
    },
  },
}))