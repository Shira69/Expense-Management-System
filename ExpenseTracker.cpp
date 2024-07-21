#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <cctype>

class Expense {
public:
    int id;
    std::string category;
    std::string description;
    double amount;
    std::string date;

    Expense(int id, const std::string &category, const std::string &description, double amount, const std::string &date)
        : id(id), category(category), description(description), amount(amount), date(date) {}
};

class ExpenseTracker {
private:
    std::unordered_map<std::string, std::string> users;
    std::vector<Expense> expenses;
    std::unordered_set<std::string> categories;
    int nextExpenseId;
    std::string currentUser;
    double monthlyBudget;

    // Load users from the file
    bool loadUsers() {
        std::ifstream file("users.txt");
        if (!file.is_open()) return false;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string username, password;
            if (!(iss >> username >> password)) continue;
            users[username] = password;
        }
        file.close();
        return true;
    }

    // Save users to the file
    bool saveUsers() {
        std::ofstream file("users.txt");
        if (!file.is_open()) return false;
        for (const auto &pair : users) {
            file << pair.first << " " << pair.second << "\n";
        }
        file.close();
        return true;
    }

    // Load expenses from the file
    bool loadExpenses() {
        std::ifstream file("expenses.txt");
        if (!file.is_open()) return false;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            int id;
            std::string category, description, date;
            double amount;
            if (!(iss >> id >> category >> description >> amount >> date)) continue;
            expenses.push_back(Expense(id, category, description, amount, date));
            categories.insert(category);
            if (id >= nextExpenseId) nextExpenseId = id + 1;
        }
        file.close();
        return true;
    }

    // Save expenses to the file
    bool saveExpenses() {
        std::ofstream file("expenses.txt");
        if (!file.is_open()) return false;
        for (const auto &expense : expenses) {
            file << expense.id << " " << expense.category << " " << expense.description << " " << expense.amount << " " << expense.date << "\n";
        }
        file.close();
        return true;
    }

    // Generate a new unique ID for expenses
    int generateExpenseId() {
        return nextExpenseId++;
    }

    // Validate the date format
    bool validateDate(const std::string &date) {
        std::regex datePattern("\\d{4}-\\d{2}-\\d{2}");
        if (!std::regex_match(date, datePattern)) {
            return false;
        }
        int year, month, day;
        char delim1, delim2;
        std::istringstream iss(date);
        if (!(iss >> year >> delim1 >> month >> delim2 >> day)) {
            return false;
        }
        if (delim1 != '-' || delim2 != '-') {
            return false;
        }
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
            return false;
        }
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (day > (isLeap ? 29 : 28)) {
                return false;
            }
        }
        return true;
    }

    // Validate the username
    bool validateUsername(const std::string &username) {
        if (username.length() < 4) {
            std::cout << "Username must be at least 4 characters long.\n";
            return false;
        }
        if (!std::all_of(username.begin(), username.end(), ::isalnum)) {
            std::cout << "Username can only contain alphanumeric characters.\n";
            return false;
        }
        return true;
    }

    // Validate the password
    bool validatePassword(const std::string &password) {
        if (password.length() < 8) {
            std::cout << "Password must be at least 8 characters long.\n";
            return false;
        }
        if (!std::any_of(password.begin(), password.end(), ::isupper)) {
            std::cout << "Password must contain at least one uppercase letter.\n";
            return false;
        }
        if (!std::any_of(password.begin(), password.end(), ::islower)) {
            std::cout << "Password must contain at least one lowercase letter.\n";
            return false;
        }
        if (!std::any_of(password.begin(), password.end(), ::isdigit)) {
            std::cout << "Password must contain at least one number.\n";
            return false;
        }
        return true;
    }

