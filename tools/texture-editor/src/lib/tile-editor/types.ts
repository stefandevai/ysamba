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
export const TileTypes = ['single', 'multiple'] as const;
export const AngleTypes = ['orthogonal', 'parallel'] as const;

type AnyFrame = {
  key: number;
  frame: number;
  id: number;
  type: typeof FrameTypes[number];
  tile_type: typeof TileTypes[number];
  angle: typeof AngleTypes[number];
  front_face_id?: number;
};

type SingleFrame = AnyFrame & {
  tile_type: 'single';
};

type MultipleFrame = AnyFrame & {
  tile_type: 'multiple';
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

export type Drop = {
  item_id: number;
  quantity: [number, number];
};

export type Action = {
  name: string;
  type: number;
  qualities_required?: string[];
  turns_into?: number;
  gives?: Drop[];
  gives_in_place?: boolean;
};

export type Tile = {
  id: number;
  name: string;
  flags?: string[];
  actions?: Action[];
  drop_ids?: number[];
  climbs_to?: number[];
};

export type TileData = {
  tiles: Tile[];
};

export enum RuleType {
  EIGHT_SIDED_TERRAIN = 0,
  RANDOM,
}

export type Rule = {
  name: string;
  id: number;
  type: RuleType;
};

export enum Side {
  TOP_LEFT = 0b00000001,
  TOP = 0b00000010,
  TOP_RIGHT = 0b00000100,
  RIGHT = 0b00001000,
  BOTTOM_RIGHT = 0b00010000,
  BOTTOM = 0b00100000,
  BOTTOM_LEFT = 0b01000000,
  LEFT = 0b10000000,
}

type SideBitmask = {
  sides: number;
  id: number;
};

export type RuleEightSidedTerrain = Rule & {
  type: RuleType.EIGHT_SIDED_TERRAIN;
  tile1: SideBitmask;
  tile3: SideBitmask;
};

export const createEightSidedTerrainRule = (name: string, id: number): RuleEightSidedTerrain => {
  return {
    name,
    id,
    type: RuleType.EIGHT_SIDED_TERRAIN,
    tile1: {
      sides: Side.TOP_LEFT,
      id: -1,
    },
    tile3: {
      sides: Side.TOP_LEFT | Side.TOP,
      id: -1,
    },
  };
}

export enum GridMode {
  NORMAL = 'normal',
  EIGHT_SIDED_BITMASK_SELECTION = 'eight-sided-bitmask-selection',
};

export type DrawParams = {
  mode: GridMode;
  cellSize: Size;
  hoveredCell: Position | null;
  selectedCells: Position[] | null;
  gridZoom: number;
  bitmask: number | null;
};

