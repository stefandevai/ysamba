<script lang="ts">
  import { textureSource, textureFrames, textureData, tileSize } from './store';
  import type { TextureData, Frame, FrameArray } from './types';
  import { defaultFrame } from './frame';
  import { saveObject } from './utils';
  import FileInput from './file-input.svelte';

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

  const onTextureLoad = async (event) => {
    if (!event.target.files || event.target.files.length !== 1) {
      return;
    }

    const file = event.target.files[0];
    textureSource.set(await readTextureData(file));
  };

  const onDataLoad = async (event) => {
    if (!event.target.files || event.target.files.length !== 1) {
      return;
    }

    const file = event.target.files[0];
    const textureJSONData: TextureData = await readTextureJSONData(file);

    tileSize.set({
      width: textureJSONData.tile_width,
      height: textureJSONData.tile_height,
    });

    const horizontalTiles = textureJSONData.width / textureJSONData.tile_width;
    const verticalTiles = textureJSONData.height / textureJSONData.tile_height;

    const loadedFrames = Array<Frame[]>(horizontalTiles * verticalTiles);

    for (let i = 0; i < loadedFrames.length; i++) {
      loadedFrames[i] = [];
    }

    for (const frame of textureJSONData.frames) {
      const key = loadedFrames[frame.frame].length;

      loadedFrames[frame.frame].push({
        ...frame,
        key,
      });
    }

    textureFrames.set(loadedFrames);
    textureData.set(textureJSONData);
  };

  const onWidthChange = (event) => {
    tileSize.update((size) => ({
      ...size,
      width: parseInt(event.target.value),
    }));
  }

  const onHeightChange = (event) => {
    tileSize.update((size) => ({
      ...size,
      height: parseInt(event.target.value),
    }));
  }

  const handleSave = () => {
    if (!$textureData || !$textureFrames || $textureFrames.length === 0) {
      return;
    }

    const allFrames = $textureFrames.reduce((acc, current) => [...acc, ...current]);

    for (const frame of allFrames) {
      delete frame.key;
    }

    const newTextureData = {
      ...$textureData,
      frames: allFrames,
    };

    saveObject('tileset.json', newTextureData);
  }

</script>

<div class="controls">
  {#if $textureSource}
    <input class="input" type="number" on:input={onWidthChange} value={$tileSize.width} />
    <input class="input" type="number" on:input={onHeightChange} value={$tileSize.height} />
  {/if}
  <FileInput class="te-file-input" label="Load Texture" accept="image/*" onChange={onTextureLoad} />
  <FileInput class="te-file-input" label="Load Data" accept="application/JSON" onChange={onDataLoad} />

  {#if $textureData && $textureSource}
    <button class="button is-primary" on:click={handleSave}>Save</button>
  {/if}
</div>

<style>
  .controls {
    display: flex;
    margin-bottom: 1rem;
  }

  .controls > *:not(:last-child) {
    margin-right: 1rem;
  }

  .controls > :global(.te-file-input) {
    margin-right: 1rem;
  }
</style>
