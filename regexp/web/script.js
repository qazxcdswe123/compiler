function saveText(tabNumber) {
    const textareaContent = document.getElementById("input" + tabNumber).value;

    const blob = new Blob([textareaContent], {type: "text/plain"});
    const url = URL.createObjectURL(blob);

    const a = document.createElement('a');
    a.href = url;
    a.download = 'saved_regex.txt';
    a.click();

    URL.revokeObjectURL(url);
}

function loadText(tabNumber) {
    const fileInput = document.getElementById('load' + tabNumber);
    const file = fileInput.files[0];

    const reader = new FileReader();
    reader.onload = function (e) {
        document.getElementById("input" + tabNumber).value = e.target.result;
    };
    reader.readAsText(file);
}

function openTab(tabName) {
    let i, tabcontent, tablinks;

    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }

    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }

    document.getElementById(tabName).style.display = "block";
}

function populateCode(data) {
    // populate to id codeOutput
    const codeOutput = document.getElementById("codeOutput4");
    // escape bracket in <string>
    let code = data["code"].replace(/</g, "&lt;").replace(/>/g, "&gt;");
    codeOutput.innerHTML = code;
}

function convertToTable(tabNumber) {
    let inputId = "input" + tabNumber;
    let inputValue = document.getElementById(inputId).value;
    let url = 'http://127.0.0.1:8080/'
    if (tabNumber === 1) {
        url += 're2nfa'
        const data = fetch(url, {
            method: 'POST', body: inputValue,
        }).then(resp => resp.json()).then(data => {
            populateNFATable(data);
        });
    } else if (tabNumber === 2) {
        url += 're2dfa'
        const data = fetch(url, {
            method: 'POST', body: inputValue,
        }).then(resp => resp.json()).then(data => {
            populateDFATable(data);
        });
    } else if (tabNumber === 3) {
        url += 're2minidfa'
        const data = fetch(url, {
            method: 'POST', body: inputValue,
        }).then(resp => resp.json()).then(data => {
            populateMiniDFATable(data);
        });
    } else if (tabNumber === 4) {
        url += 're2code'
        const data = fetch(url, {
            method: 'POST', body: inputValue,
        }).then(resp => resp.json()).then(data => {
            populateCode(data);
        });
    }
}

// Show the first tab by default
openTab('tab1');

function populateNFATable(data) {
    // Get all unique states (excluding empty states), both from and to
    const uniqueStates = Array.from(new Set(data.map(item => item.from).concat(data.map(item => item.to))))
    const uniqueTransitions = Array.from(new Set(data.filter(item => item.char !== undefined).map(item => item.char)));
    uniqueTransitions.sort((a, b) => b - a);

    const table1 = document.getElementById("output1");

    // clear the table first
    while (table1.firstChild) {
        table1.removeChild(table1.firstChild);
    }

    /*
    the first col is for the start and end state indication
    the second col is for the state id
    the rest of the cols are for the transition characters, with the last one being epsilon transition
     */

    firstRow = document.createElement("tr");
    firstRow.innerHTML = "<th>Start or End?</th><th>From</th>";
    // Create column headers for each unique transition symbol
    uniqueTransitions.forEach(char => {
        firstRow.innerHTML += `<th>${char === 0 ? "ε" : String.fromCharCode(char)}</th>`;
    })
    table1.appendChild(firstRow);

    // Populate the rows based on transitions for each state
    // First column: Start and end state indication, if is_start means the from state is the start state, if is_end means the to state is the end state
    // mark all the start states and end states first
    const startStates = [...new Set(data.filter(item => item.is_start).map(item => item.from))]
    const endStates = [...new Set(data.filter(item => item.is_end).map(item => item.to))];

    uniqueStates.forEach(state => {
        const row = document.createElement("tr");
        row.innerHTML += `<td>${startStates.includes(state) ? "S" : ""}${endStates.includes(state) ? "E" : ""}</td>`;
        row.innerHTML += `<td>${state}</td>`;
        uniqueTransitions.forEach(char => {
            const transitionStates = data.filter(item => item.from === state && item.char === char).map(item => item.to);
            row.innerHTML += `<td>${transitionStates.join(", ") || ""}</td>`;
        })
        table1.appendChild(row);
    });
}

