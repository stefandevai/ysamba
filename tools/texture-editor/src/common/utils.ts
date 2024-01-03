export const saveObject = (name: string, object: unknown) => {
  // Save with line breaks and spaces
  // const objectString = JSON.stringify(object, null, 4);
  const objectString = JSON.stringify(object);
  const type = 'text/plain';
  const a = document.createElement('a');
  const file = new Blob([objectString], { type });

  a.href = URL.createObjectURL(file);
  a.download = name;
  document.body.appendChild(a);
  a.click();
  a.remove();
};
