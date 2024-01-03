import { writable } from 'svelte/store';
import type { Position, Size, TextureData, Frame } from './types';

export const tileSize = writable<Size>({ 
  width: 16,
  height: 16,
});
export const hoveredTile = writable<Position | null>(null);
export const selectedTiles = writable<Position[]>([]);
export const textureSource = writable<string>('');
export const textureData = writable<TextureData | null>(null);
export const textureFrames = writable<Frame[] | null>(null);
export const zoom = writable<number>(1);

