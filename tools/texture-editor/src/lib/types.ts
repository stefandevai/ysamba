export type Position = {
  index: number;
  x: number;
  y: number;
};

export type DrawParams = {
  tileWidth: number;
  tileHeight: number;
  hoveredCell: Position | null;
  selectedCells: Position[] | null;
};
