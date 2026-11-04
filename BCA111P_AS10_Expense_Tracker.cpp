// Smart Digital Expense Tracker with Budget Prediction
// BCA I Semester - Lab Sheet 09

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <limits>
using namespace std;

struct Expense {
    int id;
    string date;        // Format: YYYY-MM-DD
    string category;
    string description;
    double amount;
};

const string EXPENSE_FILE = "expenses.txt";
const string USER_FILE = "users.txt";
const string BUDGET_FILE = "budget.txt";

// Read expenses from the file.
vector<Expense> loadExpenses() {
    vector<Expense> expenses;
    ifstream file(EXPENSE_FILE);
    Expense e;
    while (file >> e.id) {
        file.ignore(numeric_limits<streamsize>::max(), '|');
        getline(file, e.date, '|');
        getline(file, e.category, '|');
        getline(file, e.description, '|');
        file >> e.amount;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        expenses.push_back(e);
    }
    return expenses;
}

// Save all expenses to the file.
void saveExpenses(const vector<Expense>& expenses) {
    ofstream file(EXPENSE_FILE);
    for (const Expense& e : expenses) {
        file << e.id << '|' << e.date << '|' << e.category << '|'
             << e.description << '|' << fixed << setprecision(2)
             << e.amount << '\n';
    }
}

// Registration and login are simple educational examples.
// Passwords are stored as plain text, so this is not production security.
bool registerUser() {
    string username, password;
    cout << "Create username (no spaces): ";
    cin >> username;
    cout << "Create password (no spaces): ";
    cin >> password;

    ifstream in(USER_FILE);
    string u, p;
    while (in >> u >> p) {
        if (u == username) {
            cout << "Username already exists.\n";
            return false;
        }
    }
    ofstream outFile(USER_FILE, ios::app);
    outFile << username << ' ' << password << '\n';
    cout << "Registration successful.\n";
    return true;
}

bool loginUser() {
    string username, password, u, p;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    ifstream file(USER_FILE);
    while (file >> u >> p) {
        if (u == username && p == password) {
            cout << "Login successful. Welcome, " << username << "!\n";
            return true;
        }
    }
    cout << "Invalid username or password.\n";
    return false;
}

double loadBudget() {
    ifstream file(BUDGET_FILE);
    double budget = 0;
    if (file >> budget) return budget;
    return 0;
}

void saveBudget(double budget) {
    ofstream file(BUDGET_FILE);
    file << fixed << setprecision(2) << budget;
}

string getCurrentMonth() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char month[8];
    strftime(month, sizeof(month), "%Y-%m", local);
    return string(month);
}

int daysInCurrentMonth() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    int year = local->tm_year + 1900;
    int month = local->tm_mon + 1;
    if (month == 2) {
        bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        return leap ? 29 : 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    return 31;
}

double monthTotal(const vector<Expense>& expenses, const string& month) {
    double total = 0;
    for (const Expense& e : expenses) {
        if (e.date.size() >= 7 && e.date.substr(0, 7) == month)
            total += e.amount;
    }
    return total;
}

