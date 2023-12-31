/*
Json example:
DFAStates (Array of Arrays): A list of states used in a Deterministic Finite Automaton (DFA), where each state is an array of objects.

dotPos (Integer): Represents the position of the dot in the DFA.
left (Integer or String): The left-hand side of a production rule. It's an integer or a string.
right (String): The right-hand side of a production rule.
firstSet (Array of Arrays): Represents the first set in the context of grammars.

Each sub-array contains two elements:
The first element (Integer): A symbol in the grammar.
The second element (Array of Integers): The first set of the symbol.
followSet (Array of Arrays): Represents the follow set in the context of grammars.

Similar to the first set, each sub-array contains:
The first element (Integer): A symbol in the grammar.
The second element (Array of Integers): The follow set of the symbol.
isSLR1 (Boolean): Indicates whether the grammar is an SLR(1) grammar.

procedure (String): A string representing a sequence of debug and info messages related to parsing a specific input string.
*/

const analyzeBtn = document.getElementById("analyze");

analyzeBtn.addEventListener("click", () => {
    const sentence = document.getElementById("sentenceInput").value.replace(/\+/g, '%2B').replace(/=/g, '%3D');
    const grammar = document.getElementById("grammarInput").value.replace(/\+/g, '%2B').replace(/=/g, '%3D');
    console.log(sentence);
    console.log(grammar);
    fetch("http://127.0.0.1:8080/grammar", {
        method: "POST", // x-www-form-urlencoded
        headers: {"Content-Type": "application/x-www-form-urlencoded"}, body: `grammar=${grammar}&sentence=${sentence}`,
    })
        .then((response) => response.json())
        .then((data) => {
            // populate first and follow
            const firstSet = data.firstSet;
            const first_tbl = document.getElementById("first-tbl");
            first_tbl.innerHTML = "";
            first_header = document.createElement("tr");
            first_header.innerHTML = "<th>Symbol</th><th>First Set</th>";
            first_tbl.appendChild(first_header);
            for (const row of firstSet) {
                // if symbol is not uppercase, then skip
                if (row[0] < 65 || row[0] > 90) {
                    continue;
                }
                const tr = document.createElement("tr");
                const symbol = document.createElement("td");
                const firstSet = document.createElement("td");
                symbol.innerText = String.fromCharCode(row[0]);
                firstSet.innerText = row[1].map((x) => String.fromCharCode(x));
                tr.appendChild(symbol);
                tr.appendChild(firstSet);
                first_tbl.appendChild(tr);
            }

            const followSet = data.followSet;
            const follow_tbl = document.getElementById("follow-tbl");
            follow_tbl.innerHTML = "";
            follow_header = document.createElement("tr");
            follow_header.innerHTML = "<th>Symbol</th><th>Follow Set</th>";
            follow_tbl.appendChild(follow_header);
            for (const row of followSet) {
                // if symbol is not uppercase, then skip
                if (row[0] < 65 || row[0] > 90) {
                    continue;
                }
                const tr = document.createElement("tr");
                const symbol = document.createElement("td");
                const followSet = document.createElement("td");
                symbol.innerText = String.fromCharCode(row[0]);
                followSet.innerText = row[1].map((x) => String.fromCharCode(x));
                tr.appendChild(symbol);
                tr.appendChild(followSet);
                follow_tbl.appendChild(tr);
            }


            // populate DFA if it is SLR(1)
            if (data.isSLR1) {
                // say yes first
                const slr1 = document.getElementById("slr1");
                slr1.innerHTML = "";
                const h2 = document.createElement("h2");
                h2.innerText = "Is SLR(1)?";
                slr1.appendChild(h2);
                const p = document.createElement("p");
                p.innerText = "Yes";
                slr1.appendChild(p);

                const DFAStates = data.DFAStates;
                const DFAStateTable = document.createElement("table");
                for (i = 0; i < DFAStates.length; i++) {
                    const state = DFAStates[i];
                    const tr = document.createElement("tr");
                    const stateNum = document.createElement("td");
                    stateNum.innerText = `State ${i}`;
                    tr.appendChild(stateNum);

                    const stateContent = document.createElement("td");
                    const stateContentList = document.createElement("ul");
                    for (const item of state) {
                        const stateContentItem = document.createElement("li");
                        stateContentItem.innerText = `${String.fromCharCode(item.left)} -> ${item.right.slice(0, item.dotPos)}.${item.right.slice(item.dotPos)}`;
                        stateContentList.appendChild(stateContentItem);
                    }
                    stateContent.appendChild(stateContentList);
                    tr.appendChild(stateContent);

                    DFAStateTable.appendChild(tr);
                }
                slr1.appendChild(DFAStateTable);

                const gotoH2 = document.createElement("h2");
                gotoH2.innerText = "Goto Table";
                slr1.appendChild(gotoH2);

                // add goto table

                const gotoTable = document.createElement("table");
                const gotoHeader = document.createElement("tr");
                gotoHeader.innerHTML = "<th>State</th>";
                for (const symbol of data.nonTerminals) {
                    gotoHeader.innerHTML += `<th>${String.fromCharCode(symbol)}</th>`;
                }
                gotoTable.appendChild(gotoHeader);
                for (i = 0; i < DFAStates.length; i++) {
                    const gotoRow = document.createElement("tr");
                    const gotoState = document.createElement("td");
                    gotoState.innerText = `State ${i}`;
                    gotoRow.appendChild(gotoState);
                    for (const symbol of data.nonTerminals) {
                        stringSymbol = String.fromCharCode(symbol);
                        const gotoCell = document.createElement("td");
                        // if the table symbol and the data symbol match and the state id match
                        const goto = data.gotoTable.find((x) => x.symbol === stringSymbol && x.state === i);
                        if (goto) {
                            gotoCell.innerText = goto.goto;
                        }
                        gotoRow.appendChild(gotoCell);
                    }
                    gotoTable.appendChild(gotoRow);
                }
                slr1.appendChild(gotoTable);

                // add action table

                const actionH2 = document.createElement("h2");
                actionH2.innerText = "Action Table";
                slr1.appendChild(actionH2);

                const actionTable = document.createElement("table");
                const actionHeader = document.createElement("tr");
                actionHeader.innerHTML = "<th>State</th>";
                for (const symbol of data.terminals) {
                    actionHeader.innerHTML += `<th>${String.fromCharCode(symbol)}</th>`;
                }
                actionTable.appendChild(actionHeader);
                for (i = 0; i < DFAStates.length; i++) {
                    const actionRow = document.createElement("tr");
                    const actionState = document.createElement("td");
                    actionState.innerText = `State ${i}`;
                    actionRow.appendChild(actionState);

                    for (const symbol of data.terminals) {
                        const actionCell = document.createElement("td");
                        let charCode = String.fromCharCode(symbol);
                        const action = data.actionTable.find((x) => {
                            const sameSymbol = x.symbol === charCode;
                            const sameState = parseInt(x.state) === i;
                            return sameSymbol && sameState;
                        });
                        if (action) {
                            actionCell.innerText = action.action;
                        }
                        actionRow.appendChild(actionCell);
                    }
                    actionTable.appendChild(actionRow);
                }
                slr1.appendChild(actionTable);
            } else {
                // just say no in the div
                const slr1 = document.getElementById("slr1");
                slr1.innerHTML = "";
                const h2 = document.createElement("h2");
                h2.innerText = "Is SLR(1)?";
                slr1.appendChild(h2);
                const p = document.createElement("p");
                p.innerText = "No\n" + data.notSLR1Reason;
                slr1.appendChild(p);

                const DFAStates = data.DFAStates;
                const DFAStateTable = document.createElement("table");
                for (i = 0; i < DFAStates.length; i++) {
                    const state = DFAStates[i];
                    const tr = document.createElement("tr");
                    const stateNum = document.createElement("td");
                    stateNum.innerText = `State ${i}`;
                    tr.appendChild(stateNum);

                    const stateContent = document.createElement("td");
                    const stateContentList = document.createElement("ul");
                    for (const item of state) {
                        const stateContentItem = document.createElement("li");
                        stateContentItem.innerText = `${String.fromCharCode(item.left)} -> ${item.right.slice(0, item.dotPos)}.${item.right.slice(item.dotPos)}`;
                        stateContentList.appendChild(stateContentItem);
                    }
                    stateContent.appendChild(stateContentList);
                    tr.appendChild(stateContent);

                    DFAStateTable.appendChild(tr);
                }
                slr1.appendChild(DFAStateTable);
            }


            // populate procedure
            const procedure = document.getElementById("procedure");
            procedure.innerHTML = "";
            const h2 = document.createElement("h2");
            h2.innerText = "Procedure";
            procedure.appendChild(h2);
            const p = document.createElement("p");
            p.innerText = data.procedure;
            procedure.appendChild(p);
        });
});

// save the grammar to file
const saveBtn = document.getElementById("save");
saveBtn.addEventListener("click", () => {
    const grammar = document.getElementById("grammarInput").value;
    const blob = new Blob([grammar], {type: "text/plain;charset=utf-8"});
    saveAs(blob, "grammar.txt");
});

// load the grammar from file
const loadBtn = document.getElementById("load");
loadBtn.addEventListener("click", () => {
    const fileInput = document.getElementById("fileInput");
    fileInput.click();
});