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

            // replace \n with <br>
            data['ast'] = data['ast'].replace(/\n/g, '<br>');

            // remove this line
            // TINY COMPILATION: input.tny
            data['ast'] = data['ast'].replace(/TINY COMPILATION: input.tny/g, '');

            // remove first 3 <br>
            data['ast'] = data['ast'].replace(/<br><br><br>/g, '');

            // remove the last <br>
            data['ast'] = data['ast'].replace(/<br>$/g, '');

            document.getElementById('output-ast').innerHTML = '';
            generateASTHTML(data['ast']);
        });
})

function generateASTHTML(data) {
    /*
    Syntax tree:<br>    Read: x<br>    If<br>        Op: <<br>            Const: 0<br>            Id: x<br>        For<br>            Assign to: fact<br>                Id: x<br>            Const: 1<br>            Assign to: fact<br>                Op: *<br>                    Id: fact<br>                    Id: x<br>        Write<br>            Id: fact<br>
    */

    // create a div for every element
    data.split('<br>').forEach(element => {
        const div = document.createElement('div');
        // get the indent level of element, 4 space is 1 level
        const indent_level = element.split('    ').length - 1;
        div.id = 'indent-' + indent_level;
        if (indent_level != 0)
            div.style.display = 'none'

        const span = document.createElement('span');
        span.innerHTML = element;

        const btn = document.createElement('button');
        btn.id = 'expand'
        btn.innerHTML = '+';
        btn.value = indent_level;
        // onclick expand all current indent level
        btn.addEventListener('click', () => {
            const indent_level = parseInt(btn.value);
            const elements = document.getElementById('output-ast').children;
            for (let i = 0; i < elements.length; i++) {
                const element = elements[i];
                const next_level = indent_level + 1;
                const next_level_id = 'indent-' + next_level;
                if (element.id === next_level_id) {
                    element.style.display = 'block';
                }
            }
        });

        div.appendChild(btn);
        div.appendChild(span);
        document.getElementById('output-ast').appendChild(div);
    })
}