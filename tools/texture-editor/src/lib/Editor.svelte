<script lang="ts">
  import { onMount } from 'svelte';
  import type { Position, DrawParams } from './types';

  let canvas;
  let tileWidth = 16;
  let tileHeight = 16;
  let textureSource = '';
  let hoveredTile: Position | null = null;
  let selectedTiles: Position[] | null = null;

  const readTextureJSONData = (file: File) => {
    const reader = new FileReader();

    return new Promise((resolve, reject) => {
      reader.onload = () => resolve(JSON.parse(reader.result));
      reader.onerror = reject;
      reader.readAsText(file);
    });
  };

  const readTextureData = (file: File) => {
    const reader = new FileReader();

    return new Promise((resolve, reject) => {
      reader.onload = () => resolve(reader.result);
      reader.onerror = reject;
      reader.readAsDataURL(file);
    });
  };

  const fitCanvasToTexture = (textureData: string) => {
    const texture = new Image();
    texture.src = textureData;
    texture.onload = () => {
      canvas.width = texture.naturalWidth;
      canvas.height = texture.naturalHeight;
    };
  };

  const onTextureLoad = async (event) => {
    if (!event.target.files || event.target.files.length !== 1) {
      return;
    }

    const file = event.target.files[0];
    textureSource = await readTextureData(file);
    fitCanvasToTexture(textureSource);
  };

  const onDataLoad = async (event) => {
    if (!event.target.files || event.target.files.length !== 1) {
      return;
    }

    const file = event.target.files[0];
    const textureJSONData = await readTextureJSONData(file);

    console.log(textureJSONData);
  };

  const drawGrid = (params: DrawParams) => {
    if (params.cellWidth <= 0 || params.cellHeight <= 0 || isNaN(params.cellWidth) || isNaN(params.cellHeight)) {
      return;
    }

    const context = canvas.getContext('2d');

    context.clearRect(0, 0, canvas.width, canvas.height);

    if (params.hoveredCell != null) {
      context.fillStyle = "rgba(96, 200, 235, 0.5)";
      context.fillRect(
        params.hoveredCell.x * params.cellWidth,
        params.hoveredCell.y * params.cellHeight,
        params.cellWidth,
        params.cellHeight,
      );
    }

    if (params.selectedCells != null) {
      for (const cell of params.selectedCells) {
        context.fillStyle = "rgba(222, 85, 80, 0.5)";
        context.fillRect(
          cell.x * params.cellWidth,
          cell.y * params.cellHeight,
          params.cellWidth,
          params.cellHeight,
        );
      }
    }

    context.beginPath();
    context.lineWidth = 1;
    context.strokeStyle = "rgba(80, 80, 80, 0.6)";

    for (let x = 0; x < canvas.width; x += params.cellWidth) {
      context.moveTo(x, 0);
      context.lineTo(x, canvas.height);
    }

    for (let y = 0; y < canvas.height; y += params.cellHeight) {
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
      hoveredTile = null;
      return;
    }

    const widthInTiles = Math.ceil(width / tileWidth);
    const tileX = Math.floor(x / tileWidth);
    const tileY = Math.floor(y / tileWidth);

    hoveredTile = {
      index: tileX + tileY * widthInTiles,
      x: tileX,
      y: tileY,
    };
  };

  const handleGridLeave = () => {
    if (hoveredTile != null) {
      hoveredTile = null;
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

    const widthInTiles = Math.ceil(width / tileWidth);
    const tileX = Math.floor(x / tileWidth);
    const tileY = Math.floor(y / tileWidth);
    const tileIndex = tileX + tileY * widthInTiles;

    const found = selectedTiles?.find((tile) => tile.index == tileIndex);

    if (found != null) {
      selectedTiles = null;
    }
    else {
      selectedTiles = [{
        index: tileX + tileY * widthInTiles,
        x: tileX,
        y: tileY,
      }];
    }
  };

  $: canvas && drawGrid({
    cellWidth: tileWidth,
    cellHeight: tileHeight,
    hoveredCell: hoveredTile,
    selectedCells: selectedTiles,
  });
</script>

<div>
  <input type="number" bind:value={tileWidth} />
  <input type="number" bind:value={tileHeight} />
  <input type="file" accept="image/*" on:change={onTextureLoad} />
  <input type="file" accept="application/JSON" on:change={onDataLoad} />
</div>

{#if textureSource !== ''}
  <div class="wrapper">
    <canvas
      bind:this={canvas}
      on:mousemove={handleGridHover}
      on:mouseleave={handleGridLeave}
      on:mouseup={handleGridClick}
    />
    <img src={textureSource} alt="" />
  </div>
{/if}


<style>
  .wrapper {
    position: relative;
  }
  canvas {
    position: absolute;
    border: 1px solid rgba(80, 80, 80, 0.6);
    image-rendering: pixelated;
    cursor: pointer;
    z-index: 2;
  }
  img {
    position: absolute;
    z-index: 1;
  }

</style>
