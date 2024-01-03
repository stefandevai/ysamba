<script lang="ts">
  import SvgIcon from '@jamescoyle/svelte-icon';
  import { mdiCollage } from '@mdi/js';
  import FileControls from './file-controls.svelte';
  import GridControls from './grid-controls.svelte';
  import SidebarControls from './sidebar-controls.svelte';
  import Grid from './grid.svelte';
  import SidebarInspector from './sidebar-inspector.svelte';
  import SidebarTiles from './sidebar-tiles.svelte';
  import SidebarTileDetails from './sidebar-tile-details.svelte';
  import SidebarRules from './sidebar-rules.svelte';
  import { sidebar } from './store';
</script>

<div class="te-header">
  <SvgIcon class="te-icon" type="mdi" path={mdiCollage} />
  <h1 class="is-size-2">Tile Editor</h1>
  <FileControls />
</div>
<div class="main-controls">
  <GridControls />
  <SidebarControls />
</div>
<section class="editor">
  <div class="grid">
    <Grid />
  </div>

  {#if $sidebar === 'inspector'}
    <SidebarInspector />
  {:else if $sidebar === 'tiles'}
    <SidebarTiles />
  {:else if $sidebar === 'tile'}
    <SidebarTileDetails />
  {:else if $sidebar === 'rules'}
    <SidebarRules />
  {/if}
</section>

<style>
  .main-controls {
    display: flex;
    margin-bottom: 1rem;
  }

  .editor {
    display: flex;
    height: 100%;
    overflow: hidden;
  }

  .grid {
    flex: 1;
    display: flex;
    background-color: var(--color-light-gray);
  }

  .te-header {
    display: flex;
    align-items: center;
    padding-bottom: 1rem;
    margin-bottom: 1rem;
    border-bottom: 1px solid var(--color-light-gray);
  }

  .te-header > :global(svg) {
    margin-right: 0.5rem;
    height: 100%;
    width: auto;
  }

  h1 {
    margin-right: 1rem;
    font-weight: bold;
  }
</style>
