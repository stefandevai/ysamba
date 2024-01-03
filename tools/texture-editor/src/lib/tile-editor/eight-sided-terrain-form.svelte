<script lang="ts">
  import type { RuleEightSidedTerrain } from './types';
  export let rule: RuleEightSidedTerrain;
</script>

<div class="wrapper">
  <p>Tile 1 ID:</p>
  <input class="input" type="number" bind:value={rule.tile1.id} />

  <p>Tile 3 ID:</p>
  <input class="input" type="number" bind:value={rule.tile3.id} />
</div>

<style>
  p {
    font-weight: bold;
    display: block;
    margin-left: 0;
    margin-right: 0;
  }

  p:not(:last-child) {
    margin-bottom: 0.5rem;
  }

  p {
    margin-top: 1rem;
  }
</style>
