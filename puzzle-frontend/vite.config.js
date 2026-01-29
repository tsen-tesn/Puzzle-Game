import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  base: '/Puzzle-Game/',  // set for GitHub Pages
  server: {
    proxy: {
      '/solve': {
        target: 'http://127.0.0.1:8080',
        changeOrigin: true,
      },
      '/pieces': {
        target: 'http://127.0.0.1:8080',
        changeOrigin: true,
      },
      '/health': {
        target: 'http://127.0.0.1:8080',
        changeOrigin: true,
      },
    },
  },
})
