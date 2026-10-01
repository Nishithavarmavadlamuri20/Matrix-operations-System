// =====================================================
// MATRIX OPERATION SYSTEM
// FRONTEND JAVASCRIPT
// =====================================================

let selectedOp = "add";


const operationNames = {

    add: "A + B",

    sub: "A - B",

    mul: "A × B",

    transA: "Transpose A",

    transB: "Transpose B",

    detA: "Determinant A",

    detB: "Determinant B"

};


// =====================================================
// CREATE DROPDOWN VALUES
// =====================================================

function fillSelect(id)
{
    const select =
        document.getElementById(id);

    if (!select)
        return;

    select.innerHTML = "";

    for (let i = 1; i <= 6; i++)
    {
        const option =
            document.createElement("option");

        option.value = i;

        option.textContent = i;

        select.appendChild(option);
    }

    select.value = 3;
}


fillSelect("rowsA");
fillSelect("colsA");
fillSelect("rowsB");
fillSelect("colsB");


// =====================================================
// CREATE MATRIX
// =====================================================

function createMatrix(letter)
{
    const rows =
        Number(
            document.getElementById(
                "rows" + letter
            ).value
        );

    const cols =
        Number(
            document.getElementById(
                "cols" + letter
            ).value
        );


    const box =
        document.getElementById(
            "matrix" + letter
        );


    if (!box)
        return;


    box.innerHTML = "";

    box.style.display = "grid";

    box.style.gridTemplateColumns =
        `repeat(${cols}, 72px)`;

    box.style.gap = "6px";


    for (let i = 0; i < rows; i++)
    {
        for (let j = 0; j < cols; j++)
        {
            const input =
                document.createElement("input");


            input.type = "number";

            input.value = 0;

            input.id =
                `${letter}_${i}_${j}`;

            input.className =
                "matrix-value";


            box.appendChild(input);
        }
    }
}


// Initial matrices

createMatrix("A");

createMatrix("B");


// =====================================================
// MATRIX SIZE CHANGE
// =====================================================

document
    .getElementById("rowsA")
    .addEventListener(
        "change",
        () => createMatrix("A")
    );


document
    .getElementById("colsA")
    .addEventListener(
        "change",
        () => createMatrix("A")
    );


document
    .getElementById("rowsB")
    .addEventListener(
        "change",
        () => createMatrix("B")
    );


document
    .getElementById("colsB")
    .addEventListener(
        "change",
        () => createMatrix("B")
    );


// =====================================================
// OPERATION BUTTONS
// =====================================================

document
    .querySelectorAll(".operation")
    .forEach(button =>
    {
        button.addEventListener(
            "click",
            function()
            {
                document
                    .querySelectorAll(".operation")
                    .forEach(btn =>
                    {
                        btn.classList.remove(
                            "selected"
                        );
                    });


                this.classList.add(
                    "selected"
                );


                selectedOp =
                    this.dataset.op;


                document
                    .getElementById(
                        "operationText"
                    )
                    .innerHTML =
                    "Operation: <b>" +
                    operationNames[selectedOp] +
                    "</b>";
            }
        );
    });


// =====================================================
// GET MATRIX
// =====================================================

function getMatrix(letter)
{
    const rows =
        Number(
            document.getElementById(
                "rows" + letter
            ).value
        );


    const cols =
        Number(
            document.getElementById(
                "cols" + letter
            ).value
        );


    const matrix = [];


    for (let i = 0; i < rows; i++)
    {
        const row = [];


        for (let j = 0; j < cols; j++)
        {
            const input =
                document.getElementById(
                    `${letter}_${i}_${j}`
                );


            row.push(
                Number(input.value) || 0
            );
        }


        matrix.push(row);
    }


    return matrix;
}


// =====================================================
// SHOW RESULT
// =====================================================

function showResult(result)
{
    const resultGrid =
        document.getElementById(
            "resultGrid"
        );


    if (!resultGrid)
        return;


    resultGrid.innerHTML = "";


    // Scalar result
    // Used for determinant

    if (!Array.isArray(result))
    {
        const value =
            document.createElement("div");


        value.className =
            "scalar-result";


        value.textContent =
            Number(result).toFixed(4);


        resultGrid.appendChild(
            value
        );


        return;
    }


    // Matrix result

    if (result.length === 0)
        return;


    const rows =
        result.length;


    const cols =
        result[0].length;


    resultGrid.style.display =
        "grid";


    resultGrid.style.gridTemplateColumns =
        `repeat(${cols}, 75px)`;


    resultGrid.style.gap =
        "6px";


    for (let i = 0; i < rows; i++)
    {
        for (let j = 0; j < cols; j++)
        {
            const cell =
                document.createElement("div");


            cell.className =
                "result-cell";


            cell.textContent =
                Number(
                    result[i][j]
                ).toFixed(2);


            resultGrid.appendChild(
                cell
            );
        }
    }
}


