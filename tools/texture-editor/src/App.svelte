<script lang="ts">
  import Router from 'svelte-spa-router';
  import Home from './routes/home.svelte';
  import TextureEditor from './routes/texture-editor.svelte';
  import TileEditor from './routes/tile-editor.svelte';

  const routes = {
    '/': Home,
    '/texture-editor': TextureEditor,
    '/tile-editor': TileEditor,
  };
</script>

<Router {routes} />
