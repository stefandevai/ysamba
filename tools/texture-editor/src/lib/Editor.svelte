<script lang="ts">
  import Controls from './controls.svelte';
  import Grid from './grid.svelte';
  import Inspector from './inspector.svelte';
</script>

<Controls />
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
</style>
