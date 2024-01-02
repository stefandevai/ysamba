<script lang="ts">
  import { selectedTiles, textureFrames } from './store';
  import type { Position, Frame } from './types';
  import { FrameTypes, TileTypes, AngleTypes } from './types';
  import { defaultFrame } from './frame';

  let currentFrame: Frame[] | null = [structuredClone(defaultFrame)];

  const getInspectorData = (tiles: Position[]) => {
    if (tiles.length === 0 || !$textureFrames) {
      currentFrame = null;
      return;
    }

    const index = tiles[0].index;
    const frame = $textureFrames[index];

    if (frame == null) {
      currentFrame = [structuredClone(defaultFrame)];
      return;
    }

    currentFrame = structuredClone(frame);
  }

  const handleSave = (frame: Frame) => {
    const newFrames = structuredClone($textureFrames);
    const selectedFrames = newFrames[frame.frame].filter((item) => item.key != frame.key);
    selectedFrames.push(frame);
    newFrames[frame.frame] = selectedFrames;
    textureFrames.set(newFrames);
  }

  const handleDelete = (frame: Frame) => {
    const newFrames = structuredClone($textureFrames);
    newFrames[frame.frame] = newFrames[frame.frame].filter((item) => item.key != frame.key);
    textureFrames.set(newFrames);
  }

  const handleAdd = (frameNumber: number) => {
    const newFrames = structuredClone($textureFrames);
    newFrames[frameNumber].push({
      ...defaultFrame,
      frame: frameNumber,
      key: newFrames[frameNumber].length,
    });
    textureFrames.set(newFrames);
  }

  const handleTileTypeChange = (event, key: number) => {
    const item = currentFrame.find((frame) => frame.key == key);

    if (!item) {
      return;
    }

    if (event.target.value == 'multiple') {
      item.tile_type = event.target.value;
      item.width = 1;
      item.height = 1;
      item.pattern = [];
      item.pattern_width = 0;
      item.pattern_height = 0;
      item.anchor_x = 0;
      item.anchor_y = 0;
      currentFrame = currentFrame.filter((frame) => frame.key != item.key);
      currentFrame.push(item);
    }
    else if (event.target.value == 'single') {
      item.tile_type = event.target.value;
      delete item.width;
      delete item.height;
      delete item.pattern;
      delete item.pattern_width;
      delete item.pattern_height;
      delete item.anchor_x;
      delete item.anchor_y;
      currentFrame = currentFrame.filter((frame) => frame.key != item.key);
      currentFrame.push(item);
    }
  }

  textureFrames.subscribe(() => {
    getInspectorData($selectedTiles);
  });

  selectedTiles.subscribe(() => {
    getInspectorData($selectedTiles);
  });
</script>

{#if currentFrame && $selectedTiles.length > 0}
  <div>
    {#each currentFrame as item}
      <div>
        <label>Frame:</label>
        {item.frame}
        <label>ID:</label>
        <input id="id" type="number" bind:value={item.id} />
        <label for="type">Type:</label>
        <select bind:value={item.type}>
          {#each FrameTypes as option}
            <option value={option}>
              {option}
            </option>
          {/each}
        </select>

        <label for="tile_type">Tile Type:</label>
        <select on:change={(event) => handleTileTypeChange(event, item.key)}>
          {#each TileTypes as option}
            <option value={option}>
              {option}
            </option>
          {/each}
        </select>

        <label for="angle">Angle:</label>
        <select bind:value={item.angle}>
          {#each AngleTypes as option}
            <option value={option}>
              {option}
            </option>
          {/each}
        </select>

        {#if item.front_face_id != null}
          <label for="front_face_id">Front Face ID:</label>
          <input id="front_face_id" type="number" bind:value={item.front_face_id} />
        {/if}
        
        {#if item.tile_type == 'multiple'}
          <label>Size:</label>
          <input type="number" bind:value={item.width} />
          <input type="number" bind:value={item.height} />
          <label>Pattern:</label>
          {item.pattern.toString()}
          <label>Pattern Size:</label>
          <input type="number" bind:value={item.pattern_width} />
          <input type="number" bind:value={item.pattern_height} />
          <label>Anchor Size:</label>
          <input type="number" bind:value={item.anchor_x} />
          <input type="number" bind:value={item.anchor_y} />
        {/if}

        <button on:click={() => handleSave(item)}>Save</button>
        <button on:click={() => handleDelete(item)}>Delete</button>
      </div>
    {/each}
    <button on:click={() => handleAdd($selectedTiles[0].index)}>Add</button>
  </div>
{/if}

<style>
  label {
    display: block;
  }
</style>
