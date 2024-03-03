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
  game_id: number;
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
  FOUR_SIDED_TERRAIN = 0,
  RANDOM,
}

export type Rule = {
  name: string;
  id: number;
  type: RuleType;
};

export enum FourSide {
  NONE = 0b0000,
  TOP = 0b0001,
  RIGHT = 0b0010,
  BOTTOM = 0b0100,
  LEFT = 0b1000,
}

export enum EightSide {
  NONE = 0b00000000,
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

export type RuleFourSidedTerrain = Rule & {
  type: RuleType.FOUR_SIDED_TERRAIN;
  tile0: SideBitmask;
  tile1: SideBitmask;
  tile2: SideBitmask;
  tile3: SideBitmask;
  tile4: SideBitmask;
  tile5: SideBitmask;
  tile6: SideBitmask;
  tile7: SideBitmask;
  tile8: SideBitmask;
  tile9: SideBitmask;
  tile10: SideBitmask;
  tile11: SideBitmask;
  tile12: SideBitmask;
  tile13: SideBitmask;
  tile14: SideBitmask;
  tile15: SideBitmask;
};

export const createEightSidedTerrainRule = (name: string, id: number): RuleEightSidedTerrain => {
  return {
    name,
    id,
    type: RuleType.EIGHT_SIDED_TERRAIN,
    tile1: {
      sides: EightSide.TOP_LEFT,
      id: -1,
    },
    tile3: {
      sides: EightSide.TOP_LEFT | EightSide.TOP,
      id: -1,
    },
  };
}

export const createFourSidedTerrainRule = (name: string, id: number): RuleFourSidedTerrain => {
  return {
    name,
    id,
    type: RuleType.FOUR_SIDED_TERRAIN,
    tile0: {
      sides: FourSide.TOP | FourSide.RIGHT | FourSide.BOTTOM | FourSide.LEFT,
      id: -1,
    },
    tile1: {
      sides: FourSide.RIGHT | FourSide.BOTTOM | FourSide.LEFT,
      id: -1,
    },
    tile2: {
      sides: FourSide.TOP | FourSide.RIGHT | FourSide.BOTTOM,
      id: -1,
    },
    tile3: {
      sides: FourSide.RIGHT | FourSide.BOTTOM,
      id: -1,
    },
    tile4: {
      sides: FourSide.TOP | FourSide.BOTTOM | FourSide.LEFT,
      id: -1,
    },
    tile5: {
      sides: FourSide.BOTTOM | FourSide.LEFT,
      id: -1,
    },
    tile6: {
      sides: FourSide.TOP | FourSide.BOTTOM,
      id: -1,
    },
    tile7: {
      sides: FourSide.BOTTOM,
      id: -1,
    },
    tile8: {
      sides: FourSide.TOP | FourSide.RIGHT | FourSide.LEFT,
      id: -1,
    },
    tile9: {
      sides: FourSide.RIGHT | FourSide.LEFT,
      id: -1,
    },
    tile10: {
      sides: FourSide.TOP | FourSide.RIGHT,
      id: -1,
    },
    tile11: {
      sides: FourSide.RIGHT,
      id: -1,
    },
    tile12: {
      sides: FourSide.TOP | FourSide.LEFT,
      id: -1,
    },
    tile13: {
      sides: FourSide.LEFT,
      id: -1,
    },
    tile14: {
      sides: FourSide.TOP,
      id: -1,
    },
    tile15: {
      sides: FourSide.NONE,
      id: -1,
    },
  };
}

export enum GridMode {
  NORMAL = 'normal',
  EIGHT_SIDED_BITMASK_SELECTION = 'eight-sided-bitmask-selection',
  FOUR_SIDED_BITMASK_SELECTION = 'eight-sided-bitmask-selection',
};

export type DrawParams = {
  mode: GridMode;
  cellSize: Size;
  hoveredCell: Position | null;
  selectedCells: Position[] | null;
  gridZoom: number;
  bitmask: number | null;
};