// =====================================================
// CALCULATE
// =====================================================

async function calculate()
{
    try
    {
        const A =
            getMatrix("A");


        const B =
            getMatrix("B");


        // -------------------------------
        // Addition / Subtraction
        // -------------------------------

        if (
            (
                selectedOp === "add" ||
                selectedOp === "sub"
            )
            &&
            (
                A.length !== B.length ||
                A[0].length !== B[0].length
            )
        )
        {
            alert(
                "For addition/subtraction, Matrix A and Matrix B must have the same dimensions."
            );

            return;
        }


        // -------------------------------
        // Multiplication
        // -------------------------------

        if (
            selectedOp === "mul" &&
            A[0].length !== B.length
        )
        {
            alert(
                "For multiplication, columns of Matrix A must equal rows of Matrix B."
            );

            return;
        }


        // -------------------------------
        // Determinant A
        // -------------------------------

        if (
            selectedOp === "detA" &&
            A.length !== A[0].length
        )
        {
            alert(
                "Determinant requires a square Matrix A."
            );

            return;
        }


        // -------------------------------
        // Determinant B
        // -------------------------------

        if (
            selectedOp === "detB" &&
            B.length !== B[0].length
        )
        {
            alert(
                "Determinant requires a square Matrix B."
            );

            return;
        }


        // -------------------------------
        // Send to C++ backend
        // -------------------------------

        const response =
            await fetch(
                "/calculate",
                {
                    method: "POST",

                    headers:
                    {
                        "Content-Type":
                            "application/json"
                    },

                    body:
                        JSON.stringify(
                        {
                            operation:
                                selectedOp,

                            A: A,

                            B: B
                        })
                }
            );


        const data =
            await response.json();


        if (
            !response.ok ||
            data.error
        )
        {
            alert(
                data.error ||
                "Calculation failed."
            );

            return;
        }


        let result;


        // Matrix result

        if (
            data.result !== undefined
        )
        {
            result =
                data.result;
        }


        // Determinant result

        else if (
            data.scalar !== undefined
        )
        {
            result =
                data.scalar;
        }


        else if (
            data.value !== undefined
        )
        {
            result =
                data.value;
        }


        else
        {
            result = data;
        }


        showResult(result);


        document
            .getElementById(
                "operationText"
            )
            .innerHTML =
            "Operation: <b>" +
            operationNames[selectedOp] +
            "</b>";
    }


    catch (error)
    {
        console.error(error);


        alert(
            "Unable to connect to the C++ backend.\n\n" +
            "Make sure backend.exe is running."
        );
    }
}


// =====================================================
// FOCUS MATRIX
// =====================================================

function focusMatrix(letter)
{
    const firstInput =
        document.querySelector(
            `#matrix${letter} input`
        );


    if (firstInput)
    {
        firstInput.focus();
    }
}


// =====================================================
// CLEAR MATRIX
// =====================================================

function clearMatrix(letter)
{
    const box =
        document.getElementById(
            `matrix${letter}`
        );


    if (!box)
        return;


    box
        .querySelectorAll("input")
        .forEach(input =>
        {
            input.value = 0;
        });
}


// =====================================================
// RESET SYSTEM
// =====================================================

function resetSystem()
{
    ["A", "B"].forEach(
        letter =>
        {
            document
                .getElementById(
                    `rows${letter}`
                )
                .value = 3;


            document
                .getElementById(
                    `cols${letter}`
                )
                .value = 3;


            createMatrix(letter);
        }
    );


    selectedOp = "add";


    document
        .querySelectorAll(".operation")
        .forEach(button =>
        {
            button.classList.remove(
                "selected"
            );
        });


    document
        .querySelector(
            '.operation[data-op="add"]'
        )
        ?.classList.add(
            "selected"
        );


    document
        .getElementById(
            "operationText"
        )
        .innerHTML =
        "Operation: <b>A + B</b>";


    document
        .getElementById(
            "resultGrid"
        )
        .innerHTML = "";
}


// =====================================================
// MAKE FUNCTIONS AVAILABLE TO HTML
// =====================================================

window.calculate =
    calculate;

window.focusMatrix =
    focusMatrix;

window.clearMatrix =
    clearMatrix;

window.resetSystem =
    resetSystem;


console.log(
    "Matrix Operation System JavaScript loaded successfully."
);