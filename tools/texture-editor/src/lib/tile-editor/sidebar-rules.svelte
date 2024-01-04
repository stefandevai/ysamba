<script lang="ts">
  import SidebarBase from '../../common/sidebar-base.svelte';
  import Accordion from '../../common/accordion.svelte';
  import EightSidedTerrainForm from './eight-sided-terrain-form.svelte';
  import FourSidedTerrainForm from './four-sided-terrain-form.svelte';
  import { createEightSidedTerrainRule, createFourSidedTerrainRule, RuleType } from './types';
  import type { Rule } from './types';

  let rules: Rule[] = [];

  const handleAdd = () => {
    /* const rule = createEightSidedTerrainRule('terrain', 0); */
    const rule = createFourSidedTerrainRule('terrain', 0);
    rules = [
      ...rules,
      rule,
    ];
  }

  const getRuleComponentByType = (type: RuleType) => {
    switch (type) {
      case RuleType.EIGHT_SIDED_TERRAIN:
        return EightSidedTerrainForm;
      case RuleType.FOUR_SIDED_TERRAIN:
        return FourSidedTerrainForm;
      default:
        return EightSidedTerrainForm;
    }
  }
</script>

<SidebarBase title="Rules">
  {#each rules as rule, index}
    {#key rule.id}
      <Accordion title={rule.name} open={index === 0}>
        <p>ID:</p>
        <input class="input" id="id" type="number" bind:value={rule.id} />
        <svelte:component this={getRuleComponentByType(rule.type)} rule={rule} />
      </Accordion>
    {/key}
  {/each}
  <button class="add-button button is-link is-fullwidth" on:click={() => handleAdd()}>+ Add</button>
</SidebarBase>

<style>
  .add-button {
    margin-top: 1rem;
  }

  p {
    font-weight: bold;
    display: block;
    margin-left: 0;
    margin-right: 0;
  }

  p:not(:last-child) {
    margin-bottom: 0.5rem;
  }

  p:not(:first-child) {
    margin-top: 1rem;
  }
</style>
