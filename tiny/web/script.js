const input = document.getElementById('input-text');
const convert_button = document.getElementById('convert-button');
const save_button = document.getElementById('save-button');
const load_button = document.getElementById('load-button');

// save the text from input to file
save_button.addEventListener('click', () => {
    const text = input.value;
    const filename = 'input.txt';
    const type = 'text/plain';
    const a = document.createElement('a');
    const file = new Blob([text], { type: type });
    a.href = URL.createObjectURL(file);
    a.download = filename;
    a.click();
});

// load the text from file to input
load_button.addEventListener('click', () => {
    // Trigger the file input dialog
    document.getElementById('file-input').click();

    // Add change event listener to file input
    document.getElementById('file-input').addEventListener('change', function () {
        const file = this.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = function (e) {
                const text = e.target.result;
                // Make sure you reference the text area correctly
                document.getElementById('input-text').value = text;
            };
            reader.readAsText(file);
        }
    });
});

// convert the text from input to ast
convert_button.addEventListener('click', () => {
    const text = input.value;
    fetch('http://localhost:8080/ast', {
        method: 'POST', body: text,
    })
        .then(response => response.json())
        .then(data => {
            console.log(data);
            // populate to div id output-ast
            const output = document.getElementById('output-ast');
            // replace \n with <br>
            data['ast'] = data['ast'].replace(/\n/g, '<br>');

            // remove this line
            // TINY COMPILATION: input.tny
            data['ast'] = data['ast'].replace(/TINY COMPILATION: input.tny/g, '');

            output.innerHTML = data['ast'];
        });
})