public:
    ExpenseTracker() : nextExpenseId(1), monthlyBudget(0) {
        loadUsers();
        loadExpenses();
    }

    // Handle user login
    bool login(const std::string &username, const std::string &password) {
        if (users.find(username) != users.end() && users[username] == password) {
            currentUser = username;
            return true;
        }
        return false;
    }

    // Handle user registration
    bool registerUser(const std::string &username, const std::string &password) {
        if (!validateUsername(username) || !validatePassword(password)) {
            return false;
        }
        if (users.find(username) != users.end()) {
            std::cout << "Username already exists. Please try again.\n";
            return false;
        }
        users[username] = password;
        saveUsers();
        return true;
    }

    // Add an expense
    void addExpense(const std::string &category, const std::string &description, double amount, std::string &date) {
        while (!validateDate(date)) {
            std::cout << "Invalid date format. Please use YYYY-MM-DD.\n";
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::getline(std::cin, date);
        }
        
        expenses.push_back(Expense(generateExpenseId(), category, description, amount, date));
        categories.insert(category);
        saveExpenses();
    }

    // Add multiple expenses
    void addMultipleExpenses() {
        int numExpenses;
        std::cout << "Enter the number of expenses you want to add: ";
        std::cin >> numExpenses;
        std::cin.ignore(); // Clear the newline character left in the buffer

        for (int i = 0; i < numExpenses; ++i) {
            std::string category, description, date;
            double amount;
            std::cout << "Expense " << (i + 1) << ":\n";
            std::cout << "Enter category: ";
            std::getline(std::cin, category);
            std::cout << "Enter description: ";
            std::getline(std::cin, description);
            std::cout << "Enter amount: ";
            std::cin >> amount;
            std::cin.ignore(); // Clear the newline character left in the buffer
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::getline(std::cin, date);

            addExpense(category, description, amount, date);
        }
    }

    // View all expenses
    void viewExpenses() {
        for (const auto &expense : expenses) {
            std::cout << "ID: " << expense.id << ", Category: " << expense.category << ", Description: " << expense.description << ", Amount: " << expense.amount << ", Date: " << expense.date << "\n";
        }
    }

    // Delete an expense by ID
    void deleteExpense(int id) {
        auto it = std::remove_if(expenses.begin(), expenses.end(), [id](const Expense &expense) {
            return expense.id == id;
        });
        if (it != expenses.end()) {
            expenses.erase(it, expenses.end());
            saveExpenses();
        }
    }

    // Search expenses by query
    void searchExpenses(const std::string &query) {
        for (const auto &expense : expenses) {
            if (expense.category.find(query) != std::string::npos || expense.description.find(query) != std::string::npos || expense.date.find(query) != std::string::npos) {
                std::cout << "ID: " << expense.id << ", Category: " << expense.category << ", Description: " << expense.description << ", Amount: " << expense.amount << ", Date: " << expense.date << "\n";
            }
        }
    }

    // Generate a report of total expenses
    void generateReport() {
        double total = 0;
        for (const auto &expense : expenses) {
            total += expense.amount;
        }
        std::cout << "Total Expenses: " << total << "\n";
    }

    // Calculate total expense in a date range
    void calculateTotalExpenseInRange(const std::string &startDate, const std::string &endDate) {
        if (!validateDate(startDate) || !validateDate(endDate)) {
            std::cout << "Invalid date format. Please use YYYY-MM-DD.\n";
            return;
        }
        double total = 0;
        for (const auto &expense : expenses) {
            if (expense.date >= startDate && expense.date <= endDate) {
                total += expense.amount;
            }
        }
        std::cout << "Total Expenses from " << startDate << " to " << endDate << ": " << total << "\n";
    }

    // Manage categories (view, add, delete)
    void manageCategories() {
        int choice;
        while (true) {
            std::cout << "\nManage Categories:\n";
            std::cout << "1. View Categories\n";
            std::cout << "2. Add Category\n";
            std::cout << "3. Delete Category\n";
            std::cout << "4. Exit\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            std::cin.ignore(); // Clear the newline character left in the buffer

            switch (choice) {
            case 1:
                viewCategories();
                break;
            case 2:
                addCategory();
                break;
            case 3:
                deleteCategory();
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    // View all categories
    void viewCategories() {
        std::cout << "Categories:\n";
        for (const auto &category : categories) {
            std::cout << category << "\n";
        }
    }

    // Add a new category
    void addCategory() {
        std::string category;
        std::cout << "Enter category name: ";
        std::getline(std::cin, category);
        categories.insert(category);
    }

    // Delete a category
    void deleteCategory() {
        std::string category;
        std::cout << "Enter category name to delete: ";
        std::getline(std::cin, category);
        categories.erase(category);
    }

    // Set the monthly budget
    void setMonthlyBudget(double budget) {
        monthlyBudget = budget;
    }

    // View the monthly budget
    void viewMonthlyBudget() {
        std::cout << "Monthly Budget: " << monthlyBudget << "\n";
    }

    // Analyze expenses against the monthly budget
    void analyzeExpensesAgainstBudget() {
        double total = 0;
        for (const auto &expense : expenses) {
            total += expense.amount;
        }
        std::cout << "Total Expenses: " << total << "\n";
        std::cout << "Monthly Budget: " << monthlyBudget << "\n";
        if (total > monthlyBudget) {
            std::cout << "You have exceeded your monthly budget by " << (total - monthlyBudget) << "\n";
        } else {
            std::cout << "You are within your monthly budget.\n";
        }
    }
};

int main() {
    ExpenseTracker tracker;
    int choice;
    std::string username, password;

    while (true) {
        std::cout << "Welcome to Expense Tracker\n";
        std::cout << "1. Register\n";
        std::cout << "2. Login\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline character left in the buffer

        switch (choice) {
        case 1:
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            if (tracker.registerUser(username, password)) {
                std::cout << "Registration successful. Please login.\n";
            }
            break;
        case 2:
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            if (tracker.login(username, password)) {
                std::cout << "Login successful. Welcome, " << username << "!\n";
                goto mainMenu; // Exit the login loop and proceed to the main menu
            } else {
                std::cout << "Invalid username or password. Please try again.\n";
            }
            break;
        case 3:
            std::cout << "Exiting...\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

mainMenu:
    while (true) {
        std::cout << "\nMain Menu:\n";
        std::cout << "1. Add Expense\n";
        std::cout << "2. Add Multiple Expenses\n";
        std::cout << "3. View Expenses\n";
        std::cout << "4. Delete Expense\n";
        std::cout << "5. Search Expenses\n";
        std::cout << "6. Generate Report\n";
        std::cout << "7. Calculate Total Expense in Range\n";
        std::cout << "8. Manage Categories\n";
        std::cout << "9. Set Monthly Budget\n";
        std::cout << "10. View Monthly Budget\n";
        std::cout << "11. Analyze Expenses Against Budget\n";
        std::cout << "12. Logout\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline character left in the buffer

        switch (choice) {
        case 1: {
            std::string category, description, date;
            double amount;
            std::cout << "Enter category: ";
            std::getline(std::cin, category);
            std::cout << "Enter description: ";
            std::getline(std::cin, description);
            std::cout << "Enter amount: ";
            std::cin >> amount;
            std::cin.ignore(); // Clear the newline character left in the buffer
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::getline(std::cin, date);

            tracker.addExpense(category, description, amount, date);
            break;
        }
        case 2:
            tracker.addMultipleExpenses();
            break;
        case 3:
            tracker.viewExpenses();
            break;
        case 4: {
            int id;
            std::cout << "Enter expense ID to delete: ";
            std::cin >> id;
            std::cin.ignore(); // Clear the newline character left in the buffer
            tracker.deleteExpense(id);
            break;
        }
        case 5: {
            std::string query;
            std::cout << "Enter search query (category/description/date): ";
            std::getline(std::cin, query);
            tracker.searchExpenses(query);
            break;
        }
        case 6:
            tracker.generateReport();
            break;
        case 7: {
            std::string startDate, endDate;
            std::cout << "Enter start date (YYYY-MM-DD): ";
            std::getline(std::cin, startDate);
            std::cout << "Enter end date (YYYY-MM-DD): ";
            std::getline(std::cin, endDate);
            tracker.calculateTotalExpenseInRange(startDate, endDate);
            break;
        }
        case 8:
            tracker.manageCategories();
            break;
        case 9: {
            double budget;
            std::cout << "Enter monthly budget: ";
            std::cin >> budget;
            std::cin.ignore(); // Clear the newline character left in the buffer
            tracker.setMonthlyBudget(budget);
            break;
        }
        case 10:
            tracker.viewMonthlyBudget();
            break;
        case 11:
            tracker.analyzeExpensesAgainstBudget();
            break;
        case 12:
            std::cout << "Logging out...\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
