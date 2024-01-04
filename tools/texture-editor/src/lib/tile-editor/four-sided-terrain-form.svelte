<script lang="ts">
  import SvgIcon from '@jamescoyle/svelte-icon';
  import { mdiEyedropper } from '@mdi/js';
  import { gridMode, selectingBitmask } from './store';
  import { GridMode } from './types';
  import type { RuleFourSidedTerrain, SideBitmask } from './types';

  export let rule: RuleFourSidedTerrain;

  const handleSelectTile = (bitmask: SideBitmask) => {
    gridMode.set(GridMode.FOUR_SIDED_BITMASK_SELECTION);
    selectingBitmask.set(bitmask.sides);
  };
</script>

<div class="wrapper">
  <div class="input-title">
    <span class="icon is-small" on:click={() => handleSelectTile(rule.tile1)}>
      <SvgIcon class="te-icon" type="mdi" path={mdiEyedropper} />
    </span>
    <p>Tile 1 ID:</p>
  </div>
  <input class="input" type="number" bind:value={rule.tile1.id} />

  <div class="input-title">
    <span class="icon is-small" on:click={() => handleSelectTile(rule.tile3)}>
      <SvgIcon class="te-icon" type="mdi" path={mdiEyedropper} />
    </span>
    <p>Tile 3 ID:</p>
  </div>
  <input class="input" type="number" bind:value={rule.tile3.id} />
</div>

<style>
  .input-title {
    display: flex;
    align-items: center;
    margin-top: 1rem;
  }

  .input-title > p {
    font-weight: bold;
    display: block;
    margin-left: 0.25rem;
    margin-right: 0;
    margin-bottom: 0;
  }

  .input-title:not(:last-child) {
    margin-bottom: 0.5rem;
  }

  .icon {
    cursor: pointer;
  }
</style>
