<script lang="ts">
  import SvgIcon from '@jamescoyle/svelte-icon';
  import { mdiPlus, mdiMinus } from '@mdi/js';
  import { tileSize, zoom } from './store';

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

  const handleZoomIn = () => {
    zoom.update(value => {
      if (value >= 8) {
        return 8;
      }

      return value * 2;
    });
  }

  const handleZoomOut = () => {
    zoom.update(value => {
      if (value <= 1) {
        return 1;
      }

      return value / 2;
    });
  }
</script>

<div class="controls">
  <input class="input" type="number" on:input={onWidthChange} value={$tileSize.width} />
  <input class="input" type="number" on:input={onHeightChange} value={$tileSize.height} />
  <button class="button" on:click={handleZoomIn}>
    <span class="icon is-small">
      <SvgIcon class="te-icon" type="mdi" path={mdiPlus} />
    </span>
  </button>
  <button class="button" on:click={handleZoomOut}>
    <span class="icon is-small">
      <SvgIcon class="te-icon" type="mdi" path={mdiMinus} />
    </span>
  </button>
</div>

<style>
  .controls {
    display: flex;
    margin-bottom: 1rem;
  }

  .controls > *:not(:last-child) {
    margin-right: 1rem;
  }

  input[type=number] {
    width: 60px;
  }
</style>

