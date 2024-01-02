<script lang="ts">
  import Tooltip from './tooltip.svelte';
  import { textureSource, hoveredTile, selectedTiles, tileSize } from './store';
  import type { DrawParams } from './types';

  let canvas;

  const fitCanvasToTexture = (textureData: string) => {
    const texture = new Image();
    texture.src = textureData;
    texture.onload = () => {
      canvas.width = texture.naturalWidth;
      canvas.height = texture.naturalHeight;
    };
  };

  textureSource.subscribe((value) => {
    fitCanvasToTexture(value);
  });

  const drawGrid = (params: DrawParams) => {
    if (params.cellSize.width <= 0 || params.cellSize.height <= 0 || isNaN(params.cellSize.width) || isNaN(params.cellSize.height)) {
      return;
    }

    const context = canvas.getContext('2d');

    context.clearRect(0, 0, canvas.width, canvas.height);

    if (params.hoveredCell != null) {
      context.fillStyle = "rgba(96, 200, 235, 0.5)";
      context.fillRect(
        params.hoveredCell.x * params.cellSize.width,
        params.hoveredCell.y * params.cellSize.height,
        params.cellSize.width,
        params.cellSize.height,
      );
    }

    if (params.selectedCells != null) {
      for (const cell of params.selectedCells) {
        context.fillStyle = "rgba(222, 85, 80, 0.5)";
        context.fillRect(
          cell.x * params.cellSize.width,
          cell.y * params.cellSize.height,
          params.cellSize.width,
          params.cellSize.height,
        );
      }
    }

    context.beginPath();
    context.lineWidth = 1;
    context.strokeStyle = "rgba(200, 200, 200, 0.5)";

    for (let x = 0; x < canvas.width; x += params.cellSize.width) {
      context.moveTo(x, 0);
      context.lineTo(x, canvas.height);
    }

    for (let y = 0; y < canvas.height; y += params.cellSize.height) {
      context.moveTo(0, y);
      context.lineTo(canvas.width, y);
    }

    context.stroke();
  };

  const handleGridHover = (event) => {
    const x = event.offsetX;
    const y = event.offsetY;
    const width = canvas.width;
    const height = canvas.height;

    if (x < 0 || y < 0 || x >= width || y >= height) {
      hoveredTile.set(null);
      return;
    }

    const widthInTiles = Math.ceil(width / $tileSize.width);
    const tileX = Math.floor(x / $tileSize.width);
    const tileY = Math.floor(y / $tileSize.height);

    hoveredTile.set({
      index: tileX + tileY * widthInTiles,
      x: tileX,
      y: tileY,
    });
  };

  const handleGridLeave = () => {
    if ($hoveredTile != null) {
      hoveredTile.set(null);
    }
  };

  const handleGridClick = (event) => {
    const x = event.offsetX;
    const y = event.offsetY;
    const width = canvas.width;
    const height = canvas.height;

    if (x < 0 || y < 0 || x >= width || y >= height) {
      return;
    }

    const widthInTiles = Math.ceil(width / $tileSize.width);
    const tileX = Math.floor(x / $tileSize.width);
    const tileY = Math.floor(y / $tileSize.height);
    const tileIndex = tileX + tileY * widthInTiles;

    const found = $selectedTiles.find((tile) => tile.index == tileIndex);

    if (found != null) {
      selectedTiles.set([]);
    }
    else {
      selectedTiles.set([{
        index: tileX + tileY * widthInTiles,
        x: tileX,
        y: tileY,
      }]);
    }
  };

  $: canvas && drawGrid({
    cellSize: $tileSize,
    hoveredCell: $hoveredTile,
    selectedCells: $selectedTiles,
  });

  $: tooltipValue = $hoveredTile ? $hoveredTile.index.toString() : undefined;
</script>

{#if $textureSource !== ''}
  <Tooltip title={tooltipValue} class="frame-tooltip">
    <div class="wrapper" >
      <canvas
        bind:this={canvas}
        on:mousemove={handleGridHover}
        on:mouseleave={handleGridLeave}
        on:mouseup={handleGridClick}
      />
      <img src={$textureSource} alt="" style="height: {canvas?.height ?? 0}px; width: {canvas?.width ?? 0}px;" />
    </div>
  </Tooltip>
{/if}


<style>
  :global(.frame-tooltip) {
    flex: 1;
  }

  .wrapper {
    position: relative;
    width: 100%;
    height: 100%;
    overflow: scroll;
    margin-right: 2rem;
    background-color: var(--color-light-gray);
  }

  canvas {
    position: absolute;
    border: 1px solid var(--color-light-gray);
    image-rendering: pixelated;
    cursor: pointer;
    z-index: 2;
  }

  img {
    max-width: unset;
    position: absolute;
    z-index: 1;
    background-color: white;
  }
</style>

