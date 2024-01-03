import { writable } from 'svelte/store';
import type { Position, Size, TextureData, Frame, TileData } from './types';

export const sidebar = writable<'inspector' | 'tiles' | 'tile' | 'rules' | null>(null);

export const tileSize = writable<Size>({ 
  width: 16,
  height: 16,
});
export const hoveredTile = writable<Position | null>(null);
export const selectedTiles = writable<Position[]>([]);
export const textureSource = writable<string>('');
export const textureFilename = writable<string | null>(null);
export const textureData = writable<TextureData | null>(null);
export const textureDataFilename = writable<string | null>(null);
export const textureFrames = writable<Frame[] | null>(null);
export const zoom = writable<number>(1);

export const tileData = writable<TileData | null>(null);
export const tileDataFilename = writable<string | null>(null);