function populateDFATable(data) {
    // Get all unique states (both from and to, excluding empty states)
    const States = Array.from(new Set(data.map(item => item.from).concat(data.map(item => item.to))))

    const uniqueStates = States.filter((item, index) => {
        return States.findIndex(i => JSON.stringify(i) === JSON.stringify(item)) === index;
    });
    const uniqueTransitions = Array.from(new Set(data.filter(item => item.char !== undefined).map(item => item.char)));
    uniqueTransitions.sort((a, b) => b - a);

    const table = document.getElementById("output2");
    while (table.firstChild) {
        table.removeChild(table.firstChild);
    }

    /*
    the first col is for the start and end state indication
    the second col is for the state id
    the rest of the cols are for the transition characters, with the last one being epsilon transition
     */

    firstRow = document.createElement("tr");
    firstRow.innerHTML = "<th>Start or End?</th><th>From</th>";
    // Create column headers for each unique transition symbol
    uniqueTransitions.forEach(char => {
        firstRow.innerHTML += `<th>${String.fromCharCode(char)}</th>`;
    })
    table.appendChild(firstRow);

    // Populate the rows based on transitions for each state
    // First column: Start and end state indication, if is_start means the from state is the start state, if is_end means the to state is the end state
    // mark all the start states and end states first
    const startStates = [...new Set(data.filter(item => item.is_start).map(item => item.from))]
    const uniqueStartStates = startStates.filter((item, index) => {
        return startStates.findIndex(i => JSON.stringify(i) === JSON.stringify(item)) === index;
    });
    const endStates = [...new Set(data.filter(item => item.is_end).map(item => item.to))];
    const uniqueEndStates = endStates.filter((item, index) => {
        return endStates.findIndex(i => JSON.stringify(i) === JSON.stringify(item)) === index;
    });

    console.log(data)
    uniqueStates.forEach(state => {
        console.log(state)
        const row = document.createElement("tr");
        // start or end
        let soe_text = "";
        if (contain(uniqueStartStates, state)) {
            soe_text += "S";
        }
        if (contain(uniqueEndStates, state)) {
            soe_text += "E";
        }
        row.innerHTML += `<td>${soe_text}</td>`;
        row.innerHTML += `<td>${state}</td>`;
        uniqueTransitions.forEach(char => {
            const toState = data.filter(item => JSON.stringify(item.from) === JSON.stringify(state) && item.char === char).map(item => item.to);
            row.innerHTML += `<td>${toState.join(", ") || ""}</td>`;
        })
        table.appendChild(row);
    });
}

function contain(arr_of_arr, arr) {
    for (let i = 0; i < arr_of_arr.length; i++) {
        if (JSON.stringify(arr_of_arr[i]) === JSON.stringify(arr)) {
            return true;
        }
    }
    return false;
}

function populateMiniDFATable(data) {
    data = data.filter(item => item.from !== undefined && item.to !== undefined && item.char !== undefined)
    const States = Array.from(new Set(data.map(item => item.from).concat(data.map(item => item.to))))
    const uniqueStates = States.filter((item, index) => {
        return States.findIndex(i => JSON.stringify(i) === JSON.stringify(item)) === index;
    });
    console.log(data)
    const uniqueTransitions = Array.from(new Set(data.map(item => item.char)));
    console.log(uniqueTransitions)
    const table = document.getElementById("output3");

    while (table.firstChild) {
        table.removeChild(table.firstChild);
    }

    firstRow = document.createElement("tr");
    firstRow.innerHTML = "<th>Start or End?</th><th>From</th>";

    uniqueTransitions.forEach(chars => {
        // use map to construct a new array
        firstRow.innerHTML += `<th>${chars.map(char => String.fromCharCode(char)).join(", ")}</th>`;
    })
    table.appendChild(firstRow);

    const startStates = [...new Set(data.filter(item => item.is_start).map(item => item.from))]
    const uniqueStartStates = startStates.filter((item, index) => {
        return startStates.findIndex(i => JSON.stringify(i) === JSON.stringify(item)) === index;
    });
    const endStates = [...new Set(data.filter(item => item.is_end).map(item => item.to))];
    const uniqueEndStates = endStates.filter((item, index) => {
        return endStates.findIndex(i => JSON.stringify(i) === JSON.stringify(item)) === index;
    });

    uniqueStates.forEach(state => {
        const row = document.createElement("tr");
        // start or end
        let soe_text = "";
        if (contain(uniqueStartStates, state)) {
            soe_text += "S";
        }
        if (contain(uniqueEndStates, state)) {
            soe_text += "E";
        }
        row.innerHTML += `<td>${soe_text}</td>`;
        // keep the array with [], do not expand using stringnify
        let fromState = state.map(item => JSON.stringify(item)).join(", ");
        row.innerHTML += `<td>${fromState}</td>`;

        uniqueTransitions.forEach(char => {
            const States = data.filter(item => item.from === state && item.char === char).map(item => item.to);
            let toState = States.map(item => item.map(i => JSON.stringify(i)).join(", ")).join(", ");
            row.innerHTML += `<td>${toState || ""}</td>`;
        })
        table.appendChild(row);
    });
}