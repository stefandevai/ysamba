<script lang="ts">
	import type { Optional } from './types';

	export let title: Opional<string> = undefined;
	let isHovering = false;
	let x: number;
	let y: number;
	
  const handleMouseOver = (event) => {
		isHovering = true;
		x = event.pageX + 5;
		y = event.pageY + 5;
	}

  const handleMouseMove = (event) => {
		x = event.pageX + 5;
		y = event.pageY + 5;
	}

  const handleMouseLeave = () => {
		isHovering = false;
	}
</script>

<div
	on:mouseover={handleMouseOver}
	on:focus={handleMouseOver}
	on:blur={handleMouseOver}
  on:mouseleave={handleMouseLeave}
	on:mousemove={handleMouseMove}
	role="tooltip"
>
	<slot />
</div>

{#if isHovering && title}
	<div
		style="top: {y}px; left: {x}px;"
		class="tooltip"
	>
		{title}
	</div>
{/if}

<style>
	.tooltip {
		border: 1px solid #ddd;
		box-shadow: 1px 1px 1px #ddd;
		background: white;
		border-radius: 4px;
		padding: 4px;
		position: absolute;
		z-index: 3;
	}
</style>
