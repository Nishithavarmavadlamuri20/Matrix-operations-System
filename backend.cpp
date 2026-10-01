#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include "httplib.h"

using namespace std;

using Matrix = vector<vector<double>>;


// =====================================================
// MATRIX ADDITION
// =====================================================

Matrix addition(const Matrix& A, const Matrix& B)
{
    Matrix result = A;

    for (size_t i = 0; i < A.size(); i++)
    {
        for (size_t j = 0; j < A[i].size(); j++)
        {
            result[i][j] = A[i][j] + B[i][j];
        }
    }

    return result;
}


// =====================================================
// MATRIX SUBTRACTION
// =====================================================

Matrix subtraction(const Matrix& A, const Matrix& B)
{
    Matrix result = A;

    for (size_t i = 0; i < A.size(); i++)
    {
        for (size_t j = 0; j < A[i].size(); j++)
        {
            result[i][j] = A[i][j] - B[i][j];
        }
    }

    return result;
}


// =====================================================
// MATRIX MULTIPLICATION
// =====================================================

Matrix multiplication(const Matrix& A, const Matrix& B)
{
    int rowsA = A.size();
    int colsA = A[0].size();
    int rowsB = B.size();
    int colsB = B[0].size();

    if (colsA != rowsB)
    {
        throw runtime_error(
            "For multiplication, columns of A must equal rows of B."
        );
    }

    Matrix result(
        rowsA,
        vector<double>(colsB, 0)
    );

    for (int i = 0; i < rowsA; i++)
    {
        for (int j = 0; j < colsB; j++)
        {
            for (int k = 0; k < colsA; k++)
            {
                result[i][j] +=
                    A[i][k] * B[k][j];
            }
        }
    }

    return result;
}


// =====================================================
// TRANSPOSE
// =====================================================

Matrix transpose(const Matrix& A)
{
    int rows = A.size();
    int cols = A[0].size();

    Matrix result(
        cols,
        vector<double>(rows)
    );

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result[j][i] = A[i][j];
        }
    }

    return result;
}


// =====================================================
// DETERMINANT
// =====================================================

double determinant(const Matrix& A)
{
    int n = A.size();

    if (n == 1)
    {
        return A[0][0];
    }

    if (n == 2)
    {
        return
            A[0][0] * A[1][1]
            -
            A[0][1] * A[1][0];
    }

    double det = 0;

    for (int col = 0; col < n; col++)
    {
        Matrix minorMatrix;

        for (int i = 1; i < n; i++)
        {
            vector<double> row;

            for (int j = 0; j < n; j++)
            {
                if (j != col)
                {
                    row.push_back(A[i][j]);
                }
            }

            minorMatrix.push_back(row);
        }

        double sign =
            (col % 2 == 0) ? 1.0 : -1.0;

        det +=
            sign *
            A[0][col] *
            determinant(minorMatrix);
    }

    return det;
}


// =====================================================
// GET JSON STRING
// =====================================================

string getJsonString(
    const string& json,
    const string& key
)
{
    string search =
        "\"" + key + "\"";

    size_t pos =
        json.find(search);

    if (pos == string::npos)
    {
        return "";
    }

    pos =
        json.find(":", pos);

    if (pos == string::npos)
    {
        return "";
    }

    pos++;

    while (
        pos < json.size() &&
        (json[pos] == ' ' ||
         json[pos] == '"')
    )
    {
        pos++;
    }

    size_t end =
        json.find(
            '"',
            pos
        );

    if (end == string::npos)
    {
        return "";
    }

    return json.substr(
        pos,
        end - pos
    );
}


// =====================================================
// PARSE MATRIX FROM JSON
// =====================================================

Matrix parseMatrix(
    const string& json,
    const string& key
)
{
    Matrix matrix;

    string search =
        "\"" + key + "\"";

    size_t keyPos =
        json.find(search);

    if (keyPos == string::npos)
    {
        return matrix;
    }

    size_t start =
        json.find(
            "[[",
            keyPos
        );

    if (start == string::npos)
    {
        return matrix;
    }

    size_t end =
        json.find(
            "]]",
            start
        );

    if (end == string::npos)
    {
        return matrix;
    }

    string content =
        json.substr(
            start + 2,
            end - start - 2
        );

    stringstream rowsStream(
        content
    );

    string row;

    while (
        getline(
            rowsStream,
            row,
            ']'
        )
    )
    {
        size_t open =
            row.find('[');

        if (open != string::npos)
        {
            row =
                row.substr(
                    open + 1
                );
        }

        if (
            !row.empty() &&
            row[0] == ','
        )
        {
            row.erase(0, 1);
        }

        if (row.empty())
        {
            continue;
        }

        stringstream valuesStream(
            row
        );

        string value;

        vector<double> currentRow;

        while (
            getline(
                valuesStream,
                value,
                ','
            )
        )
        {
            try
            {
                currentRow.push_back(
                    stod(value)
                );
            }
            catch (...)
            {
            }
        }

        if (!currentRow.empty())
        {
            matrix.push_back(
                currentRow
            );
        }
    }

    return matrix;
}


// =====================================================
// MATRIX TO JSON
// =====================================================

string matrixToJSON(
    const Matrix& matrix
)
{
    stringstream output;

    output << "[";

    for (
        size_t i = 0;
        i < matrix.size();
        i++
    )
    {
        output << "[";

        for (
            size_t j = 0;
            j < matrix[i].size();
            j++
        )
        {
            output
                << fixed
                << setprecision(4)
                << matrix[i][j];

            if (
                j + 1 <
                matrix[i].size()
            )
            {
                output << ",";
            }
        }

        output << "]";

        if (
            i + 1 <
            matrix.size()
        )
        {
            output << ",";
        }
    }

    output << "]";

    return output.str();
}


