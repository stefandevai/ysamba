export type Optional<T> = T | undefined;

export type Position = {
  index: number;
  x: number;
  y: number;
};

export type Size = {
  width: number;
  height: number;
};

export const FrameTypes = ['tile', 'item'] as const;
export const SpriteTypes = ['single', 'multiple'] as const;
export const AngleTypes = ['orthogonal', 'parallel'] as const;

type AnyFrame = {
  key: number;
  frame: number;
  id: number;
  type: typeof FrameTypes[number];
  sprite_type: typeof SpriteTypes[number];
  angle: typeof AngleTypes[number];
  front_face_id?: number;
};

type SingleFrame = AnyFrame & {
  sprite_type: 'single';
};

type MultipleFrame = AnyFrame & {
  sprite_type: 'multiple';
  width: number;
  height: number;
  pattern: number[];
  pattern_width: number;
  pattern_height: number;
  anchor_x: number;
  anchor_y: number;
};

export type Frame = SingleFrame | MultipleFrame;

export type FrameArray = Array<Frame[]>;

export type TextureData = {
  name: string;
  filepath: string;
  width: number;
  height: number;
  tile_width: number;
  tile_height: number;
  frames: Frame[];
};

export type DrawParams = {
  cellSize: Size;
  hoveredCell: Position | null;
  selectedCells: Position[] | null;
};
