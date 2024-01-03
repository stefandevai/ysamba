<script lang="ts">
  import Accordion from '../../common/accordion.svelte';
  import SidebarBase from '../../common/sidebar-base.svelte';
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
    const newFrames: Array<Array<Frame>> = structuredClone($textureFrames);
    const selectedFrames = newFrames[frame.frame].filter((item) => item.key != frame.key);
    selectedFrames.push(frame);
    newFrames[frame.frame] = selectedFrames;
    textureFrames.set(newFrames);
  }

  const handleDelete = (frame: Frame) => {
    const newFrames: Array<Array<Frame>> = structuredClone($textureFrames);
    newFrames[frame.frame] = newFrames[frame.frame].filter((item) => item.key != frame.key);
    textureFrames.set(newFrames);
  }

  const handleAdd = (frameNumber: number) => {
    const newFrames: Array<Array<Frame>> = structuredClone($textureFrames);
    newFrames[frameNumber].push({
      ...defaultFrame,
      frame: frameNumber,
      key: newFrames[frameNumber].length,
    });
    textureFrames.set(newFrames);
  }

  const handleTileTypeChange = (event: Event, key: number) => {
    const item = currentFrame.find((frame) => frame.key == key);

    if (!item) {
      return;
    }

    const target = (event.target as HTMLSelectElement)

    if (target.value == 'multiple') {
      item.tile_type = target.value;
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
    else if (target.value == 'single') {
      item.tile_type = target.value;
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
  <SidebarBase title={`Frame ${$selectedTiles[0].index}`}>
    {#each currentFrame as item, index}
      {#key item.frame}
        <Accordion title="Game ID: {item.id}" open={index === 0}>
          <p>Game ID:</p>
          <input class="input" id="id" type="number" bind:value={item.id} />
          <p>Type:</p>
          <div class="select input-group">
            <select bind:value={item.type}>
              <option value="" disabled selected>- Select -</option>
              {#each FrameTypes as option}
                <option value={option}>
                  {option}
                </option>
              {/each}
            </select>
          </div>

          <p>Tile Type:</p>
          <div class="select input-group">
            <select bind:value={item.tile_type} on:change={(event) => handleTileTypeChange(event, item.key)}>
              <option value="" disabled selected>- Select -</option>
              {#each TileTypes as option}
                <option value={option}>
                  {option}
                </option>
              {/each}
            </select>
          </div>

          <p>Angle:</p>
          <div class="select input-group">
            <select bind:value={item.angle}>
              <option value="" disabled selected>- Select -</option>
              {#each AngleTypes as option}
                <option value={option}>
                  {option}
                </option>
              {/each}
            </select>
          </div>

          <p>Front Face ID:</p>
          <input class="input" id="front_face_id" type="number" placeholder="Game ID" bind:value={item.front_face_id} />
          
          {#if item.tile_type == 'multiple'}
            <p>Size:</p>
            <div class="input-group">
              <input class="input" type="number" bind:value={item.width} />
              <input class="input" type="number" bind:value={item.height} />
            </div>
            <p>Pattern:</p>
            {item.pattern.toString()}
            <p>Pattern Size:</p>
            <div class="input-group">
              <input class="input" type="number" bind:value={item.pattern_width} />
              <input class="input" type="number" bind:value={item.pattern_height} />
            </div>
            <p>Anchor Size:</p>
            <div class="input-group">
              <input class="input" type="number" bind:value={item.anchor_x} />
              <input class="input" type="number" bind:value={item.anchor_y} />
            </div>
          {/if}

          <div class="buttons">
            <button class="button is-success is-fullwidth" on:click={() => handleSave(item)}>Save</button>
            <button class="button is-danger is-light is-fullwidth" on:click={() => handleDelete(item)}>Delete</button>
          </div>
        </Accordion>
      {/key}
    {/each}
    <button class="add-button button is-link is-fullwidth" on:click={() => handleAdd($selectedTiles[0].index)}>+ Add</button>
  </SidebarBase>
{/if}

<style>
  p {
    font-weight: bold;
    display: block;
    margin-left: 0;
    margin-right: 0;
  }

  p:not(:last-child) {
    margin-bottom: 0.5rem;
  }

  p:not(:first-child) {
    margin-top: 1rem;
  }

  .buttons {
    display: flex;
    margin: 1rem 0 0 0;
  }

  .buttons > button {
    margin: 0;
    flex: 1;
  }

  .buttons > button:not(:last-child) {
    margin-right: 0.5rem;
  }

  .add-button {
    margin-top: 1rem;
  }

  .input-group {
    display: flex;
  }

  .input-group > input:not(:last-child) {
    margin-right: 0.5rem;
  }

  .select > select {
    flex: 1;
  }
</style>