// =====================================================
// SEND HTML FILE
// =====================================================

void sendHTML(
    httplib::Response& res,
    const string& filename
)
{
    res.set_file_content(
        filename,
        "text/html"
    );
}


// =====================================================
// MAIN SERVER
// =====================================================

int main()
{
    httplib::Server server;


    // =================================================
    // HOME
    // =================================================

    server.Get(
        "/",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            sendHTML(
                res,
                "index.html"
            );
        }
    );


    // =================================================
    // ABOUT PAGE
    // =================================================

    server.Get(
        "/about.html",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            sendHTML(
                res,
                "about.html"
            );
        }
    );


    // =================================================
    // TEAM PAGE
    // =================================================

    server.Get(
        "/team.html",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            sendHTML(
                res,
                "team.html"
            );
        }
    );


    // =================================================
    // FORMULAS PAGE
    // =================================================

    server.Get(
        "/formulas.html",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            sendHTML(
                res,
                "formulas.html"
            );
        }
    );


    // =================================================
    // CSS
    // =================================================

    server.Get(
        "/style.css",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            res.set_file_content(
                "style.css",
                "text/css"
            );
        }
    );


    // =================================================
    // JAVASCRIPT
    // =================================================

    server.Get(
        "/script.js",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            res.set_file_content(
                "script.js",
                "application/javascript"
            );
        }
    );


    // =================================================
    // CALCULATE API
    // =================================================

    server.Post(
        "/calculate",
        [](const httplib::Request& req,
           httplib::Response& res)
        {
            try
            {
                Matrix A =
                    parseMatrix(
                        req.body,
                        "A"
                    );

                Matrix B =
                    parseMatrix(
                        req.body,
                        "B"
                    );

                string operation =
                    getJsonString(
                        req.body,
                        "operation"
                    );


                if (A.empty())
                {
                    throw runtime_error(
                        "Matrix A is empty."
                    );
                }


                Matrix result;


                // =================================
                // ADDITION
                // =================================

                if (
                    operation == "add"
                )
                {
                    if (
                        A.size() != B.size() ||
                        A[0].size() != B[0].size()
                    )
                    {
                        throw runtime_error(
                            "For addition, both matrices must have the same dimensions."
                        );
                    }

                    result =
                        addition(
                            A,
                            B
                        );
                }


                // =================================
                // SUBTRACTION
                // =================================

                else if (
                    operation == "sub"
                )
                {
                    if (
                        A.size() != B.size() ||
                        A[0].size() != B[0].size()
                    )
                    {
                        throw runtime_error(
                            "For subtraction, both matrices must have the same dimensions."
                        );
                    }

                    result =
                        subtraction(
                            A,
                            B
                        );
                }


                // =================================
                // MULTIPLICATION
                // =================================

                else if (
                    operation == "mul"
                )
                {
                    result =
                        multiplication(
                            A,
                            B
                        );
                }


                // =================================
                // TRANSPOSE A
                // =================================

                else if (
                    operation == "transA"
                )
                {
                    result =
                        transpose(A);
                }


                // =================================
                // TRANSPOSE B
                // =================================

                else if (
                    operation == "transB"
                )
                {
                    if (B.empty())
                    {
                        throw runtime_error(
                            "Matrix B is empty."
                        );
                    }

                    result =
                        transpose(B);
                }


                // =================================
                // DETERMINANT A
                // =================================

                else if (
                    operation == "detA"
                )
                {
                    if (
                        A.size() !=
                        A[0].size()
                    )
                    {
                        throw runtime_error(
                            "Determinant requires a square matrix."
                        );
                    }

                    double answer =
                        determinant(A);

                    stringstream json;

                    json
                        << "{\"success\":true,"
                        << "\"scalar\":"
                        << fixed
                        << setprecision(4)
                        << answer
                        << "}";

                    res.set_content(
                        json.str(),
                        "application/json"
                    );

                    return;
                }


                // =================================
                // DETERMINANT B
                // =================================

                else if (
                    operation == "detB"
                )
                {
                    if (B.empty())
                    {
                        throw runtime_error(
                            "Matrix B is empty."
                        );
                    }

                    if (
                        B.size() !=
                        B[0].size()
                    )
                    {
                        throw runtime_error(
                            "Determinant requires a square matrix."
                        );
                    }

                    double answer =
                        determinant(B);

                    stringstream json;

                    json
                        << "{\"success\":true,"
                        << "\"scalar\":"
                        << fixed
                        << setprecision(4)
                        << answer
                        << "}";

                    res.set_content(
                        json.str(),
                        "application/json"
                    );

                    return;
                }


                // =================================
                // INVALID OPERATION
                // =================================

                else
                {
                    throw runtime_error(
                        "Invalid operation selected."
                    );
                }


                // =================================
                // SEND MATRIX RESULT
                // =================================

                string response =
                    "{\"success\":true,"
                    "\"result\":" +
                    matrixToJSON(result) +
                    "}";

                res.set_content(
                    response,
                    "application/json"
                );
            }


            catch (
                const exception& e
            )
            {
                string error =
                    "{\"success\":false,"
                    "\"error\":\"" +
                    string(e.what()) +
                    "\"}";

                res.status = 400;

                res.set_content(
                    error,
                    "application/json"
                );
            }
        }
    );


    // =================================================
    // START SERVER
    // =================================================

    cout << endl;
    cout << "========================================" << endl;
    cout << "       MATRIX OPERATION SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "Backend : C++" << endl;
    cout << "Server  : http://localhost:8080" << endl;
    cout << "========================================" << endl;
    cout << "Press CTRL + C to stop." << endl;
    cout << endl;


    server.listen(
        "localhost",
        8080
    );


    return 0;
}