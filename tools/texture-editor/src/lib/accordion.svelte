<script lang="ts">
  import SvgIcon from '@jamescoyle/svelte-icon';
  import { mdiChevronDown, mdiChevronUp } from '@mdi/js';

  export let title = '';
  export let open = false;

  const handleHeaderClick = () => {
    open = !open;
  }
</script>

<div class="wrapper {$$restProps.class}">
  <div class="header {open ? 'header-open' : 'header-closed'} has-background-dark has-text-white" on:click={handleHeaderClick}>
    <span>{title}</span>
    <SvgIcon class="te-icon" type="mdi" path={open ? mdiChevronUp : mdiChevronDown} />
  </div>
  <div class="content {open === false ? 'content-hidden' : 'content-shown'}">
    <slot />
  </div>
</div>

<style>
  .header {
    width: 100%;
    user-select: none;
    cursor: pointer;
    padding: 1rem;
    background-color: blue;
    font-weight: bold;
    display: flex;
    align-items: center;
    justify-content: space-between;
  }

  .header > :global(svg) {
    height: 20px;
    width: auto;
  }

  .header-open {
    border-radius: 4px 4px 0 0;
  }

  .header-closed {
    border-radius: 4px;
  }

  .wrapper:not(:last-child) {
    margin-bottom: 0.5rem;
  }

  .content-hidden {
    height: 0;
    overflow: hidden;
    margin: 0;
  }

  .content-shown {
    padding: 1rem 1rem 1rem 1rem;
    margin-bottom: 1rem;
    border-bottom: 1px solid var(--color-light-gray);
    border-left: 1px solid var(--color-light-gray);
    border-right: 1px solid var(--color-light-gray);
    border-radius: 0 0 4px 4px;
    /* transition: all 0.3s ease; */
  }
</style>

