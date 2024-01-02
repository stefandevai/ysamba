export type Position = {
  index: number;
  x: number;
  y: number;
};

export type Size = {
  width: number;
  height: number;
};

export type DrawParams = {
  tileSize: Size;
  hoveredCell: Position | null;
  selectedCells: Position[] | null;
};
