<script lang="ts">
  import Editor from './lib/editor.svelte';
</script>

<main>
  <h1 class="is-size-2">Texture Editor</h1>
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

  h1 {
    margin-bottom: 1rem;
  }
</style>
