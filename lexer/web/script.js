const codeInput = document.getElementById('codeInput');
const analyzeBtn = document.getElementById('analyze');
const replaceBtn = document.getElementById('replace');
const resultsBody = document.getElementById('results').getElementsByTagName('tbody')[0];

analyzeBtn.addEventListener('click', () => {
    fetch('http://127.0.0.1:8080/lex', {
        method: 'POST', body: codeInput.value,
    })
        .then(response => response.json())
        .then(data => {
            resultsBody.innerHTML = '';
            data.forEach(item => {
                const row = document.createElement('tr');
                const lexemeCell = document.createElement('td');
                lexemeCell.textContent = item.lexeme;
                const typeCell = document.createElement('td');
                typeCell.textContent = item.type;

                row.appendChild(lexemeCell);
                row.appendChild(typeCell);
                resultsBody.appendChild(row);
            });
        });
})

replaceBtn.addEventListener('click', () => {
    fetch('http://127.0.0.1:8080/replace', {
        method: 'POST', body: codeInput.value,
    })
        .then(response => response.json())
        .then(data => {
            codeInput.value = '';
            data.forEach(item => {
                codeInput.value += item.lexeme;
                codeInput.value += '\n';
            })
        });
})