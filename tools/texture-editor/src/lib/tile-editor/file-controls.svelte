<script lang="ts">
  import { mdiTextureBox, mdiText } from '@mdi/js';
  import { saveObject } from '../../common/utils';
  import FileInput from '../../common/file-input.svelte';
  import { textureSource, textureFrames, textureData, tileSize, tileData } from './store';
  import type { TextureData, TileData, Frame } from './types';

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

  const onTileDataLoad = async (event: Event) => {
    const target = (event.target as HTMLInputElement);

    if (!target.files || target.files.length !== 1) {
      return;
    }

    const file = target.files[0];
    const tileJSONData: TileData = await readTextureJSONData(file);

    tileData.set(tileJSONData);
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

</script>

<div class="controls">
  <FileInput class="te-file-input {!$textureSource ? 'is-warning is-light' : ''}" icon={mdiTextureBox} label="Load Texture" accept="image/*" onChange={onTextureLoad} />
  <FileInput class="te-file-input {!$textureData ? 'is-warning is-light' : ''}" icon={mdiText} label="Load Texture Data" accept="application/JSON" onChange={onTextureDataLoad} />
  <FileInput class="te-file-input {!$tileData ? 'is-warning is-light' : ''}" icon={mdiText} label="Load Tile Data" accept="application/JSON" onChange={onTileDataLoad} />

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
