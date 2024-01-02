import { writable } from 'svelte/store';
  import type { Position } from './types';

export const tileSize = writable<Size>({ 
  width: 16,
  height: 16,
});
export const hoveredTile = writable<Position | null>(null);
export const selectedTiles = writable<Position[]>([]);
export const textureSource = writable<string>('');
