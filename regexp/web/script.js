// JavaScript logic for tab switching and conversion

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

function convert(tabNumber) {
    // let inputId = "input" + tabNumber;
    // let outputId = "output" + tabNumber;
    // let inputValue = document.getElementById(inputId).value;
    // let outputTable = document.getElementById(outputId);
    // // You can add your conversion logic here and update the table content
    // // For now, let's display the input value in the table.
    // outputTable.innerHTML = "<tr><td>" + inputValue + "</td></tr>";
}

// Show the first tab by default
openTab('tab1');

const NFAData = [{
    "char": 108, "from": 1, "is_end": false, "is_start": true, "to": 2
}, {
    "char": 0, "from": 2, "is_end": false, "is_start": false, "to": 7
}, {
    "char": 108, "from": 3, "is_end": false, "is_start": false, "to": 4
}, {
    "char": 0, "from": 4, "is_end": true, "is_start": false, "to": 8
}, {
    "char": 100, "from": 5, "is_end": false, "is_start": false, "to": 6
}, {
    "char": 0, "from": 6, "is_end": true, "is_start": false, "to": 8
}, {
    "char": 0, "from": 7, "is_end": false, "is_start": false, "to": 3
}, {
    "char": 0, "from": 7, "is_end": false, "is_start": false, "to": 5
}, {
    "char": 0, "from": 8, "is_end": false, "is_start": false, "to": 7
}, {
    "char": 0, "from": 8, "is_end": false, "is_start": false, "to": 10
}, {
    "char": 0, "from": 9, "is_end": false, "is_start": false, "to": 7
}, {
    "char": 0, "from": 9, "is_end": false, "is_start": false, "to": 10
}]

function populateNFATable(data) {
    // Get all unique states (excluding empty states)
    const uniqueStates = Array.from(new Set(data.filter(item => item.from !== undefined).map(item => item.from)));
    console.log(uniqueStates);
    const uniqueTransitions = Array.from(new Set(data.filter(item => item.char !== undefined).map(item => item.char)));
    uniqueTransitions.sort((a, b) => b - a);
    console.log(uniqueTransitions);

    const table1 = document.getElementById("output1");

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
    console.log(startStates);
    console.log(endStates);

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

    // table1.innerHTML = "<tr><th></th><th>From</th></tr>";
    //
    // // Create column headers for each unique state
    // for (const state of uniqueStates) {
    //     table1.innerHTML += `<th>${state}</th>`;
    // }

    // table1.innerHTML += "</tr>";
    //
    // // Populate the rows based on transitions
    // for (const state of uniqueStates) {
    //     table1.innerHTML += "<tr>";
    //
    //     // First column: Start and end state indication
    //     const isStartState = data.find(item => item.from === state && item.is_start);
    //     const isEndState = data.find(item => item.from === state && item.is_end);
    //
    //     table1.innerHTML += `<td>${isStartState ? "S" : ""}${isEndState ? "E" : ""}</td>`;
    //     table1.innerHTML += `<td>${state}</td>`;
    //
    //     // Subsequent columns: Transition characters
    //     for (const targetState of uniqueStates) {
    //         const transitionChars = tableData
    //             .filter(item => item.from === state && item.to === targetState)
    //             .map(item => (item.char === 0 ? "ε" : String.fromCharCode(item.char)))
    //             .join(", ");
    //         table1.innerHTML += `<td>${transitionChars || ""}</td>`;
    //     }
    //
    //     table1.innerHTML += "</tr>";
    // }
}

populateNFATable(NFAData);
