<script lang="ts">
  import Tooltip from '../../common/tooltip.svelte';
  import { textureSource, hoveredTile, selectedTiles, tileSize, zoom, sidebar } from './store';
  import type { DrawParams, Size } from './types';

  let canvas: HTMLCanvasElement;
  let originalSize: Size = {
    width: 0,
    height: 0,
  };

  const fitCanvasToTexture = (textureData: string) => {
    const texture = new Image();
    texture.src = textureData;
    texture.onload = () => {
      originalSize.width = texture.naturalWidth;
      originalSize.height = texture.naturalHeight;

      if (!canvas) {
        return;
      }

      canvas.width = texture.naturalWidth;
      canvas.height = texture.naturalHeight;
    };
  };

  textureSource.subscribe((value) => {
    fitCanvasToTexture(value);
  });

  const drawGrid = (params: DrawParams) => {
    if (params.cellSize.width <= 0 || params.cellSize.height <= 0 || isNaN(params.cellSize.width) || isNaN(params.cellSize.height) || !canvas) {
      return;
    }

    const context = canvas.getContext('2d');

    context.clearRect(0, 0, canvas.width, canvas.height);

    if (params.hoveredCell != null) {
      context.fillStyle = "rgba(96, 200, 235, 0.5)";
      context.fillRect(
        params.hoveredCell.x * params.cellSize.width * $zoom,
        params.hoveredCell.y * params.cellSize.height * $zoom,
        params.cellSize.width * $zoom,
        params.cellSize.height * $zoom,
      );
    }

    if (params.selectedCells != null) {
      for (const cell of params.selectedCells) {
        context.fillStyle = "rgba(222, 85, 80, 0.5)";
        context.fillRect(
          cell.x * params.cellSize.width * $zoom,
          cell.y * params.cellSize.height * $zoom,
          params.cellSize.width * $zoom,
          params.cellSize.height * $zoom,
        );
      }
    }

    context.beginPath();
    context.lineWidth = 1;
    context.strokeStyle = "rgba(200, 200, 200, 0.5)";

    for (let x = 0; x < canvas.width; x += params.cellSize.width * $zoom) {
      context.moveTo(x, 0);
      context.lineTo(x, canvas.height);
    }

    for (let y = 0; y < canvas.height; y += params.cellSize.height * $zoom) {
      context.moveTo(0, y);
      context.lineTo(canvas.width, y);
    }

    context.stroke();
  };

  const handleGridHover = (event: MouseEvent) => {
    const x = event.offsetX;
    const y = event.offsetY;
    const width = canvas.width;
    const height = canvas.height;

    if (x < 0 || y < 0 || x >= width || y >= height) {
      hoveredTile.set(null);
      return;
    }

    const widthInTiles = Math.ceil(width / ($tileSize.width * $zoom));
    const tileX = Math.floor(x / ($tileSize.width * $zoom));
    const tileY = Math.floor(y / ($tileSize.height * $zoom));
    const tileIndex = tileX + tileY * widthInTiles;

    if ($hoveredTile != null && $hoveredTile.index != tileIndex) {
      hoveredTile.set({
        index: tileIndex,
        x: tileX,
        y: tileY,
      });
    }
    else if ($hoveredTile == null) {
      hoveredTile.set({
        index: tileIndex,
        x: tileX,
        y: tileY,
      });
    }
  };

  const handleGridLeave = () => {
    if ($hoveredTile != null) {
      hoveredTile.set(null);
    }
  };

  const handleGridClick = (event: MouseEvent) => {
    const x = event.offsetX;
    const y = event.offsetY;
    const width = canvas.width;
    const height = canvas.height;

    if (x < 0 || y < 0 || x >= width || y >= height) {
      return;
    }

    const widthInTiles = Math.ceil(width / ($tileSize.width * $zoom));
    const tileX = Math.floor(x / ($tileSize.width * $zoom));
    const tileY = Math.floor(y / ($tileSize.height * $zoom));
    const tileIndex = tileX + tileY * widthInTiles;

    const found = $selectedTiles.find((tile) => tile.index == tileIndex);

    if (found != null && $selectedTiles.length > 0) {
      sidebar.set(null);
      selectedTiles.set([]);
    }
    else if (found == null) {
      sidebar.set('inspector');
      selectedTiles.set([{
        index: tileIndex,
        x: tileX,
        y: tileY,
      }]);
    }
  };

  const applyZoom = (zoomLevel: number) => {
    canvas.width = originalSize.width * zoomLevel;
    canvas.height = originalSize.height * zoomLevel;
  };

  $: canvas && applyZoom($zoom)

  $: canvas && drawGrid({
    cellSize: $tileSize,
    hoveredCell: $hoveredTile,
    selectedCells: $selectedTiles,
  });

  $: tooltipValue = $hoveredTile ? $hoveredTile.index.toString() : undefined;
</script>

{#if $textureSource !== ''}
  <Tooltip title={tooltipValue} class="frame-tooltip">
    <div class="grid-wrapper" >
      <canvas
        bind:this={canvas}
        on:mousemove={handleGridHover}
        on:mouseleave={handleGridLeave}
        on:mouseup={handleGridClick}
      />
      <img src={$textureSource} alt="" style="height: {canvas?.height ?? 0}px; width: {canvas?.width ?? 0}px; transform-origin: left top; transform: scale({1});" />
    </div>
  </Tooltip>
{/if}


<style>
  :global(.frame-tooltip) {
    flex: 1;
  }

  .grid-wrapper {
    position: relative;
    width: 100%;
    height: 100%;
    overflow: scroll;
    margin-right: 2rem;
  }

  canvas {
    position: absolute;
    border: 1px solid var(--color-light-gray);
    image-rendering: pixelated;
    z-index: 2;
    cursor: pointer;
  }

  img {
    image-rendering: pixelated;
    max-width: unset;
    position: absolute;
    z-index: 1;
    background-color: white;
    cursor: pointer;
  }
</style>

