<script lang="ts">
  export let title = '';
</script>

<aside>
  <div class="header">
    <h2 class="is-size-4">{title}</h2>
  </div>
  <div class="main-content">
    <slot />
  </div>
</aside>

<style>
  aside {
    width: calc(300px + 1rem);
    display: flex;
    flex-direction: column;
    border: 1px solid var(--color-light-gray);
  }

  .header {
    padding-top: 1rem;
    padding-bottom: 1rem;
    margin-bottom: 1rem;
    border-bottom: 1px solid var(--color-light-gray);
  }

  h2 {
    margin-left: 1.5rem;
    font-weight: 500;
  }

  .main-content {
    flex: 1;
    overflow: auto;
    margin-left: 1.5rem;
    margin-right: 1.5rem;
    margin-bottom: 1rem;
  }
</style>
