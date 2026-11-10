#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <cctype>

using namespace std;

// ==================== HELPERS ====================
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return (char)tolower(c); });
    return s;
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

int readInt(const string& prompt, int minVal, int maxVal) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size() && v >= minVal && v <= maxVal)
                return v;
        } catch (...) {}
        cout << "Invalid input. Enter a number from " << minVal << " to " << maxVal << ".\n";
    }
}

double readDouble(const string& prompt, double minVal) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            double v = stod(s, &pos);
            if (pos == s.size() && v >= minVal)
                return v;
        } catch (...) {}
        cout << "Invalid input. Enter a number >= " << minVal << ".\n";
    }
}

void pressEnter() {
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

// ==================== PRODUCT STRUCT ====================
struct Product {
    int id;
    string name;
    string category;
    string brand;
    double price;
    int stockQuantity;
};

void printTable(const vector<Product>& list) {
    if (list.empty()) {
        cout << "\nNo watches found.\n";
        return;
    }
    cout << "\n" << string(92, '-') << '\n';
    cout << left << setw(5) << "ID" << setw(34) << "Name" << setw(14) << "Category"
         << setw(14) << "Brand" << right << setw(10) << "Price(PHP)" << setw(8) << "Stock" << '\n';
    cout << string(92, '-') << '\n';
    for (const Product& p : list) {
        cout << left << setw(5) << p.id << setw(34) << p.name.substr(0, 32)
             << setw(14) << p.category << setw(14) << p.brand
             << right << setw(10) << fixed << setprecision(2) << p.price
             << setw(8) << p.stockQuantity << '\n';
    }
    cout << string(92, '-') << '\n';
    cout << list.size() << " watch(es) listed.\n";
}

// ==================== SHOPPING CART: DOUBLY LINKED LIST ====================
struct CartNode {
    int productId;
    int quantity;
    CartNode* next;
    CartNode* prev;

    CartNode(int id, int qty)
        : productId(id), quantity(qty), next(nullptr), prev(nullptr) {}
};

class ShoppingCart {
private:
    CartNode* head;
    CartNode* tail;
    CartNode* current;  // navigation cursor

public:
    ShoppingCart() : head(nullptr), tail(nullptr), current(nullptr) {}
    ~ShoppingCart() { clear(); }

    void addItem(int productId, int quantity) {
        for (CartNode* n = head; n != nullptr; n = n->next) {
            if (n->productId == productId) {
                n->quantity += quantity;
                current = n;
                cout << "Quantity updated in cart.\n";
                return;
            }
        }
        CartNode* node = new CartNode(productId, quantity);
        if (head == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        current = node;
        cout << "Watch added to cart successfully.\n";
    }

    int quantityOf(int productId) const {
        for (CartNode* n = head; n != nullptr; n = n->next)
            if (n->productId == productId) return n->quantity;
        return 0;
    }

    bool moveNext() {
        if (current && current->next) { current = current->next; return true; }
        return false;
    }

    bool movePrev() {
        if (current && current->prev) { current = current->prev; return true; }
        return false;
    }

    void removeCurrent() {
        if (current == nullptr) {
            cout << "Cart is empty.\n";
            return;
        }
        CartNode* target = current;
        if (target->prev) target->prev->next = target->next; else head = target->next;
        if (target->next) target->next->prev = target->prev; else tail = target->prev;
        current = target->next ? target->next : target->prev;
        delete target;
        cout << "Watch removed from cart.\n";
    }

    bool isEmpty() const { return head == nullptr; }
    CartNode* getHead() const { return head; }
    CartNode* getCurrent() const { return current; }

    void clear() {
        CartNode* n = head;
        while (n != nullptr) {
            CartNode* nxt = n->next;
            delete n;
            n = nxt;
        }
        head = tail = current = nullptr;
    }
};

// ==================== BST FOR PRODUCT ID SEARCH ====================
struct BSTNode {
    Product product;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const Product& p) : product(p), left(nullptr), right(nullptr) {}
};

BSTNode* insertBST(BSTNode* root, const Product& product) {
    if (root == nullptr) return new BSTNode(product);
    if (product.id < root->product.id)
        root->left = insertBST(root->left, product);
    else if (product.id > root->product.id)
        root->right = insertBST(root->right, product);
    return root;
}

BSTNode* searchBST(BSTNode* root, int id) {
    if (root == nullptr || root->product.id == id) return root;
    if (id < root->product.id) return searchBST(root->left, id);
    return searchBST(root->right, id);
}

void deleteBST(BSTNode* root) {
    if (root == nullptr) return;
    deleteBST(root->left);
    deleteBST(root->right);
    delete root;
}

// ==================== CIRCULAR LINKED LIST: BRANDS ====================
struct BrandNode {
    string brand;
    BrandNode* next;

    BrandNode(const string& b) : brand(b), next(nullptr) {}
};

class BrandShowcase {
private:
    BrandNode* tail;

public:
    BrandShowcase() : tail(nullptr) {}
    ~BrandShowcase() { clear(); }

    void addBrand(const string& brand) {
        BrandNode* node = new BrandNode(brand);
        if (tail == nullptr) {
            tail = node;
            tail->next = tail;
        } else {
            node->next = tail->next;
            tail->next = node;
            tail = node;
        }
    }

    BrandNode* first() const { return tail ? tail->next : nullptr; }

    void clear() {
        if (tail == nullptr) return;
        BrandNode* head = tail->next;
        tail->next = nullptr;
        while (head != nullptr) {
            BrandNode* nxt = head->next;
            delete head;
            head = nxt;
        }
        tail = nullptr;
    }
};

// ==================== STACK: UNDO STOCK CHANGE ====================
enum ActionType { ACTION_ADDED, ACTION_RESTOCKED, ACTION_REMOVED };

struct AdminAction {
    ActionType type;
    Product product;  // snapshot (for ADDED/REMOVED) or id holder (for RESTOCKED)
    int quantity;     // restock amount
};

// ==================== QUEUE: CHECKOUT ORDERS ====================
struct OrderItem {
    int productId;
    int quantity;
};

struct Order {
    int orderId;
    vector<OrderItem> items;
};

// ==================== WATCH STORE ====================
class WatchStore {
private:
    vector<Product> inventory;
    BSTNode* root;
    ShoppingCart cart;
    queue<Order> checkoutQueue;
    stack<AdminAction> undoStack;
    BrandShowcase showcase;
    int nextOrderId;

    void rebuildBST() {
        deleteBST(root);
        root = nullptr;
        for (const Product& p : inventory)
            root = insertBST(root, p);
    }

    Product* findById(int id) {
        BSTNode* node = searchBST(root, id);
        if (node == nullptr) return nullptr;
        for (Product& p : inventory)
            if (p.id == id) return &p;
        return nullptr;
    }

    int nextProductId() const {
        int mx = 0;
        for (const Product& p : inventory) mx = max(mx, p.id);
        return mx + 1;
    }

    bool brandKnown(const string& brand) const {
        for (const Product& p : inventory)
            if (toLower(p.brand) == toLower(brand)) return true;
        return false;
    }

    void seedData() {
        inventory = {
            {101, "Pablo Raez Men's Stainless Steel Watch", "Quartz",   "Pablo Raez", 363.00, 40},
            {102, "TOMI Men's Dual Display Green Watch",    "Sporty",   "TOMI",       344.00, 25},
            {103, "BINBOND Luxury Men's Quartz Watch",      "Luxury",   "BINBOND",    296.00, 30},
            {104, "Men's Quartz Graffiti Dial Watch",       "Fashion",  "Generic",    191.00, 50},
            {105, "Casio F-91W Classic Digital",            "Digital",  "Casio",      1295.00, 20},
            {106, "Casio G-Shock GA-2100",                  "Sporty",   "Casio",      6995.00, 8},
            {107, "Seiko 5 Sports Automatic",               "Automatic","Seiko",      14500.00, 6},
            {108, "Citizen Eco-Drive Chronograph",          "Solar",    "Citizen",    16800.00, 5},
            {109, "Fossil Grant Chronograph Leather",       "Fashion",  "Fossil",     7995.00, 10},
            {110, "Timex Weekender 38mm",                   "Casual",   "Timex",      3995.00, 15},
            {111, "BINBOND Gold Tone Chronograph Look",     "Luxury",   "BINBOND",    312.00, 35},
            {112, "TOMI Women's Minimalist Mesh Watch",     "Casual",   "TOMI",       279.00, 28}
        };
        rebuildBST();

        // build unique brand list for the circular banner
        vector<string> seen;
        for (const Product& p : inventory) {
            bool found = false;
            for (const string& b : seen)
                if (b == p.brand) { found = true; break; }
            if (!found) {
                seen.push_back(p.brand);
                showcase.addBrand(p.brand);
            }
        }
    }

    // ---------- 1. VIEW COMPLETE CATALOG ----------
    void viewCatalog() {
        cout << "\n=========== COMPLETE WATCH CATALOG (with Brand Affiliations) ===========";
        printTable(inventory);
    }

    // ---------- 2. FILTER BY BRAND ----------
    void filterByBrand() {
        cout << "\nAvailable brands: ";
        BrandNode* start = showcase.first();
        if (start) {
            BrandNode* n = start;
            do {
                cout << n->brand << (n->next != start ? ", " : "");
                n = n->next;
            } while (n != start);
        }
        cout << '\n';
        string brand = toLower(readLine("Enter brand name: "));
        vector<Product> result;
        for (const Product& p : inventory)
            if (toLower(p.brand) == brand) result.push_back(p);
        printTable(result);
    }

    // ---------- 3. SEARCH BY KEYWORD ----------
    void searchByKeyword() {
        string key = toLower(readLine("\nEnter keyword (name, category or brand): "));
        if (key.empty()) {
            cout << "Keyword cannot be empty.\n";
            return;
        }
        vector<Product> result;
        for (const Product& p : inventory) {
            if (toLower(p.name).find(key) != string::npos ||
                toLower(p.category).find(key) != string::npos ||
                toLower(p.brand).find(key) != string::npos)
                result.push_back(p);
        }
        printTable(result);
    }

    // ---------- 4. FILTER BY PRICE RANGE (BINARY SEARCH) ----------
    static int lowerBoundPrice(const vector<Product>& v, double target) {
        int lo = 0, hi = (int)v.size();
        while (lo < hi) {
            int mid = lo + (hi - lo) / 2;
            if (v[mid].price < target) lo = mid + 1;
            else hi = mid;
        }
        return lo;
    }

    static int upperBoundPrice(const vector<Product>& v, double target) {
        int lo = 0, hi = (int)v.size();
        while (lo < hi) {
            int mid = lo + (hi - lo) / 2;
            if (v[mid].price <= target) lo = mid + 1;
            else hi = mid;
        }
        return lo;
    }

    void filterByPriceRange() {
        double minP = readDouble("\nMinimum price (PHP): ", 0);
        double maxP = readDouble("Maximum price (PHP): ", 0);
        if (minP > maxP) swap(minP, maxP);

        vector<Product> sorted = inventory;
        sort(sorted.begin(), sorted.end(),
             [](const Product& a, const Product& b) { return a.price < b.price; });

        int lo = lowerBoundPrice(sorted, minP);
        int hi = upperBoundPrice(sorted, maxP);

        vector<Product> result(sorted.begin() + lo, sorted.begin() + hi);
        cout << "\nBinary search found range [" << lo << ", " << hi << ") in price-sorted list.";
        printTable(result);
    }

    // ---------- 5. ADD TO CART (DOUBLY LINKED LIST NAVIGATION) ----------
    void showCartCurrent() {
        CartNode* cur = cart.getCurrent();
        if (cur == nullptr) {
            cout << "\n[Cart is empty]\n";
            return;
        }
        Product* p = findById(cur->productId);
        cout << "\n>> Current item: ";
        if (p)
            cout << p->name << " (" << p->brand << ") x" << cur->quantity
                 << " @ PHP " << fixed << setprecision(2) << p->price << '\n';
        else
            cout << "Unknown product #" << cur->productId << '\n';
    }

    void viewCart() {
        if (cart.isEmpty()) {
            cout << "\nYour cart is empty.\n";
            return;
        }
        double total = 0;
        cout << "\n=============== SHOPPING CART ===============\n";
        for (CartNode* n = cart.getHead(); n != nullptr; n = n->next) {
            Product* p = findById(n->productId);
            if (!p) continue;
            double sub = p->price * n->quantity;
            total += sub;
            cout << (n == cart.getCurrent() ? " > " : "   ")
                 << left << setw(36) << p->name.substr(0, 34)
                 << "x" << setw(4) << n->quantity
                 << right << setw(12) << fixed << setprecision(2) << sub << '\n';
        }
        cout << string(48, '-') << '\n';
        cout << left << setw(40) << "   TOTAL (PHP)" << right << setw(12) << total << '\n';
    }

    void cartMenu() {
        while (true) {
            cout << "\n------ SHOPPING CART (Doubly Linked List) ------\n";
            showCartCurrent();
            cout << "1. Add watch by ID\n"
                 << "2. Next item  -->\n"
                 << "3. Previous item  <--\n"
                 << "4. Remove current item\n"
                 << "5. View full cart\n"
                 << "6. Back to main menu\n";
            int c = readInt("Choice: ", 1, 6);
            if (c == 1) {
                printTable(inventory);
                int id = readInt("Enter watch ID: ", 1, 1000000);
                Product* p = findById(id);
                if (!p) {
                    cout << "Watch ID not found.\n";
                    continue;
                }
                int qty = readInt("Quantity: ", 1, 1000);
                if (cart.quantityOf(id) + qty > p->stockQuantity) {
                    cout << "Not enough stock. Available: " << p->stockQuantity << '\n';
                    continue;
                }
                cart.addItem(id, qty);
            } else if (c == 2) {
                if (!cart.moveNext()) cout << "Already at the last item (or cart empty).\n";
            } else if (c == 3) {
                if (!cart.movePrev()) cout << "Already at the first item (or cart empty).\n";
            } else if (c == 4) {
                cart.removeCurrent();
            } else if (c == 5) {
                viewCart();
            } else {
                break;
            }
        }
    }

    // ---------- 6. CHECKOUT QUEUE (FIFO) ----------
    void viewPendingOrders() {
        if (checkoutQueue.empty()) {
            cout << "\nNo pending orders.\n";
            return;
        }
        queue<Order> copy = checkoutQueue;
        cout << "\n--- Pending Orders (front = next to be served) ---\n";
        int pos = 1;
        while (!copy.empty()) {
            const Order& o = copy.front();
            cout << pos++ << ". Order #" << o.orderId << " - " << o.items.size() << " item(s)\n";
            copy.pop();
        }
    }

    void processNextOrder() {
        if (checkoutQueue.empty()) {
            cout << "\nNo orders to process.\n";
            return;
        }
        Order o = checkoutQueue.front();
        checkoutQueue.pop();

        cout << "\n========== PROCESSING ORDER #" << o.orderId << " ==========\n";
        double total = 0;
        for (const OrderItem& it : o.items) {
            Product* p = findById(it.productId);
            if (!p) {
                cout << "Product #" << it.productId << " no longer exists. Skipped.\n";
                continue;
            }
            int qty = min(it.quantity, p->stockQuantity);
            if (qty < it.quantity)
                cout << "Warning: only " << qty << " of " << p->name << " available.\n";
            p->stockQuantity -= qty;
            double sub = qty * p->price;
            total += sub;
            cout << left << setw(36) << p->name.substr(0, 34) << "x" << setw(4) << qty
                 << right << setw(12) << fixed << setprecision(2) << sub << '\n';
        }
        cout << string(52, '-') << '\n';
        cout << left << setw(40) << "TOTAL (PHP)" << right << setw(12) << total << '\n';
        cout << "Order completed. Thank you for shopping!\n";
        rebuildBST();
    }

    void checkoutMenu() {
        while (true) {
            cout << "\n------ CUSTOMER CHECKOUT QUEUE (FIFO) ------\n"
                 << "1. Place order (move cart to queue)\n"
                 << "2. Process next order in queue\n"
                 << "3. View pending orders\n"
                 << "4. Back to main menu\n";
            int c = readInt("Choice: ", 1, 4);
            if (c == 1) {
                if (cart.isEmpty()) {
                    cout << "Cart is empty. Add watches first (option 5).\n";
                    continue;
                }
                Order o;
                o.orderId = nextOrderId++;
                for (CartNode* n = cart.getHead(); n != nullptr; n = n->next)
                    o.items.push_back({n->productId, n->quantity});
                checkoutQueue.push(o);
                cart.clear();
                cout << "Order #" << o.orderId << " placed in the checkout queue.\n";
            } else if (c == 2) {
                processNextOrder();
            } else if (c == 3) {
                viewPendingOrders();
            } else {
                break;
            }
        }
    }

    // ---------- 7. ADMIN UNDO (LIFO STACK POP) ----------
    void adminUndo() {
        if (undoStack.empty()) {
            cout << "\nNothing to undo.\n";
            return;
        }
        AdminAction a = undoStack.top();
        undoStack.pop();

        switch (a.type) {
        case ACTION_ADDED: {
            for (size_t i = 0; i < inventory.size(); i++) {
                if (inventory[i].id == a.product.id) {
                    inventory.erase(inventory.begin() + i);
                    break;
                }
            }
            cout << "Undo: removed newly added watch '" << a.product.name << "'.\n";
            break;
        }
        case ACTION_RESTOCKED: {
            Product* p = nullptr;
            for (Product& x : inventory)
                if (x.id == a.product.id) { p = &x; break; }
            if (p) {
                p->stockQuantity = max(0, p->stockQuantity - a.quantity);
                cout << "Undo: reverted restock of " << a.quantity << " for '" << p->name << "'.\n";
            } else {
                cout << "Undo skipped: product no longer exists.\n";
            }
            break;
        }
        case ACTION_REMOVED: {
            inventory.push_back(a.product);
            sort(inventory.begin(), inventory.end(),
                 [](const Product& x, const Product& y) { return x.id < y.id; });
            cout << "Undo: restored removed watch '" << a.product.name << "'.\n";
            break;
        }
        }
        rebuildBST();
    }

    // ---------- 8. PARTNER BRAND BANNER SHOWCASE (CIRCULAR LIST) ----------
    void brandShowcaseMenu() {
        BrandNode* cur = showcase.first();
        if (cur == nullptr) {
            cout << "\nNo partner brands available.\n";
            return;
        }
        cout << "\n====== PARTNER BRAND BANNER SHOWCASE (Circular List) ======\n"
             << "Press Enter to cycle to the next brand, or type Q then Enter to stop.\n";
        while (true) {
            int count = 0;
            for (const Product& p : inventory)
                if (p.brand == cur->brand) count++;
            cout << "\n   *****************************************\n"
                 << "   *   FEATURED BRAND:  " << left << setw(18) << cur->brand << "*\n"
                 << "   *   Watches in store: " << left << setw(17) << count << "*\n"
                 << "   *****************************************\n";
            string in = readLine("[Enter] next  |  [Q] quit: ");
            if (!in.empty() && (in[0] == 'q' || in[0] == 'Q')) break;
            cur = cur->next;  // wraps around automatically
        }
    }

    // ---------- 9. SORT INVENTORY DISPLAY ----------
    void sortInventory() {
        cout << "\nSort by:\n1. Price (low to high)\n2. Price (high to low)\n"
             << "3. Stock (low to high)\n4. Brand (A-Z)\n";
        int c = readInt("Choice: ", 1, 4);
        vector<Product> v = inventory;
        switch (c) {
        case 1: sort(v.begin(), v.end(), [](const Product& a, const Product& b) { return a.price < b.price; }); break;
        case 2: sort(v.begin(), v.end(), [](const Product& a, const Product& b) { return a.price > b.price; }); break;
        case 3: sort(v.begin(), v.end(), [](const Product& a, const Product& b) { return a.stockQuantity < b.stockQuantity; }); break;
        case 4: sort(v.begin(), v.end(), [](const Product& a, const Product& b) {
                    if (toLower(a.brand) != toLower(b.brand)) return toLower(a.brand) < toLower(b.brand);
                    return a.name < b.name; }); break;
        }
        printTable(v);
    }

    // ---------- 11. ADMIN: ADD / RESTOCK / REMOVE ----------
    void adminMenu() {
        while (true) {
            cout << "\n------ [ADMIN] WATCH INVENTORY MANAGEMENT ------\n"
                 << "1. Add new watch\n"
                 << "2. Restock existing watch\n"
                 << "3. Remove watch\n"
                 << "4. Back to main menu\n";
            int c = readInt("Choice: ", 1, 4);
            if (c == 1) {
                Product p;
                p.id = nextProductId();
                p.name = readLine("Watch name: ");
                p.category = readLine("Category (Quartz/Digital/Automatic/...): ");
                p.brand = readLine("Brand: ");
                if (p.name.empty() || p.brand.empty()) {
                    cout << "Name and brand are required.\n";
                    continue;
                }
                p.price = readDouble("Price (PHP): ", 0);
                p.stockQuantity = readInt("Initial stock: ", 0, 100000);
                bool newBrand = !brandKnown(p.brand);
                inventory.push_back(p);
                if (newBrand) showcase.addBrand(p.brand);
                undoStack.push({ACTION_ADDED, p, 0});
                rebuildBST();
                cout << "Added watch with ID " << p.id << ".\n";
            } else if (c == 2) {
                printTable(inventory);
                int id = readInt("Enter watch ID to restock: ", 1, 1000000);
                Product* p = findById(id);
                if (!p) { cout << "Watch ID not found.\n"; continue; }
                int qty = readInt("Quantity to add: ", 1, 100000);
                p->stockQuantity += qty;
                undoStack.push({ACTION_RESTOCKED, *p, qty});
                cout << "Restocked. New stock for " << p->name << ": " << p->stockQuantity << '\n';
            } else if (c == 3) {
                printTable(inventory);
                int id = readInt("Enter watch ID to remove: ", 1, 1000000);
                bool found = false;
                for (size_t i = 0; i < inventory.size(); i++) {
                    if (inventory[i].id == id) {
                        undoStack.push({ACTION_REMOVED, inventory[i], 0});
                        cout << "Removed '" << inventory[i].name << "'.\n";
                        inventory.erase(inventory.begin() + i);
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Watch ID not found.\n";
                else rebuildBST();
            } else {
                break;
            }
        }
    }

    void printMenu() {
        string line(78, '=');
        cout << "\n" << line << '\n'
             << "                          YOUR WATCH SHOP\n"
             << "            BRAND-AFFILIATED WATCH INVENTORY & SEARCH SYSTEM\n"
             << line << '\n'
             << " 1. View Complete Catalog (Tabular View with Brand Affiliations)\n"
             << " 2. Filter Products by Official Brand (e.g., Casio, Seiko, Citizen)\n"
             << " 3. Search Products by Keyword\n"
             << " 4. Filter Products by Price Range (Binary Search)\n"
             << " 5. Add Item to Shopping Cart (Doubly Linked List Navigation)\n"
             << " 6. Process Customer Checkout Queue (FIFO Order Queue)\n"
             << " 7. Perform Admin Undo Action (LIFO Stack Pop)\n"
             << " 8. Display Partner Brand Banner Showcase (Circular List Cycle)\n"
             << " 9. Sort Inventory Display (By Price, Stock, or Brand)\n"
             << "10. Exit\n"
             << "11. [Admin] Add / Restock / Remove Watches\n"
             << line << '\n';
    }

public:
    WatchStore() : root(nullptr), nextOrderId(1) { seedData(); }
    ~WatchStore() { deleteBST(root); }

    void run() {
        while (true) {
            printMenu();
            int choice = readInt("Enter choice: ", 1, 11);
            switch (choice) {
            case 1: viewCatalog(); break;
            case 2: filterByBrand(); break;
            case 3: searchByKeyword(); break;
            case 4: filterByPriceRange(); break;
            case 5: cartMenu(); break;
            case 6: checkoutMenu(); break;
            case 7: adminUndo(); break;
            case 8: brandShowcaseMenu(); break;
            case 9: sortInventory(); break;
            case 10:
                cout << "\nThank you for visiting Your Watch Shop. Goodbye!\n";
                return;
            case 11: adminMenu(); break;
            }
            pressEnter();
        }
    }
};

// ==================== MAIN ====================
int main() {
    WatchStore store;
    store.run();
    return 0;
}