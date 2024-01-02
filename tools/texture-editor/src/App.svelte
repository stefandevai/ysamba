<script lang="ts">
  import Editor from './lib/editor.svelte';
</script>

<main>
  <Editor />
</main>

<style>
  :global(#app) {
    display: flex;
    height: 100%;
  }

  main {
    flex: 1;
    margin: 1rem 8rem;
    display: flex;
    flex-direction: column;
  }
</style>
