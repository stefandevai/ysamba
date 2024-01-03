<script lang="ts">
  import SvgIcon from '@jamescoyle/svelte-icon';
  import { mdiCollage } from '@mdi/js';
  import FileControls from './file-controls.svelte';
  import GridControls from './grid-controls.svelte';
  import Grid from './grid.svelte';
  import Inspector from './inspector.svelte';
</script>

<div class="te-header">
  <SvgIcon class="te-icon" type="mdi" path={mdiCollage} />
  <h1 class="is-size-2">Texture Editor</h1>
  <FileControls />
</div>
<GridControls />
<section class="editor">
  <Grid />
  <Inspector />
</section>

<style>
  .editor {
    display: flex;
    height: 100%;
    overflow: hidden;
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
