<script lang="ts">
  import SvgIcon from '@jamescoyle/svelte-icon';
  import { mdiApps, mdiLandPlots } from '@mdi/js';
  import { sidebar } from './store';

  const handleAllTilesClick = () => {
    if ($sidebar === 'tiles') {
      sidebar.set(null);
      return;
    }

    sidebar.set('tiles');
  }

  const handleRulesClick = () => {
    if ($sidebar === 'rules') {
      sidebar.set(null);
      return;
    }

    sidebar.set('rules');
  }
</script>

<div class="controls">
  <button title="All Tiles" class="button" on:click={handleAllTilesClick}>
    <span class="icon is-small">
      <SvgIcon class="te-icon" type="mdi" path={mdiApps} />
    </span>
    <span>Tiles</span>
  </button>
  <button title="Rules" class="button" on:click={handleRulesClick}>
    <span class="icon is-small">
      <SvgIcon class="te-icon" type="mdi" path={mdiLandPlots} />
    </span>
    <span>Rules</span>
  </button>
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
</style>


