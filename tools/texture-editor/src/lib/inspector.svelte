<script lang="ts">
  import { selectedTiles, textureFrames } from './store';
  import type { Position, Frame } from './types';
  import { FrameTypes, TileTypes, AngleTypes } from './types';
  import { defaultFrame } from './frame';

  let currentFrame: Frame | null = defaultFrame;

  const getInspectorData = (tiles: Position[]) => {
    if (tiles.length === 0 || !$textureFrames) {
      currentFrame = null;
      return;
    }

    const index = tiles[0].index;
    const frame = $textureFrames[index];

    if (frame == null) {
      textureFrames.update((oldFrames) => [
        ...oldFrames.slice(0, index),
        { ...defaultFrame },
        ...oldFrames.slice(index + 1),
      ]);

      currentFrame = { ...defaultFrame };
      return;
    }

    currentFrame = { ...frame };
  }

  $: getInspectorData($selectedTiles);
</script>

{#if currentFrame}
  <div>
    <label>Frame:</label>
    {currentFrame.frame}
    <label for="type">Type:</label>
    <select bind:value={currentFrame.type}>
      {#each FrameTypes as option}
        <option value={option}>
          {option}
        </option>
      {/each}
    </select>

    <label for="tile_type">Tile Type:</label>
    <select bind:value={currentFrame.tile_type}>
      {#each TileTypes as option}
        <option value={option}>
          {option}
        </option>
      {/each}
    </select>

    <label for="angle">Angle:</label>
    <select bind:value={currentFrame.angle}>
      {#each AngleTypes as option}
        <option value={option}>
          {option}
        </option>
      {/each}
    </select>

    {#if currentFrame.front_face_id}
      <label for="front_face_id">Front Face ID:</label>
      <input id="front_face_id" type="number" bind:value={currentFrame.front_face_id} />
    {/if}
    
    {#if currentFrame.tile_type == 'multiple'}
      <label>Size:</label>
      <input type="number" bind:value={currentFrame.width} />
      <input type="number" bind:value={currentFrame.height} />
      <label>Pattern:</label>
      {currentFrame.pattern.toString()}
      <label>Pattern Size:</label>
      <input type="number" bind:value={currentFrame.pattern_width} />
      <input type="number" bind:value={currentFrame.pattern_height} />
      <label>Anchor Size:</label>
      <input type="number" bind:value={currentFrame.anchor_x} />
      <input type="number" bind:value={currentFrame.anchor_y} />
    {/if}
  </div>
{/if}

<style>
  label {
    display: block;
  }
</style>
