<script lang="ts">
  import { mdiUpload } from '@mdi/js';
  import SvgIcon from '@jamescoyle/svelte-icon';

  export let label: string;
  export let accept: string;
  export let icon = '';
  export let onChange: (event: Event) => void;

  let fileName = '';

  const localOnChange = (event: Event) => {
    const target = (event.target as HTMLInputElement);

    if (target.files && target.files.length > 0) {
      fileName = target.files[0].name;
    }

    if (onChange != null) {
      onChange(event);
    }
  }
</script>

<div class="file {$$restProps.class}">
  <label class="file-label">
    <input class="file-input" type="file" accept={accept} on:change={localOnChange}>
    <span class="file-cta">
      <span class="file-icon">
        <SvgIcon type="mdi" path={icon ?? mdiUpload} />
      </span>
      <span class="file-label">
        {#if fileName}
          {fileName}
        {:else}
          {label}
        {/if}
      </span>
    </span>
  </label>
</div>
