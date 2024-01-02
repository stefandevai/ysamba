<script lang="ts">
  import { textureSource, textureFrames, textureData, tileSize } from './store';
  import type { TextureData, Frame } from './types';
  import { defaultFrame } from './frame';

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

    const loadedFrames = Array<Frame>(textureJSONData.width * textureJSONData.height);
    loadedFrames.fill(defaultFrame);

    for (const frame of textureJSONData.frames) {
      loadedFrames[frame.frame] = frame;
    }

    textureFrames.set(loadedFrames);
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
</script>

<div>
  <input type="number" on:input={onWidthChange} value={$tileSize.width} />
  <input type="number" on:input={onHeightChange} value={$tileSize.height} />
  <input type="file" accept="image/*" on:change={onTextureLoad} />
  <input type="file" accept="application/JSON" on:change={onDataLoad} />
</div>

<style>
</style>