void addExpense() {
    vector<Expense> expenses = loadExpenses();
    Expense e;
    int nextId = 1;
    for (const Expense& old : expenses)
        if (old.id >= nextId) nextId = old.id + 1;

    e.id = nextId;
    cout << "Enter date (YYYY-MM-DD): ";
    cin >> e.date;
    cout << "Enter category (Food/Travel/Study/Other): ";
    cin >> e.category;
    cout << "Enter short description (use_underscores_for_spaces): ";
    cin >> e.description;
    cout << "Enter amount (Rs.): ";
    while (!(cin >> e.amount) || e.amount <= 0) {
        cout << "Enter a valid positive amount: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    expenses.push_back(e);
    saveExpenses(expenses);
    cout << "Expense saved. Expense ID: " << e.id << '\n';

    double budget = loadBudget();
    double total = monthTotal(expenses, getCurrentMonth());
    if (budget > 0 && total > budget)
        cout << "WARNING: You have exceeded this month's budget!\n";
    else if (budget > 0 && total >= budget * 0.8)
        cout << "Budget warning: You have used at least 80% of your budget.\n";
}

void viewHistory() {
    vector<Expense> expenses = loadExpenses();
    if (expenses.empty()) {
        cout << "No expenses recorded yet.\n";
        return;
    }
    cout << "\nID   Date        Category     Description                 Amount\n";
    cout << "---------------------------------------------------------------\n";
    for (const Expense& e : expenses) {
        cout << left << setw(5) << e.id << setw(12) << e.date
             << setw(13) << e.category << setw(28) << e.description
             << "Rs. " << fixed << setprecision(2) << e.amount << '\n';
    }
}

void categoryReport() {
    vector<Expense> expenses = loadExpenses();
    string month = getCurrentMonth();
    vector<string> categories;
    vector<double> totals;

    for (const Expense& e : expenses) {
        if (e.date.size() < 7 || e.date.substr(0, 7) != month) continue;
        int i = 0;
        while (i < (int)categories.size() && categories[i] != e.category) i++;
        if (i == (int)categories.size()) {
            categories.push_back(e.category);
            totals.push_back(e.amount);
        } else {
            totals[i] += e.amount;
        }
    }

    cout << "\nCategory-wise spending for " << month << ":\n";
    if (categories.empty()) cout << "No expenses for this month.\n";
    for (int i = 0; i < (int)categories.size(); i++)
        cout << categories[i] << ": Rs. " << fixed << setprecision(2) << totals[i] << '\n';
}

void monthlyReport() {
    vector<Expense> expenses = loadExpenses();
    string month = getCurrentMonth();
    double total = monthTotal(expenses, month);
    double budget = loadBudget();

    cout << "\nMonthly Expense Report (" << month << ")\n";
    cout << "Total spent: Rs. " << fixed << setprecision(2) << total << '\n';
    cout << "Monthly budget: Rs. " << budget << '\n';
    cout << "Remaining budget: Rs. " << budget - total << '\n';
    if (budget > 0 && total > budget)
        cout << "Status: Budget exceeded.\n";
    else if (budget > 0)
        cout << "Status: Within budget.\n";
    else
        cout << "Status: Set a budget to monitor spending.\n";
}

void setBudget() {
    double budget;
    cout << "Enter monthly budget (Rs.): ";
    while (!(cin >> budget) || budget <= 0) {
        cout << "Enter a valid positive budget: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    saveBudget(budget);
    cout << "Monthly budget saved.\n";
}

void predictExpense() {
    vector<Expense> expenses = loadExpenses();
    string month = getCurrentMonth();
    double total = monthTotal(expenses, month);
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    int daysPassed = local->tm_mday;
    int daysTotal = daysInCurrentMonth();
    double predicted = (total / daysPassed) * daysTotal;
    double budget = loadBudget();

    cout << "\nPrediction for " << month << '\n';
    cout << "Spent so far: Rs. " << fixed << setprecision(2) << total << '\n';
    cout << "Estimated month-end spending: Rs. " << predicted << '\n';
    cout << "This is a simple estimate based on average daily spending.\n";
    if (budget > 0 && predicted > budget)
        cout << "Alert: You may exceed your monthly budget.\n";
    else if (budget > 0)
        cout << "Prediction: You may stay within your budget.\n";
    else
        cout << "Set a monthly budget to compare your prediction.\n";
}

void searchExpense() {
    vector<Expense> expenses = loadExpenses();
    string key;
    cout << "Enter date, category, or description to search (exact text): ";
    cin >> key;
    bool found = false;
    for (const Expense& e : expenses) {
        if (e.date.find(key) != string::npos ||
            e.category.find(key) != string::npos ||
            e.description.find(key) != string::npos) {
            cout << "ID: " << e.id << ", Date: " << e.date
                 << ", Category: " << e.category
                 << ", Description: " << e.description
                 << ", Amount: Rs. " << fixed << setprecision(2) << e.amount << '\n';
            found = true;
        }
    }
    if (!found) cout << "No matching expense found.\n";
}

void deleteExpense() {
    vector<Expense> expenses = loadExpenses();
    int id;
    cout << "Enter expense ID to delete: ";
    if (!(cin >> id)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID.\n";
        return;
    }

    bool found = false;
    for (int i = 0; i < (int)expenses.size(); i++) {
        if (expenses[i].id == id) {
            expenses.erase(expenses.begin() + i);
            found = true;
            break;
        }
    }
    if (found) {
        saveExpenses(expenses);
        cout << "Expense deleted.\n";
    } else {
        cout << "Expense ID not found.\n";
    }
}

int main() {
    int choice;
    cout << "========================================\n";
    cout << " SMART DIGITAL EXPENSE TRACKER\n";
    cout << "========================================\n";

    while (true) {
        cout << "\n1. Register\n2. Login\n3. Exit\nChoose: ";
        if (!(cin >> choice)) return 0;
        if (choice == 1) registerUser();
        else if (choice == 2) {
            if (loginUser()) break;
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            return 0;
        } else cout << "Invalid choice.\n";
    }

    do {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Add Daily Expense\n";
        cout << "2. View Expense History\n";
        cout << "3. Category-wise Expense\n";
        cout << "4. Monthly Expense Report\n";
        cout << "5. Set Monthly Budget\n";
        cout << "6. Budget Warning / Check\n";
        cout << "7. Expense Prediction\n";
        cout << "8. Search Expense\n";
        cout << "9. Delete Expense\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
            case 1: addExpense(); break;
            case 2: viewHistory(); break;
            case 3: categoryReport(); break;
            case 4: monthlyReport(); break;
            case 5: setBudget(); break;
            case 6: monthlyReport(); break;
            case 7: predictExpense(); break;
            case 8: searchExpense(); break;
            case 9: deleteExpense(); break;
            case 0: cout << "Thank you for using the tracker.\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
