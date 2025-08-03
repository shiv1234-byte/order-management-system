#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <chrono>

using namespace std;

// Product model
class Product {
public:
    string id, name;
    double price;
    int stock;

    Product(string i, string n, double p, int s) : id(i), name(n), price(p), stock(s) {}
};

// Order model
class Order {
public:
    string orderId;
    unordered_map<string, int> items;
    string status = "Pending";

    Order(string id) : orderId(id) {}
};

// Services
class InventoryService {
public:
    unordered_map<string, Product> products;

    void addProduct(Product p) {
        products[p.id] = p;
    }

    void listProducts() {
        cout << "Available Products:\n";
        for (auto& p : products) {
            cout << "- " << p.second.id << ": " << p.second.name << " (₹" << p.second.price << ", Stock: " << p.second.stock << ")\n";
        }
    }

    bool isAvailable(string pid, int qty) {
        return products.count(pid) && products[pid].stock >= qty;
    }

    void reduceStock(string pid, int qty) {
        products[pid].stock -= qty;
    }
};

class CartService {
public:
    unordered_map<string, int> cart;

    void addToCart(string pid, int qty) {
        cart[pid] += qty;
    }

    void showCart(InventoryService& inv) {
        cout << "\n🛒 Your Cart:\n";
        for (auto& item : cart) {
            auto& p = inv.products[item.first];
            cout << "- " << p.name << " x" << item.second << "\n";
        }
    }

    unordered_map<string, int> getItems() {
        return cart;
    }

    void clearCart() {
        cart.clear();
    }

    bool isEmpty() {
        return cart.empty();
    }
};

class ShippingService {
public:
    void shipOrder(Order& order) {
        cout << "🚚 Shipping order: " << order.orderId << "...\n";
        this_thread::sleep_for(chrono::seconds(2));
        order.status = "Shipped";
        cout << "✅ Order " << order.orderId << " shipped!\n\n";
    }
};

class OrderService {
public:
    int orderCounter = 1;

    Order createOrder(CartService& cart, InventoryService& inv) {
        string id = "ORD" + to_string(orderCounter++);
        Order order(id);
        for (auto& item : cart.getItems()) {
            order.items[item.first] = item.second;
            inv.reduceStock(item.first, item.second);
        }
        return order;
    }
};

// Main application loop
int main() {
    InventoryService inventory;
    CartService cart;
    OrderService orderService;
    ShippingService shipping;

    // Seed inventory
    inventory.addProduct(Product("P1", "T-Shirt", 499.0, 10));
    inventory.addProduct(Product("P2", "Jeans", 999.0, 5));
    inventory.addProduct(Product("P3", "Sneakers", 1999.0, 3));

    int choice;
    do {
        cout << "\n====== Order Management System ======\n";
        cout << "1. View Products\n";
        cout << "2. Add to Cart\n";
        cout << "3. View Cart\n";
        cout << "4. Place Order\n";
        cout << "5. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            inventory.listProducts();
            break;
        case 2: {
            string pid;
            int qty;
            cout << "Enter Product ID: ";
            cin >> pid;
            cout << "Enter quantity: ";
            cin >> qty;

            if (inventory.isAvailable(pid, qty)) {
                cart.addToCart(pid, qty);
                cout << "✅ Added to cart.\n";
            } else {
                cout << "❌ Product not available or insufficient stock.\n";
            }
            break;
        }
        case 3:
            if (cart.isEmpty())
                cout << "🛒 Your cart is empty.\n";
            else
                cart.showCart(inventory);
            break;
        case 4:
            if (cart.isEmpty()) {
                cout << "⚠️ Cannot place order. Your cart is empty.\n";
            } else {
                Order order = orderService.createOrder(cart, inventory);
                cout << "📦 Order placed with ID: " << order.orderId << "\n";
                shipping.shipOrder(order);
                cart.clearCart();
            }
            break;
        case 5:
            cout << "👋 Exiting... Thank you!\n";
            break;
        default:
            cout << "❓ Invalid option. Try again.\n";
        }

    } while (choice != 5);

    return 0;
}
