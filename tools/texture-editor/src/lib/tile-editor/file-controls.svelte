<script lang="ts">
  import { onMount } from 'svelte';
  import { mdiTextureBox, mdiText } from '@mdi/js';
  import { saveObject } from '../../common/utils';
  import FileInput from '../../common/file-input.svelte';
  import { set as idbSet, get as idbGet } from 'idb-keyval';
  import type { TextureData, TileData, Frame, Size } from './types';
  import {
    textureSource,
    textureFrames,
    textureData,
    tileSize,
    tileData,
    textureFilename,
    textureDataFilename,
    tileDataFilename,
  } from './store';

  const loadExistingData = async () => {
    const textureSourceString: string = await idbGet('textureSource');
    const textureFramesString: string = await idbGet('textureFrames');
    const textureDataString: string = await idbGet('textureData');
    const tileSizeString: string = await idbGet('tileSize');
    const tileDataString: string = await idbGet('tileData');
    const textureFilenameString: string = await idbGet('textureFilename');
    const textureDataFilenameString: string = await idbGet('textureDataFilename');
    const tileDataFilenameString: string = await idbGet('tileDataFilename');

    if (textureSourceString) {
      textureSource.set((JSON.parse(textureSourceString) as string));
    }

    if (textureFramesString) {
      textureFrames.set((JSON.parse(textureFramesString) as Frame[]));
    }

    if (textureDataString) {
      textureData.set((JSON.parse(textureDataString) as TextureData));
    }

    if (tileSizeString) {
      tileSize.set((JSON.parse(tileSizeString) as Size));
    }

    if (tileDataString) {
      tileData.set((JSON.parse(tileDataString) as TileData));
    }

    if (textureFilenameString) {
      textureFilename.set(textureFilenameString);
    }

    if (textureDataFilenameString) {
      textureDataFilename.set(textureDataFilenameString);
    }

    if (tileDataFilenameString) {
      tileDataFilename.set(tileDataFilenameString);
    }
  };

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

  const onTextureLoad = async (event: Event) => {
    const target = (event.target as HTMLInputElement);

    if (!target.files || target.files.length !== 1) {
      return;
    }

    const file = target.files[0];
    textureSource.set(await readTextureData(file));
    textureFilename.set(file.name);
    await idbSet('textureSource', JSON.stringify($textureSource));
    await idbSet('textureFilename', $textureFilename);
  };

  const onTextureDataLoad = async (event: Event) => {
    const target = (event.target as HTMLInputElement);

    if (!target.files || target.files.length !== 1) {
      return;
    }

    const file = target.files[0];
    const textureJSONData: TextureData = await readTextureJSONData(file);

    tileSize.set({
      width: textureJSONData.tile_width,
      height: textureJSONData.tile_height,
    });
    await idbSet('tileSize', JSON.stringify($tileSize));

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
    textureDataFilename.set(file.name);
    await idbSet('textureFrames', JSON.stringify($textureFrames));
    await idbSet('textureData', JSON.stringify($textureData));
    await idbSet('textureDataFilename', $textureDataFilename);
  };

  const onTileDataLoad = async (event: Event) => {
    const target = (event.target as HTMLInputElement);

    if (!target.files || target.files.length !== 1) {
      return;
    }

    const file = target.files[0];
    const tileJSONData: TileData = await readTextureJSONData(file);

    tileData.set(tileJSONData);
    tileDataFilename.set(file.name);
    await idbSet('tileData', JSON.stringify($tileData));
    await idbSet('tileDataFilename', $tileDataFilename);
  };

  const handleSave = () => {
    if (!$textureData || !$textureFrames || $textureFrames.length === 0) {
      return;
    }

    const allFrames: Frame[] = $textureFrames.reduce((acc, current) => [...acc, ...current] as Frame[]);

    for (const frame of allFrames) {
      delete frame.key;
    }

    const newTextureData = {
      ...$textureData,
      frames: allFrames,
    };

    saveObject('tileset.json', newTextureData);
  }

  onMount(() => {
    void loadExistingData();
  });
</script>

<div class="controls">
  <FileInput
    class="te-file-input {!$textureSource ? 'is-warning is-light' : ''}"
    icon={mdiTextureBox}
    label={$textureFilename ?? "Load Texture"}
    accept="image/*"
    onChange={onTextureLoad}
  />

  <FileInput
    class="te-file-input {!$textureData ? 'is-warning is-light' : ''}"
    icon={mdiText}
    label={$textureDataFilename ?? "Load Texture Data"}
    accept="application/JSON"
    onChange={onTextureDataLoad}
  />

  <FileInput
    class="te-file-input {!$tileData ? 'is-warning is-light' : ''}"
    icon={mdiText}
    label={$tileDataFilename ?? "Load Tile Data"}
    accept="application/JSON"
    onChange={onTileDataLoad}
  />

  <button class="button is-success" on:click={handleSave} disabled={!$textureData || !$textureSource}>Save</button>
</div>

<style>
  .controls {
    flex: 1;
    display: flex;
    justify-content: flex-end;
  }

  .controls > *:not(:last-child) {
    margin-right: 1rem;
  }

  .controls > :global(.te-file-input) {
    margin-right: 1rem;
  }
</style>
