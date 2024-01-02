<script lang="ts">
  export let label: string;
  export let accept: string;
  export let onChange: (event: Event) => void;
</script>

<div class="file {$$restProps.class}">
  <label class="file-label">
    <input class="file-input" type="file" accept={accept} on:change={onChange}>
    <span class="file-cta">
      <span class="file-icon">
        <i class="material-icons">upload</i>
      </span>
      <span class="file-label">
        {label}
      </span>
    </span>
  </label>
</div>
