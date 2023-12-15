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
    const sentence = document.getElementById("sentenceInput").value.replace(/\+/g, '%2B');
    const grammar = document.getElementById("grammarInput").value.replace(/\+/g, '%2B');
    console.log(sentence);
    console.log(grammar);
    fetch("http://127.0.0.1:8080/grammar", {
        method: "POST",
        // x-www-form-urlencoded
        headers: {"Content-Type": "application/x-www-form-urlencoded"},
        body: `grammar=${grammar}&sentence=${sentence}`,
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
                firstSet.innerText = row[1].map((x) =>
                    String.fromCharCode(x)
                );
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
                followSet.innerText = row[1].map((x) =>
                    String.fromCharCode(x)
                );
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
                    /*
                     "dotPos": 0,
                    "left": 70,
                    "right": "(E)"

                    means F -> .E
                     */
                    const stateContent = document.createElement("td");
                    const stateContentList = document.createElement("ul");
                    for (const item of state) {
                        const stateContentItem = document.createElement("li");
                        stateContentItem.innerText = `${String.fromCharCode(
                            item.left
                        )} -> ${item.right.slice(
                            0,
                            item.dotPos
                        )}.${item.right.slice(item.dotPos)}`;
                        stateContentList.appendChild(stateContentItem);
                    }
                    stateContent.appendChild(stateContentList);
                    tr.appendChild(stateContent);
                    DFAStateTable.appendChild(tr);
                }
                slr1.appendChild(DFAStateTable);

            } else {
                // just say no in

                const slr1 = document.getElementById("slr1");
                slr1.innerHTML = "";
                const h2 = document.createElement("h2");
                h2.innerText = "Is SLR(1)?";
                slr1.appendChild(h2);
                const p = document.createElement("p");
                p.innerText = "No";
                slr1.appendChild(p);
            }
        });
});
